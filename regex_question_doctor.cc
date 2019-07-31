
#include "regex_question_doctor.h"
#include "comm_db.h"
#include "qa_log.h"
#include "common/file/file.h"
#include "common/file/local_file.h"
#include "common/file/file_define.h"
#include "common/encoding/charset_converter.h"
#include "common/file/file_change_notifier.h"

using namespace common;
//RegexQuestionDoctor *RegexQuestionDoctor::instance;
RegexQuestionDoctor::RegexQuestionDoctor(string pattern_file)
{
	if (m_allow_token)
		LoadRind();
	Function<void ()> check_regex_func = Bind(LoadRegexPattern, 
									pattern_file, this);
	FileChangeNotifier::Subscribe(pattern_file, check_regex_func, 60);
}
/*
RegexQuestionDoctor * RegexQuestionDoctor::GetInstance()
{
	if (!instance)
		instance = new RegexQuestionDoctor();

	return instance;
}
*/
void RegexQuestionDoctor::AddPattern(Pattern pattern)
{
	pattern.reg = new Regex(pattern.pattern.c_str(), Regex::Options().SetExtended(true));
	pattern_vec.push_back(pattern);
}

void RegexQuestionDoctor::ClearPattern()
{
	pattern_vec.clear();
}

int RegexQuestionDoctor::Tokenizer(string question, vector<string> & fields,
				string & target, string & context)
{
	if (question.length() == 0)
		return 0;

	//const size_t nmatch = 10;
	//regmatch_t pm[10];
	int i = 0;
	vector<Pattern>::iterator it = pattern_vec.begin();
	while (it != pattern_vec.end())
	{
		//int ret = regexec(&it->reg, question.c_str(),nmatch, pm, REG_NOTEOL);
		//string cap1;
		bool ret = it->reg->FullMatch(question.c_str(), &context);
		if (ret)
		{
			fields = it->fields;
			target = it->target;
			StringTrimRight(&context, "的");
			StringTrim(&context, '\n');
			StringTrim(&context, '\t');
			return 1;
		}
		it ++;
		i ++;
	}
	return 0;
}

int RegexQuestionDoctor::MatchQuery(string query,
		ConditionContainer &c_container, Target &target)
{
	if (query.length() == 0)
		return 0;
	StringTrim(&query, '\n');
	StringTrim(&query, '\t');

	MutexLocker locker(m_regex_mutex);

	int i = 0;
	string context;
	vector<string> fields;
	vector<string>::iterator fit;
	vector<Pattern>::iterator it = pattern_vec.begin();
	LOG_PRINT(LOG_NORMAL, "query is %s\n", query.c_str());
	while (it != pattern_vec.end())
	{
		bool ret = it->reg->FullMatch(query.c_str(), &context);
		if (ret)
		{
			fields = it->fields;
			target = it->target;
			StringTrimRight(&context, "的");
			StringTrim(&context, '\n');
			StringTrim(&context, '\t');

			fit = fields.begin();
			for (; fit != fields.end(); fit ++)
			{
				Condition cond;
				cond.field = *fit;
				cond.value = context;
				c_container.container.push_back(cond);
			}
			c_container.relation = RelationOR;
			return 1;
		}
		it ++;
		i ++;
	}
	return 0;
}

RegexQuestionDoctor::~RegexQuestionDoctor()
{
	vector<Pattern>::iterator it = pattern_vec.begin();
	while (it != pattern_vec.end())
	{
		if (it->reg)
		{
			delete it->reg;
			it->reg = NULL;
		}
	}
}

int RegexQuestionDoctor::CheckPid(string f1, string f2, 
			vector<int> &match_pid, int &match_point)
{
	match_point = 0;
	list<int> tmp_list_1;
	list<int> tmp_list_2;
	hash_map<string, list<int> >::iterator it;
	if ((it = rind_map.find(f1)) != rind_map.end())
	{
		match_point = f1.length();;
		tmp_list_1 = it->second;
	}
	else if (f1.length() == 0)
		match_point ++;

	if ((it = rind_map.find(f2)) != rind_map.end())
	{
		match_point = f2.length();
		tmp_list_2 = it->second;
	}
	else if (f2.length() == 0)
		match_point ++;

	list<int>::iterator it2;
	if (!tmp_list_1.size() && !tmp_list_2.size())
	{	
		LOG_PRINT(LOG_NORMAL,"no list has element\n");
		return 0;
	}
	else if (tmp_list_1.size() && !tmp_list_2.size())
	{
		for (it2 = tmp_list_1.begin(); it2 != tmp_list_1.end(); it2 ++)
			match_pid.push_back(*it2);
		LOG_PRINT(LOG_NORMAL, "1.matched pid num is %u", match_pid.size());
	}
	else if (tmp_list_2.size() && !tmp_list_1.size())
	{
		for (it2 = tmp_list_1.begin(); it2 != tmp_list_1.end(); it2 ++)
			match_pid.push_back(*it2);
		LOG_PRINT(LOG_NORMAL, "2.matched pid num is %u", match_pid.size());
	}
	else
	{
		for (it2 = tmp_list_1.begin(); it2 != tmp_list_1.end(); it2 ++)
		{
			if (find(tmp_list_2.begin(), tmp_list_2.end(), *it2) != tmp_list_2.end())
				match_pid.push_back(*it2);
		}
		LOG_PRINT(LOG_NORMAL, "3.matched pid num is %u", match_pid.size());
	}
	LOG_PRINT(LOG_NORMAL, "match point is %u", match_point);
	return 1;
}

int RegexQuestionDoctor::LoadRind()
{
	char * tables[] = {"tb_title", "tb_author", "tb_dynasty", \
					   "tb_content", "tb_style"};
	CommDB db;
	//db.Connect("10.173.13.40", "root", "root40", "qa_test");
	db.Connect("10.198.30.113", "root", "root113", "qa_test", 3335);

	LOG_PRINT(LOG_NORMAL, "LoadRind\n");
	int tb_size = sizeof(tables)/sizeof(char*);
	for (int i = 0; i < tb_size; i++)
	{
		string sql = "SELECT * FROM ";
		MYSQL_RES res;
		MYSQL_ROW row;

		sql += tables[i];
		if (!db.SelectData(sql, res))
		{
			LOG_PRINT(LOG_NORMAL, "sql: %s\n", sql.c_str());
			continue;
		}
		int num = 0;
		if ((num = mysql_num_fields(&res)) != 3)
		{
			LOG_PRINT(LOG_NORMAL, "Wrong Fields Num(should be 3), But Not It is %u\n", num);
			db.FreeResult(&res);
			continue;
		}
		while ((row = mysql_fetch_row(&res)))
		{
			//std::cout << "num fields:" << mysql_num_fields(&res) << std::endl;
			hash_map<string, list<int> >::iterator it = 
							rind_map.find(row[1]);
			if (it == rind_map.end())
			{
				list<int> q;
				vector<string> vs;
				SplitStringByDelimiter(row[2], ",", &vs);
				for (vector<string>::iterator vit = vs.begin();
					 vit != vs.end(); vit ++)
				{
					q.push_back(atoi(vit->c_str()));
				}
				rind_map.insert(pair<string, list<int> >(row[1],q));
			}
			else
			{
				list<int> q = it->second;
				vector<string> vs;
				SplitStringByDelimiter(row[2], ",", &vs);
				for (vector<string>::iterator vit = vs.begin();
					vit != vs.end(); vit ++)
				{
					if (find(q.begin(), q.end(),atoi(vit->c_str())) == q.end())
					{
						q.push_back(atoi(vit->c_str()));
					}
				}
			}
		}
		db.FreeResult(&res);
	}
	db.Close();
}

int RegexQuestionDoctor::SplitAndCheck(string qu, vector<int> &pids)
{
		int max_match_point = 0;
		ReplaceAll(&qu, " ", "");
		string rule = "的";
		int u_size;
		int qu_size = qu.length();
		int tmp_match_point = 0;
		vector<int> tmp_match_pid;
		u_size = rule.length();
		LOG_PRINT(LOG_NORMAL, "qu_size = %d, u_size = %u\n", qu_size, u_size);
		if (qu_size/u_size <= 2)
		{
			if (CheckPid(qu,"",tmp_match_pid,tmp_match_point))
			{
				int i = 0;
				vector<int>::iterator it = tmp_match_pid.begin();
				for (; it != tmp_match_pid.end(); it ++)
					LOG_PRINT(LOG_NORMAL, "pid%u:%d\n", i++,*it);
				return 1;
			}

			return 0;
		}

		for(int i = 0 + u_size; i <= qu_size; i += u_size)
		{

			string f1 = qu.substr(0, i);
			string f2 = qu.substr(i,qu_size-i);
			LOG_PRINT(LOG_NORMAL, "f1:%s,f2:%s\n", f1.c_str(), f2.c_str());
		

			tmp_match_point = 0;
			tmp_match_pid.clear();

			if (!CheckPid(f1,f2,tmp_match_pid,tmp_match_point))
				continue;

			if (tmp_match_point > max_match_point)
			{
				max_match_point = tmp_match_point;
				pids = tmp_match_pid;
			}
		}

		vector<int>::iterator it = pids.begin();
		int i = 0;
		LOG_PRINT(LOG_NORMAL, "Final Result Num Is %u\n", pids.size());
		for (; it != pids.end(); it ++)
		{
			LOG_PRINT(LOG_NORMAL, "pid%u:%d\n",i++, *it);
		}
		return 1;
}

void LoadRegexPattern(string t_file, RegexQuestionDoctor *rqd)
{
	unsigned rsize = 0;
	string buf;

	File *file_obj = File::Open(t_file, "r");
	if (!file_obj)
	{
		LOG_PRINT(LOG_NORMAL, "Load Regex Template Failed: File %s Not Exsit!\n", 
					t_file.c_str());
		//return -1;
		//exit(1);
		return;
	}

	MutexLocker locker(m_regex_mutex);
	rqd->ClearPattern();
	while ((rsize = file_obj->ReadLine(&buf)) > 5)
	{
		vector<string> tmp_vec;
		vector<string> f_vec;
		Pattern p;
		SplitStringByDelimiter(buf, "$$", &tmp_vec);
		if (tmp_vec.size() != 3)
		{
			LOG_PRINT(LOG_NORMAL, "fields not matched, line is %s\n",buf.c_str());
			continue;
		}
		p.pattern = tmp_vec[0];
		p.target = tmp_vec[2];
		StringTrim(&(p.target));
		SplitStringByDelimiter(tmp_vec[1], ",", &f_vec);
		p.fields = f_vec;
		rqd->AddPattern(p);
	}
	LOG_PRINT(LOG_NORMAL, "Load Regex Template Success\n");
	file_obj->Close();
	delete file_obj;
}
