#ifndef _QUESTION_ANALYZER_H_
#define _QUESTION_ANALYZER_H_

#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <vector>
#include <list>
#include <map>
#include "common/text/regex/regex.h"
#include "common/base/string/algorithm.h"
#include "common/base/stdext/hash_map.h"
#include "common/system/concurrency/mutex.h"
#include "qa_doctor_base.h"

using namespace std;
using namespace stdext;

const uint8_t MAX_RESULT_NUM = 5;
static int m_allow_token = 0;

typedef struct s_pattern {
	string pattern;
	Regex *reg;
	//unsigned ftype;
	vector<string> fields;
	string target;
	//unsigned ttype;
} Pattern;

class RegexQuestionDoctor;
static Mutex m_regex_mutex;
extern void LoadRegexPattern(string t_file, RegexQuestionDoctor *rqd);

class RegexQuestionDoctor : public QuestionDoctorBase
{
	public:
		//static RegexQuestionDoctor * GetInstance();
		RegexQuestionDoctor(string pattern_file);
		~RegexQuestionDoctor();
		
		int Tokenizer(string question, vector<string> & fields, 
				string & target, string & context);
		
		int MatchQuery(string query,
			ConditionContainer &c_container, Target &target);
		
		int CheckPid(string f1, string f2,
				vector<int> &match_pid, int &match_point);
		
		int SplitAndCheck(string qu, vector<int> &pids);

		void AddPattern(Pattern pattern);
		
		void ClearPattern();
		
		void TranslateTarget(unsigned ttype, std::vector<string> &vf);
	private:
		int LoadRind();//Load Reverse Index
	private:
		static RegexQuestionDoctor * instance;
		vector<Pattern> pattern_vec;
		hash_map<string, list<int> > rind_map;
};

#endif
