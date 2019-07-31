// Copyright (c) Tencent Inc.
// All rights reserved.
//
// Author: 
//
// ¹¦ÄÜ: Ë÷Òýne´Êµä
//
// Time:

#include "joke.h"
#include <string.h>
#include <map>
#include <vector>
#include <cstdio>
#include <stdlib.h>
#include "util.h"
using namespace life_service;

bool JokeInfo::Init()
{
    if(!LoadJokeData() || !LodaTypes())
        return false;
    return true;
}

bool JokeInfo::LoadJokeData()
{
    FILE * fp = NULL;
    char * line = NULL;
    size_t len = 0 ;
    size_t capacity = 0;
    int32_t style_id = 0;
	int32_t idx = 0 ;
	string joke;

    LOG_PRINT(LOG_NORMAL, "Joke data path path %s\n",JokeInfo_path.c_str());
    fp = fopen(JokeInfo_path.c_str(), "r");
    if (fp == NULL) {
        LOG_PRINT(LOG_NORMAL, "open Joke data path path %s failed \n",JokeInfo_path.c_str());
        return false;
        }

    std::vector<std::string> parts;
    while ((len = getline(&line, &capacity, fp)) != -1)
	{
		if(line[0] == '#') continue;

		if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
            --len;
        }
        if (len == 0) continue;

		Split(line, "\t", parts);
		if(parts.size() != 2)
        {
            LOG_PRINT(LOG_NORMAL, "Joke format error %s\n",line);
            //log_error("jokes format error! %s",line);
			continue;
        }
		style_id =atoi(parts[0].c_str());

		Joke joke;
		joke.style = style_id;
		joke.joke = parts[1];
		joke.idx = idx;
		idx += 1;

		std::map<int32_t, vector<Joke> >::iterator p_iter =  m_jokes.find(style_id);
		 if (p_iter != m_jokes.end())
         {
                p_iter->second.push_back(joke);
         }
         else
         {
                vector<Joke> joke_vector;
                joke_vector.push_back(joke);
                m_jokes[style_id] = joke_vector;
         }

	}
    m_joke_num = idx-1;
	m_joke_style_num = m_jokes.size();

	free(line);
    fclose(fp);

	return true;
}

bool JokeInfo::LodaTypes()
{
    FILE * fp = NULL;
    char * line = NULL;
    size_t len = 0 ;
    size_t capacity = 0;
    int32_t style_id = 0;
    string style_name;

    LOG_PRINT(LOG_NORMAL, "Joke type path path %s\n",JokeTypes_path.c_str());
    fp = fopen(JokeTypes_path.c_str(), "r");
    if (fp == NULL) {
        LOG_PRINT(LOG_NORMAL, "open Joke type  path %s failed \n",JokeTypes_path.c_str());
        return false;
    }

    std::vector<std::string> parts;
    while ((len = getline(&line, &capacity, fp)) != -1)
    {
	if(line[0] == '#') continue;

	if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
            --len;
        }
        if (len == 0) continue;

	Split(line, "\t", parts);
	if(parts.size() != 2)
        {
		continue;
        }
	style_name  =parts[0];
	style_id = atoi(parts[1].c_str());

	m_joke_types.insert(pair<string,int32_t>(style_name, style_id));
    }

    free(line);
    fclose(fp);

    return true;
}


uint32_t  JokeInfo::GetJokes(int32_t style_id, vector<JokeAnswer> &ans_vec )
{
	ans_vec.clear();

	set<uint32_t> his_idx;
	his_idx.clear();

	if(style_id == 0 )
	{
        srand((uint32_t)time(NULL));
		for(uint32_t i = 0 ; i < MAX_JOKE_NUM ; )
		{
		    uint32_t s_idx = (rand() % (m_joke_style_num)) + 1;

		    uint32_t j_idx = (rand() % (m_jokes[s_idx].size()));

		    uint32_t jokes_idx = m_jokes[s_idx][j_idx].idx;
			if(his_idx.find(jokes_idx) == his_idx.end())
			{
				JokeAnswer joke_ans;
				joke_ans.joke_str = m_jokes[s_idx][j_idx].joke;
				joke_ans.point = 1.0;
				joke_ans.joke.style = m_jokes[s_idx][j_idx].style;
                joke_ans.joke.idx = m_jokes[s_idx][j_idx].idx;
                joke_ans.joke.joke = m_jokes[s_idx][j_idx].joke;
                //joke_ans.joke(m_jokes[s_idx][j_idx]);
				ans_vec.push_back(joke_ans);
				his_idx.insert(jokes_idx);
				i++;
			}
		}		
	}
    else
	{

	    std::map<int32_t, vector<Joke> >::iterator p_iter =  m_jokes.find(style_id);
	    if(p_iter != m_jokes.end())
	    {
            srand((uint32_t)time(NULL));
		    for(uint32_t i = 0 ; i < MAX_JOKE_NUM ; )
			{
                uint32_t j_idx = (rand() % (p_iter->second.size()));
                uint32_t jokes_idx = p_iter->second[j_idx].idx;
                LOG_PRINT(LOG_NORMAL, "rand %d in %d type jokes, get %d Jokes\n",style_id, j_idx, jokes_idx);
			    if(his_idx.find(jokes_idx) == his_idx.end())
			    {
				    JokeAnswer joke_ans;
				    joke_ans.joke_str =  p_iter->second[j_idx].joke;
				    joke_ans.point = 1.0;
				    joke_ans.joke.style =  p_iter->second[j_idx].style;
                    joke_ans.joke.idx =  p_iter->second[j_idx].idx;
                    joke_ans.joke.joke =  p_iter->second[j_idx].joke;
                    ans_vec.push_back(joke_ans);
				    his_idx.insert(jokes_idx);
				    i++;
                    jokes_idx += 1;
                    LOG_PRINT(LOG_NORMAL, "get jokes %s\n",joke_ans.joke_str.c_str());


			   }
			}
		}
	}
	
	return ans_vec.size();
}


int32_t JokeInfo::GetJokeTypeId(string type_word)
{
    map<string,int32_t>::iterator ite = m_joke_types.find(type_word);
    if(ite != m_joke_types.end())
        return ite->second;
    else return 0;
}
