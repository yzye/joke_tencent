#ifndef _QA_MCD_PROC_H_
#define _QA_MCD_PROC_H_

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <iostream>

#include "tfc_cache_proc.h"
#include "tfc_net_ccd_define.h"
#include "tfc_base_config_file.h"
#include "tfc_base_str.h"
#include "tfc_base_http.h"
#include "myalloc.h"
#include "qa_log.h"
#include "common/base/scoped_ptr.h"
#include "proto/nlu/dqa_rootmerge.pb.h"
#include "longconn/protocol/base_protocol.h"
#include "comm_db.h"
#include "nlu/sub_service/weather/spider_patternmatch/pattern_component.h"
#include "common/system/concurrency/mutex.h"
#include "qa_common_tools.h"
#include "qa_stat.h"
#include "joke.h"

using namespace std;
using namespace tfc::cache;
using namespace tfc::base;
using namespace pb::dqa_rootmerge;

class PatternCmpnt;

static const unsigned kDataBufMaxSize = (128<<20);

extern void QADispCCD(void * priv);
/*
static void LoadTemplates(string tfile, RegexQuestionDoctor *rqd);
static void ReloadNlpPattern(PatternCmpnt *nlp_ptn);
static Mutex m_regex_lock;

*/
static Mutex m_nlp_lock;

class CQAServ : public CacheProc
{
public:
	CQAServ();
	~CQAServ();
	
    int InitLog();
	int Init(const string & conf_file);
	virtual void run(const std::string & conf_file) { Run(conf_file); }
	
    virtual void Run(const std::string & conf_file);
	int DecodeQuery(char *rbuf, int buf_len, RootServerRequest &request);
    int TokenByNlpTool(string query, vector<JokeAnswer> &ans_vec);
	int DoQuery(string query, vector<JokeAnswer> &ans_vec);
	void ConvertJokeToPb(JokeAnswer p, JokeResponse *resp);
	char *EncodeResult(vector<JokeAnswer> &ans_vec,
                            RootServerRequest request, uint32_t & pack_len);
    int HandleCcdRequest();
	int Enqueue2Ccd(const char * data, int data_len, 
					unsigned long long flow);
	void CheckFlags();
private:
	//int CountWeight(GscAnswer *ans, ResultVec rv,
	//	MYSQL_ROW row, MYSQL_FIELD *fields, unsigned fields_num);
	//string ConvUtf8ToGbk(string utf8_str);
	//string ConvGbkToUtf8(string gbk_str);
	//void DebugPB(GscResponse *p);
	//int GuessTarget(string query, string &target);
private:
	CFifoSyncMQ			*m_mq_ccd_2_mcd;
	CFifoSyncMQ			*m_mq_mcd_2_ccd;
	int 				m_allow_regex;
	TCCDHeader			*m_ccdheader;

	char m_recv_buf[kDataBufMaxSize];
	char m_send_buf[kDataBufMaxSize];
	scoped_ptr<CFileConfig> m_mcd_config;
	xfs::base::CBaseProtocolPack	*m_longconn_pack;
	xfs::base::CBaseProtocolUnpack	*m_longconn_unpack;
	uint16_t m_service_type;
	unsigned m_seq_no;

	PatternCmpnt *m_nlp_ptn;
	JokeInfo *m_joke_dict;
	string m_template_file;
    string m_query_analysis;

	CQAStat *m_qa_stat;
};

#endif
