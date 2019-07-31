#ifndef _JOKE_H_
#define _JOKE_H_

#include <iostream>
#include <string>

#include "nlu/sub_service/weather/spider_patternmatch/pattern_component.h"
#include "proto/nlu/joke.pb.h"
#include "qa_log.h"

using namespace pb::dqa_joke;

#define MAX_JOKE_NUM 5

typedef struct tag_Joke {
	uint32_t style;
	uint32_t idx;
	//vector<string> tags;
	string   joke;
	tag_Joke ()
	{
		idx = 0 ;
		style = 0;
		//tags.clear();
		joke = "";
	}
	void print()
	{
	}
} Joke, *PJoke;

typedef struct tag_joke_answer {
	string joke_str;
	float point;  
    Joke joke;

    tag_joke_answer ()
	{
	};

	~tag_joke_answer ()
	{
	};
} JokeAnswer, *PJokeAnswer;
/*
bool compare(const JokeAnswer &first, const JokeAnswer &second)
{
	return first.point > second.point;
}
*/
class JokeInfo
{
	public:
		JokeInfo(const std::string& _joke_path="../etc/joke_info.dat" , const std::string& _joke_type_path="../etc/joke_types.dat")
        {
            JokeInfo_path=_joke_path;
            JokeTypes_path=_joke_type_path;
        }
        bool Init();
		bool LodaTypes();
		bool LoadJokeData();
		uint32_t  GetJokes(int32_t style,vector<JokeAnswer>& joke_ans);
		int32_t GetJokeTypeId(string type_word);
	
	public:
	    map<int32_t , vector<Joke> > m_jokes;
	    map<string,int32_t> m_joke_types;
	    uint32_t m_joke_num ;
    	uint32_t m_joke_style_num ; 
        std::string JokeInfo_path;
        std::string JokeTypes_path;
};


#endif
