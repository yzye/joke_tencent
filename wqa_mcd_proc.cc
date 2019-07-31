#include "wqa_mcd_proc.h"

#include <algorithm>
#include "comm_db.h"
#include "common/encoding/charset_converter.h"
#include "common/file/file.h"
#include "common/file/file_change_notifier.h"
#include "common/file/file_define.h"
#include "common/file/local_file.h"
#include "proto/nlu/dqa_artifact.pb.h"
#include "proto/nlu/life_service_query.pb.h"

using namespace std;
using namespace tfc::cache;
using namespace tfc::base;
using namespace pb::dqa_rootmerge;
using namespace common;
using namespace common::internal;

class common::FileChangeNotifier;

CQAServ::CQAServ()
{
	m_allow_regex = 0;
	m_qa_stat = CQAStat::Instance();
}

CQAServ::~CQAServ()
{
}
int CQAServ::InitLog()
{
    LOG_PRINT(LOG_NORMAL, "init log\n");
    int log_level = tfc::base::from_str<int>((*m_mcd_config)["root\\log\\log_level"]);
    int log_type = tfc::base::from_str<int>((*m_mcd_config)["root\\log\\log_type"]);
    string path = (*m_mcd_config)["root\\log\\path"];
    string name_prefix =(*m_mcd_config)["root\\log\\name_prefix"];
    int max_file_size = tfc::base::from_str<int>((*m_mcd_config)["root\\log\\max_file_size"]);
    int max_file_no = tfc::base::from_str<int>((*m_mcd_config)["root\\log\\max_file_no"]);
    
    int ret = 0;
    ret = DEBUG_OPEN(log_level, log_type, path, 
                     name_prefix,max_file_size, max_file_no);
    if(ret)
    {
        LOG_PRINT(LOG_NORMAL, "init log error\n");
        exit(1);
    }
   	LOG_PRINT(LOG_NORMAL, "init log ok\n");
    return 0;
}

int CQAServ::Init(const string & conf_file)
{
	m_mcd_config.reset(new CFileConfig());
	m_mcd_config->Init(conf_file);

	InitLog();
	m_allow_regex = from_str<int>((*m_mcd_config)["root\\allow_regex"]);	
	m_mq_ccd_2_mcd = _mqs["mq_ccd_2_mcd"];
	m_mq_mcd_2_ccd = _mqs["mq_mcd_2_ccd"];

	std::string str_name_prefix = (*m_mcd_config)["root\\stat\\name_prefix"];
	int max_file_size = tfc::base::from_str<int>((*m_mcd_config)["root\\stat\\max_file_size"]);
	int max_file_index =tfc::base::from_str<int>((*m_mcd_config)["root\\stat\\max_file_index"]);
	int timeout_1 = tfc::base::from_str<int>((*m_mcd_config)["root\\stat\\timeout_1"]);
	int timeout_2 = tfc::base::from_str<int>((*m_mcd_config)["root\\stat\\timeout_2"]);
	int timeout_3 = tfc::base::from_str<int>((*m_mcd_config)["root\\stat\\timeout_3"]);

	m_qa_stat->Init(str_name_prefix, max_file_size, max_file_index, 
					timeout_1, timeout_2, timeout_3);
    LOG_PRINT(LOG_NORMAL, "init config ok\n");
}

void CQAServ::Run(const string & conf_file)
{
	//TODO: load configure file
	//No configure file at this time
	Init(conf_file);
	
    string nlp_pattern_file = (*m_mcd_config)["root\\nlp_pattern_file"];
	string nlp_type_file = (*m_mcd_config)["root\\nlp_type_file"];
	string nlp_target_file = (*m_mcd_config)["root\\nlp_target_file"];
	m_nlp_ptn = new PatternCmpnt(nlp_pattern_file, nlp_type_file, nlp_target_file);
	
    if (m_nlp_ptn->init())
	{
		LOG_PRINT(LOG_NORMAL, "NLP init failed\n");
		exit(-1);
	}
	//Function<void ()> check_nlp_func = Bind(ReloadNlpPattern, m_nlp_ptn);
	//FileChangeNotifier::Subscribe(nlp_pattern_file, check_nlp_func, 60);

    string joke_info_file =  (*m_mcd_config)["root\\joke_info_file"];
    string joke_types_file =  (*m_mcd_config)["root\\joke_types_file"];
    m_joke_dict = new JokeInfo(joke_info_file,joke_types_file);
	if(!m_joke_dict->Init())
	{
		LOG_PRINT(LOG_NORMAL, "Joke init failed\n");
		exit(-1);
	}

	m_longconn_pack = new xfs::base::CBaseProtocolPack();
	m_longconn_unpack = new xfs::base::CBaseProtocolUnpack();
	LOG_PRINT(LOG_NORMAL, "CQAServ Has Been Launched\n");
	if (-1 == add_mq_2_epoll(m_mq_ccd_2_mcd, QADispCCD, this))
		LOG_PRINT(LOG_ERROR, "Add input mq m_mq_ccd_2_mcd to EPOLL fail!\n");

	while (!stop)
	{
		run_epoll_4_mq();
		CheckFlags();
	}
    LOG_PRINT(LOG_NORMAL, "init data ok\n");
}

//读取上游的请求
int CQAServ::DecodeQuery(char *rbuf, int buf_len, RootServerRequest &request)
{
	m_longconn_unpack->UntachPackage();
	m_longconn_unpack->AttachPackage((unsigned char*)(rbuf), buf_len);
	m_service_type = m_longconn_unpack->GetServiceType();
	m_seq_no = m_longconn_unpack->GetSeq();
	if (!m_longconn_unpack->Unpack())
	{
		LOG_PRINT(LOG_NORMAL, "Unpack Failed,buf_len is %d\n", buf_len);
		return 0;
	}

	uint16_t key = 0;
	char * content;
	uint32_t content_len;
	if (!m_longconn_unpack->GetVal(key, &content, &content_len))
	{
		LOG_PRINT(LOG_NORMAL, "GetVal Failed\n");
		return 0;
	}

	string data(content, content_len);
	if(!request.ParseFromString(data))
	{
		LOG_PRINT(LOG_NORMAL, "ParseFromString Failed\n");
		return 0;
	}

	return 1;
}


//解析上游请求
int CQAServ::DoQuery(string query, vector<JokeAnswer> &ans_vec)
{
	int ret = 0;
	string query_utf8;
	//ConditionContainer c_container;
	//Target target;
	if (query.length() == 0)
	{
		LOG_PRINT(LOG_NORMAL, "query string is null\n");
		return 0;
	}
	query_utf8 = ConvGbkToUtf8(query);
	StringTrim(&query_utf8, " ");
	LOG_PRINT(LOG_NORMAL, "query is %s\n", query_utf8.c_str());
        
	ret = TokenByNlpTool(query_utf8, ans_vec);
	return ret;
}

char *CQAServ::EncodeResult(vector<JokeAnswer> &ans_vec, 
	  RootServerRequest request, uint32_t & pack_len)
{
	char *pack = NULL;
	RootServerResponse response;
	AnswerItem *answer;
	AnswerSourceMask *ans_source_mask;
	string response_str;
	//Target target = "weather";
	uint64_t i = 0;
	response.set_ret_code(0);
	response.set_bid(request.bid());
	vector<JokeAnswer>::iterator it = ans_vec.begin();
	for (; it != ans_vec.end(); it++)
	{
		string serialized_ans;
		JokeResponse resp;
        answer = response.add_result_list();
        ConvertJokeToPb(*it, &resp);
        if (!resp.SerializeToString(&serialized_ans))
		{
				LOG_PRINT(LOG_NORMAL, "Response Serialize Failed\n");
				serialized_ans = "";
		}
		else
		{
			answer->set_merge_info(serialized_ans);
		}
        LOG_PRINT(LOG_NORMAL,"BEGIN add RESULT %s\n",it->joke_str.c_str());

		//answer->set_ans_type(DQA_STOCK);
        answer->set_ans_type(DQA_JOKE);
        answer->set_ans_id(i++);
        answer->set_ans_text(ConvUtf8ToGbk(it->joke_str));
        answer->set_ans_weight(it->point);
        //answer.question_idx = 
        ans_source_mask = answer->add_ans_source_mask();
        ans_source_mask->set_question_idx(0);
        ans_source_mask->set_source_idx(0);
	}

	LOG_PRINT(LOG_NORMAL, "result size is %u\n", 
				response.result_list_size());

	if (!response.SerializeToString(&response_str))
	{
		LOG_PRINT(LOG_NORMAL, "Response Serialize Failed\n");
		return NULL;
	}

	m_longconn_pack->Init();
	m_longconn_pack->ResetContent();
	m_longconn_pack->SetServiceType(m_service_type);
	m_longconn_pack->SetSeq(m_seq_no);
	m_longconn_pack->SetKey((uint16_t)0,
							reinterpret_cast<uint8_t*>(&(*(response_str.begin()))),
							response_str.size());
	m_longconn_pack->GetPackage((unsigned char **)&pack, &pack_len);
	LOG_PRINT(LOG_NORMAL, "Response Str Is %d, pack_len is %d\n", 
			response_str.size(), pack_len);
	/*
	i = 0;
	const char *response_chars = response_str.c_str();
	for (i; i < response_str.size(); i ++)
	{
		printf("%d ", response_chars[i]);
	}
	printf("\n");
	*/
    LOG_PRINT(LOG_NORMAL,"Response pb :%s\n", response.DebugString().c_str());
   
	return pack;
}


int CQAServ::TokenByNlpTool(string query, vector<JokeAnswer> &ans_vec)  
{
	ResultVec rv;
	TargetVec tv;
    ResultVecIter iter;
	ReplaceAll(&query, " ", "");
	string query_gbk = ConvUtf8ToGbk(query);
	int ret = 0;
	{
        MutexLocker locker(m_nlp_lock);
		ret = m_nlp_ptn->match_query(query_gbk, rv, tv); // match_query
	}
	if (ret == -1 || !rv.size() || !tv.size())
	{
		LOG_PRINT(LOG_NORMAL, "match query failed, ret is %d, \
				  rv.size() is %d, tv.size() is %d\n", 
				  ret, rv.size(), tv.size());
		return 0;
	}

	LOG_PRINT(LOG_NORMAL, "match query success, ret is %d, \
			  rv.size() is %d, tv.size() is %d\n", 
			  ret, rv.size(), tv.size());


    iter = rv.begin();
    string target="";
    string value = "";
    int32_t target_idx = 0 ;
    int32_t ans_size = 0 ; 
    for (; iter != rv.end(); iter ++)
    {
        target  = iter->tag;
        value = iter->value;
        if(target == "joke_type")
        {
            target_idx = m_joke_dict->GetJokeTypeId(value);
            //target_idx = atoi(value.c_str());
            LOG_PRINT(LOG_NORMAL, "get tag  %s value %s type_id %d jokes\n",target.c_str(),value.c_str(), target_idx );
            ans_size += m_joke_dict->GetJokes(target_idx, ans_vec);
        }

    }
    if(ans_size == 0)
    {
        ans_size += m_joke_dict->GetJokes(0, ans_vec);
    }
    LOG_PRINT(LOG_NORMAL, "get %d jokes\n",ans_size );
	return ans_size ;
}


void CQAServ::ConvertJokeToPb(JokeAnswer p, JokeResponse *resp)
{
	resp->set_style(p.joke.style);
	resp->set_joke(p.joke.joke);

}
int CQAServ::HandleCcdRequest()
{
	int ret = 0;
	unsigned data_len, i = 0;
	unsigned long long cur_flow;
	uint32_t pack_len = 0;
	unsigned ccd_header_len = sizeof(TCCDHeader);
	struct timeval start_tv, end_tv;
	TCCDHeader *ccd_header = (TCCDHeader*) m_recv_buf;
	vector<JokeAnswer> ans_vec;
	RootServerRequest request;
	try{
		do {
			string query;
			ans_vec.clear();

			if (++i > 1000) break;
			ret = m_mq_ccd_2_mcd->try_dequeue(m_recv_buf, sizeof(m_recv_buf),
									data_len, cur_flow);
			if (ret < 0)
				LOG_PRINT(LOG_ERROR, "try_dequeue m_mq_ccd_2_mcd failed!\n");

			if (data_len <= 0)
				continue;

			if (data_len < ccd_header_len)
			{
				LOG_PRINT(LOG_NORMAL, "invalid pkt\n");
				continue;
			}
			LOG_PRINT(LOG_NORMAL, "------------new request pkt--------------\n");

			m_qa_stat->AddStatWithNoTime("Request", 0);
			gettimeofday(&start_tv, NULL);
			if (ccd_header->_type != ccd_rsp_data) {
				LOG_PRINT(LOG_NORMAL, "Invalid CCD Header Type\n");
				continue;
			}
			LOG_PRINT(LOG_NORMAL, "data_len:%u, ccd_header_len:%u\n",
					data_len, ccd_header_len);
			if (!DecodeQuery(m_recv_buf + ccd_header_len, 
						data_len - ccd_header_len, request))
			{
				LOG_PRINT(LOG_NORMAL, "Decode Query Failed\n");
				continue;
			}
			
			string target;
			ret = DoQuery(request.question(), ans_vec);
			LOG_PRINT(LOG_NORMAL,"DO QUERY RET %d\n",ret);
            //if (ret)
			//{
			char *data = EncodeResult(ans_vec, request, pack_len);
				/*
				int i = 0;
				for (; i < pack_len; i ++)
					printf("%d ", data[i]);
				printf("\n");
				*/
			if (Enqueue2Ccd(data, pack_len, cur_flow))
			{
				LOG_PRINT(LOG_NORMAL, "Failed To Enqueue Data To CCD\n");
				m_qa_stat->AddStatWithNoTime("Enqueue2Ccd", -1);
			}

			gettimeofday(&end_tv, NULL);
			m_qa_stat->AddStatWithNoTime("Enqueue2Ccd", 0);
			m_qa_stat->AddStatWithNoTime("QueryMatched", ret ? 0 : -1);
			m_qa_stat->AddStat("ProcessTime", 0, start_tv, end_tv);
			//}
			long time_gap = (end_tv.tv_sec - start_tv.tv_sec)*1000 + 
							(end_tv.tv_usec - start_tv.tv_usec)/1000;
			LOG_PRINT(LOG_NORMAL, "time used is %lu ms\n",time_gap); 
		}
		while(!stop && !ret && data_len > 0);
	} catch (exception & ex) {
		LOG_PRINT(LOG_NORMAL, "Exception in dispatch CCD! exception:%s\n", ex.what());
	}
}

int CQAServ::Enqueue2Ccd(const char * data, int data_len,
		unsigned long long flow)
{
    TCCDHeader stCcdHeader;
    stCcdHeader._ip   = 0;
    stCcdHeader._port = 0;
    stCcdHeader._type = ccd_req_data;
    stCcdHeader._arg  = 0;

    memcpy(m_send_buf, &stCcdHeader, sizeof(TCCDHeader));
    memmove(m_send_buf + sizeof(TCCDHeader), data, data_len);

    int ret = -1;
    ret = m_mq_mcd_2_ccd->enqueue(m_send_buf, \
        sizeof(TCCDHeader)+data_len, flow);

    return 0;
}

void CQAServ::CheckFlags()
{
	return;
}

void QADispCCD(void * priv)
{
	CQAServ *qa_serv = (CQAServ*)priv;
	qa_serv->HandleCcdRequest();
}

void ReloadNlpPattern(PatternCmpnt *nlp_ptn)
{
    if (!nlp_ptn) return;

    MutexLocker locker(m_nlp_lock);
    if (!nlp_ptn->reload_dict())
        LOG_PRINT(LOG_NORMAL, "Reload NLP Pattern Success\n");
    else
        LOG_PRINT(LOG_NORMAL, "Reload NLP Pattern Failed\n");
}

extern "C"
{
	CacheProc* create_app()
	{
		return new CQAServ();
	}
}
