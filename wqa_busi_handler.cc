#include "wqa_busi_handler.h"
#include "life_service/proto/dqa_rootmerge.pb.h"
#include "common/base/string/algorithm.h"
#include "thirdparty/jsoncpp/json.h"
#include  "time_utils.h"

using namespace pb::dqa_rootmerge;
using namespace Json;

const string time_format="yyyy-MM-dd HH:mm:ss";

WeatherBusiHandler::WeatherBusiHandler() 
			: BusiHandlerBase(DQA_WEATHER)
{
	uint64_t id = 0;
	LoadData(id);
	TimerManager::CallbackFunction load_data_func = \
				Bind(&WeatherBusiHandler::LoadData, this);
	TimerManager::DefaultInstance().AddPeriodTimer(3600000, load_data_func);
}

WeatherBusiHandler::~WeatherBusiHandler()
{
	map<string, AreaIndices*>::iterator iter = area_indices_map.begin();
	for (; iter != area_indices_map.end(); iter ++)
	{
		if (iter->second)
		{
			delete iter->second;
			iter->second = NULL;
		}
	}

	area_indices_map.clear();
}

int WeatherBusiHandler::PreHandle(string &query)
{
	StringTrim(&query, "?");
	StringTrim(&query, ".");
	ReplaceAll(&query, "1", "一");
	ReplaceAll(&query, "2", "二");
	ReplaceAll(&query, "3", "三");
	ReplaceAll(&query, "4", "四");
	ReplaceAll(&query, "5", "五");
	ReplaceAll(&query, "6", "六");
	ReplaceAll(&query, "7", "七");
	ReplaceAll(&query, "星期", "周");
	LOG_PRINT(LOG_NORMAL, "Query:%s\n",query.c_str());
	return 1;
}

//检索数据，生成答案
int WeatherBusiHandler::HandleRequest(ConditionContainer c_container,
		Target target, MessageVec &mv)
{
	string area = m_default_area;
	cur_cond_container = c_container;
	DateConf dc;
	dc.start = 0;
	dc.end = 0;

	AreaWeather area_weather;
	ConditionVec cvec = c_container.container;
	ConditionVec::iterator iter = cvec.begin();
	// 匹配出日期和地名，并转换
	for (; iter != cvec.end(); iter ++)
	{
		LOG_PRINT(LOG_NORMAL, "field:%s,value:%s\n",
			iter->field.c_str(), iter->value.c_str());
		if (!iter->field.compare(FIELD_DATE))
		{
                    size_t findIndex=iter->value.find("#");
                    if(findIndex!=string::npos)
                    {
                        string beginTimeStr=iter->value.substr(0,findIndex);
                        string endTimeStr=iter->value.substr(findIndex+1);
                        life_service::TimeUtils::compareNowDays(beginTimeStr,time_format,&dc.start);
                        life_service::TimeUtils::compareNowDays(endTimeStr,time_format,&dc.end);
                    }
		}
		else if (!iter->field.compare(FIELD_AREA))
		{
			area = iter->value;
		}
	}
	//没有地域信息
	if (area.empty())
	{
		WeatherResponse *resp = new WeatherResponse();
		resp->set_feedback(WFB_NO_AREA);
		resp->set_matched_target(TARGET_NO_AREA);
		mv.push_back(resp);
		return 1;
	}

	if (!FindAreaWeather(area, &area_weather))
	{
		LOG_PRINT(LOG_ERROR, "No Such Area String:%s\n", area.c_str());
		return 0;
	}

	//填充PB	
	WeatherResponse *resp = new WeatherResponse();
	Weather *w = resp->add_matched_weather();
        if(dc.start==0)
        {
            FillPB(w, &area_weather.today_weather);
            w->set_area(ConvUtf8ToGbk(area));
        }else if(dc.start>=1 && dc.start<8)
        {
	    FillPB(w, &area_weather.future_7_weather[dc.start-1]);
            w->set_area(ConvUtf8ToGbk(area));
        }
	/*int base_index = CountBaseIndex(&area_weather);
	int max_size = area_weather.future_7_weather.size();
	int i = base_index + dc.start;
	int end = ((base_index + dc.end) < max_size) ? 
				(base_index + dc.end) : (max_size - 1);
	LOG_PRINT(LOG_NORMAL, "max_size:%d,dc.start=%d,dc.end=%d, base_index:%d\n", 
			max_size, dc.start, dc.end, base_index);
	WeatherResponse *resp = new WeatherResponse();
	for (i; i <= end && i >= 0; i ++)
	{
		Weather *w = resp->add_matched_weather();
		if (i == 0)
			FillPB(w, &area_weather.today_weather);
		else
			FillPB(w, &area_weather.future_7_weather[i]);
		//设置地域
		w->set_area(ConvUtf8ToGbk(area));
	}

	for (i = base_index; i < max_size; i ++)
	{
		Weather *w = resp->add_next_seven_weather();
		FillPB(w, &area_weather.future_7_weather[i]);
		//设置地域
		w->set_area(ConvUtf8ToGbk(area));
	}*/

	resp->set_feedback(WFB_OK);
	resp->set_matched_target(target);
	mv.push_back(resp);

	return 1;
}

float WeatherBusiHandler::GetWeight(GMessage *msg)
{
	return 0.8;
}

//加工答案
string WeatherBusiHandler::GetAnsText(GMessage *msg, Target target)
{
	WeatherResponse *wresp = (WeatherResponse*)msg;
	//target = wresp->matched_target();
	string area = m_default_area;
	LOG_PRINT(LOG_NORMAL, "TARGET is %s\n", target.c_str());
	if (!target.compare(TARGET_TEMPERATURE))
	{
		if (!wresp->matched_weather_size())
			return "";
		char buf[128];
		sprintf(buf, "%.2f°C ~ %.2f°C", 
			wresp->matched_weather(0).temperature().min_temperature(),
			wresp->matched_weather(0).temperature().max_temperature());
		return buf;
	}
	else if (!target.compare(TARGET_IS_COLD))
	{
		if (!wresp->matched_weather_size())
			return "";
		if (wresp->matched_weather(0).temperature().min_temperature() < 15.0)
			return "yes";
		else
			return "no";
	}
	else if (!target.compare(TARGET_IS_HOT))
	{
		if (!wresp->matched_weather_size())
			return "";
		if (wresp->matched_weather(0).temperature().max_temperature() > 30.0)
			return "yes";
		else
			return "no";
	}
	else if (!target.compare(TARGET_IS_RAINY))
	{
		if (!wresp->matched_weather_size())
			return "";
		string day_status = ConvGbkToUtf8(wresp->matched_weather(0).day_status());
		string night_status = ConvGbkToUtf8(wresp->matched_weather(0).night_status());
		if (strstr(day_status.c_str(), "雨") ||
			strstr(night_status.c_str(), "雨"))
		{
			return "yes";
		}

		return "no";
	}
	else if (!target.compare(TARGET_IS_SNOWY))
	{
		if (!wresp->matched_weather_size())
			return "";
		string day_status = ConvGbkToUtf8(wresp->matched_weather(0).day_status());
		string night_status = ConvGbkToUtf8(wresp->matched_weather(0).night_status());
		if (strstr(day_status.c_str(), "雪") ||
			strstr(night_status.c_str(), "雪"))
		{
			return "yes";
		}

		return "no";
	}
	else if (!target.compare(TARGET_IS_WINDY))
	{
		if (!wresp->matched_weather_size())
			return "";
		string day_wind = ConvGbkToUtf8(wresp->matched_weather(0).day_wind());
		string night_wind = ConvGbkToUtf8(wresp->matched_weather(0).night_wind());
		if (strstr(day_wind.c_str(), "风") ||
			strstr(night_wind.c_str(), "风"))
		{
			return "yes";
		}

		return "no";
	}
	else if (!target.compare(TARGET_IS_TYPHOON))
	{
		if (!wresp->matched_weather_size())
			return "";
		string day_wind = ConvGbkToUtf8(wresp->matched_weather(0).day_wind());
		string night_wind = ConvGbkToUtf8(wresp->matched_weather(0).night_wind());
		if (strstr(day_wind.c_str(), "台风") ||
			strstr(night_wind.c_str(), "台风"))
		{
			return "yes";
		}

		return "no";
	}
	else if (!target.compare(TARGET_WEATHER))
	{
		if (!wresp->matched_weather_size())
			return "";
		string ret_str = "白天:";
		ret_str += ConvGbkToUtf8(wresp->matched_weather(0).day_status());
		ret_str += "  夜间:";
		ret_str += ConvGbkToUtf8(wresp->matched_weather(0).night_status());
		return ret_str;
	}
	else if (!target.compare(TARGET_INDEX))
	{
		string index_name;
		string date;
		DateConf dc;
		//查找指数名
		ConditionVec::iterator iter = cur_cond_container.container.begin();
		for (; iter != cur_cond_container.container.end(); iter ++)
		{
			if (!iter->field.compare(FIELD_DATE))
			{
				//if (!FindDateConf(iter->value, &dc) ||
				//	!(dc.start == 0 && dc.end == 0))
				{
					return "";
				}
			}
			else if (!iter->field.compare(FIELD_AREA))
			{
				area = iter->value;
			}
			else if (!iter->field.compare(FIELD_INDEX))
			{
				index_name = iter->value;
			}
		}
		//
		date = FormatDayStr();
		LOG_PRINT(LOG_NORMAL, "area:%s,date:%s,index:%s\n",
				area.c_str(), date.c_str(), index_name.c_str());
		map<string, AreaIndices*>::iterator it = area_indices_map.find(area);
		if (it == area_indices_map.end())
		{
			LOG_PRINT(LOG_NORMAL, "no area indices\n");
			return "";
		}
	

		if (date.compare(it->second->date))
		{
			LOG_PRINT(LOG_NORMAL, "date not matched, query date:%s, indices date:%s\n",
				date.c_str(), it->second->date.c_str());
			return "";
		}

		LOG_PRINT(LOG_NORMAL,"it->second->indices.size is %d\n",
					it->second->indices.size());
		//查找指数
		vector<Index>::iterator it2 = it->second->indices.begin();
		for (; it2 != it->second->indices.end(); it2 ++)
		{
			//LOG_PRINT(LOG_NORMAL,"indices name:%s\n", it2->name.c_str());
			if (strstr(it2->name.c_str(), index_name.c_str()))
			{
				return it2->hint + "(" + it2->dess + ")";
			}
		}

		LOG_PRINT(LOG_NORMAL, "No Indices Matched,query indice:%s\n", index_name.c_str());
		return "";
	}
	else
	{
		return "";
	}
}

string WeatherBusiHandler::SerializeToString(GMessage *msg)
{
	string s_str;
	WeatherResponse *weather_resp = (WeatherResponse *)msg;
	if (! weather_resp)
	{
		return "";
	}
	else
	{
		weather_resp->SerializeToString(&s_str);
		return s_str;
	}
}

void WeatherBusiHandler::ReleaseMessage(MessageVec &mv)
{
	MessageVec::iterator it = mv.begin();
	for (; it != mv.end(); it ++)
	{
		if (*it)
		{
			delete *it;
			*it = NULL;
		}
	}

	mv.clear();
}

void WeatherBusiHandler::FillPB(Weather *weather, DayWeather *day_weather)
{
	if (!weather || !day_weather)
	{
		LOG_PRINT(LOG_ERROR, "weather OR day_weather is null\n");
		return;
	}
	/*
	time_t rawtime;
	struct tm *timeinfo;
	char buf[128];

	time(&rawtime);
	rawtime += (86400 * day_from_today);
	timeinfo = localtime(&rawtime);
	strftime(buf, sizeof(buf), "%Y%m%d", timeinfo);
	*/
	//设置日期
	//string date_str(buf);
	weather->set_date(ConvUtf8ToGbk(day_weather->date));

	//设置温度
	Temperature *t = weather->mutable_temperature();
	t->set_max_temperature(day_weather->max_temperature);
	t->set_min_temperature(day_weather->min_temperature);
	//设置天气状态
	weather->set_day_status(ConvUtf8ToGbk(day_weather->day_status));
	weather->set_night_status(ConvUtf8ToGbk(day_weather->night_status));
	//设置风力
	weather->set_day_wind(ConvUtf8ToGbk(day_weather->day_wind));
	weather->set_night_wind(ConvUtf8ToGbk(day_weather->night_wind));
	//LOG_PRINT(LOG_NORMAL, "date:%s, max_temp:%.2f, min_temp:%.2f, day_status:%s, night_status:%s\n",
	//	date_str.c_str(), day_weather->max_temperature, day_weather->min_temperature,
	//	day_weather->day_status.c_str(), day_weather->night_status.c_str());
}

int WeatherBusiHandler::FindAreaWeather(string area, AreaWeather *area_weather)
{
	vector<AreaWeather>::iterator it = real_time_area_weather.begin();
	while (it != real_time_area_weather.end())
	{
		if (!area.compare(it->area) || (it->area.find(area)!=string::npos && area.length()>5) || (area.find(it->area)!=string::npos && it->area.length()>5))
		{
			LOG_PRINT(LOG_NORMAL, "matched area weather's area:%s\n", it->area.c_str());
			*area_weather = *it;
			return 1;
		}
		it ++;
	}

	it = last_time_area_weather.begin();
	while (it != last_time_area_weather.end())
	{
		if (!area.compare(it->area))
		{
			*area_weather = *it;
			LOG_PRINT(LOG_NORMAL, "matched area weather's area:%s\n", it->area.c_str());
			return 1;
		}
		it ++;
	}

	return 0;
}

//加载天气数据
void WeatherBusiHandler::LoadData(const uint64_t id)
{
	time_t rawtime;
	struct tm * timeinfo;
	char buf[128];
	char buf2[128];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buf, sizeof(buf), "%Y%m%d%H", timeinfo);
	strftime(buf2, sizeof(buf2), "%Y%m%d", timeinfo);
	
	LOG_PRINT(LOG_NORMAL, "Time Is %s\n", buf);
	string file_name = "../etc/data/";
	file_name += (const char *)&buf2[0];
	file_name += File::kPathSeparator;
	//file_name += (const char *)&buf[0];
	file_name += "weather.dat";

	LOG_PRINT(LOG_NORMAL, "File Is %s\n", file_name.c_str());

	unsigned rsize = 0;
	string line;
	File *file_obj = File::Open(file_name, "r");
	if (!file_obj)
	{
		LOG_PRINT(LOG_ERROR, "LoadData Failed: File %s Not Exists\n", file_name.c_str());
		return;
	}
	
	last_time_area_weather = real_time_area_weather;
	real_time_area_weather.clear();
	while ((rsize = file_obj->ReadLine(&line)) > 10)
	{
		vector<string> tmp_vec;
		line = ConvGbkToUtf8(line);
		ReplaceAll(&line, "\t\t", "\t \t");
		SplitStringByDelimiter(line, "\t", &tmp_vec);
		if (tmp_vec.size() <=8)// 37)
		{
			LOG_PRINT(LOG_ERROR, "fields not matched, line is %s\n", line.c_str());
			continue;
		}

		AreaWeather area_weather;
		string tmp_temperature;
		area_weather.cur_date = buf2;
		area_weather.area = tmp_vec[1];
		tmp_temperature = tmp_vec[2];
		StringTrimRight(tmp_temperature, "°C");
		area_weather.current_temperature = atof(tmp_temperature.c_str());
		
		area_weather.today_weather.day_status = tmp_vec[3];
		area_weather.today_weather.day_wind = tmp_vec[5];
		area_weather.today_weather.night_status = tmp_vec[6];
		area_weather.today_weather.night_wind = tmp_vec[8];
		tmp_temperature = tmp_vec[4];
		StringTrimRight(tmp_temperature, "°C");
		area_weather.today_weather.max_temperature = atof(tmp_temperature.c_str());
		tmp_temperature = tmp_vec[7];
		StringTrimRight(tmp_temperature, "°C");
		area_weather.today_weather.min_temperature = atof(tmp_temperature.c_str());
		
		if (area_weather.today_weather.min_temperature > area_weather.today_weather.max_temperature)
		{
			float tmp = area_weather.today_weather.min_temperature;
			area_weather.today_weather.min_temperature = area_weather.today_weather.max_temperature;
			area_weather.today_weather.max_temperature = tmp;
		}
		
		area_weather.today_weather.date = area_weather.cur_date;

		int i = 0;//12;
		struct tm *tminfo;
		time_t tmp_time;
		for (; i < 7 && 12+i*3+2 < tmp_vec.size(); i ++)
		{
			char d_buf[32];
			DayWeather day_weather;
			day_weather.day_status = tmp_vec[12 + i * 3];
			day_weather.day_wind = tmp_vec[12 + i * 3 + 2];
			day_weather.night_status = tmp_vec[12 + i * 3];
			day_weather.night_wind = tmp_vec[12 + i * 3 + 2];

			tmp_temperature = tmp_vec[12 + i * 3 + 1];
                        tmp_temperature = tmp_temperature.erase(tmp_temperature.find('/'));
			day_weather.max_temperature = atof(tmp_temperature.c_str());

			tmp_temperature = tmp_vec[12 + i * 3 + 1];
                        tmp_temperature = tmp_temperature.erase(0,tmp_temperature.find('/')+1);
			day_weather.min_temperature = atof(tmp_temperature.c_str());

			tmp_time = rawtime + (i+1) * SECOND_IN_ONE_DAY;
			tminfo = localtime(&tmp_time);
			strftime(d_buf, sizeof(d_buf), "%Y%m%d", tminfo);
			day_weather.date = d_buf;
			area_weather.future_7_weather.push_back(day_weather);
		}

		real_time_area_weather.push_back(area_weather);
	}
	
	LOG_PRINT(LOG_NORMAL, "Load %d Area Weather From File %s\n",
		real_time_area_weather.size(), file_name.c_str());
	file_obj->Close();
	delete file_obj;

	WeatherBusiHandler::LoadIndices();
}

//指数，不用
void WeatherBusiHandler::LoadIndices()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buf[128];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buf, sizeof(buf), "%Y%m%d", timeinfo);
	
	string file_name = "../etc/data/";
	file_name += (const char *)&buf[0];
	file_name += File::kPathSeparator;
	file_name += "weather_indices.dat";

	File *file_obj = File::Open(file_name, "r");
	if (!file_obj)
	{
		LOG_PRINT(LOG_ERROR, "File %s Not Exists!\n", file_name.c_str());
		return;
	}

	unsigned rsize = 0;
	string line;
	Reader reader;
	char *index_basic_str[] = {"ac", "ag", "be", "cl", "co", "ct",
							 "dy", "fs", "gj", "gl", "gm", "gz", 
							 "hc", "jt", "lk", "ls", "mf", "nl", 
							 "pj", "pk", "pl", "pp", "pt", "sg", 
							 "tr", "uv", "xc", "xq", "yd", "yh", 
							 "ys", "zs"};
	while ((rsize = file_obj->ReadLine(&line)) > 10)
	{
		Value root;
		line = ConvGbkToUtf8(line);
		StringTrim(&line, "\r");
		StringTrim(&line, "\n");
		StringTrim(&line);

		if (!reader.parse(line, root))
			continue;

		string area = root["cn"].asString();
		//string date = root["zs"]["date"].asString();
		//if (date.size() > 8)
		//	date = date.substr(0, 8);
		//int size = root["zs"].size();
		int size = 32;
		int i = 0;
		string name;
		string hint;
		string dess;
		
		AreaIndices *ai = new AreaIndices();
		ai->area = area;
		ai->date = buf;
		for (; i < size; i ++)
		{
			string sname = index_basic_str[i];
			sname += "_name";
			string shint = index_basic_str[i];
			shint += "_hint";
			string sdess = index_basic_str[i];
			sdess += "_des_s";
			//if (i % 3 == 1)
				name = root["zs"][sname.c_str()].asString();
			//else if (i % 3 == 2)
				hint = root["zs"][shint.c_str()].asString();
			//else
			//{
				dess = root["zs"][sdess.c_str()].asString();

				Index ind;
				ind.name = name;
				ind.hint = hint;
				ind.dess = dess;

				ai->indices.push_back(ind);
			//}
			//LOG_PRINT(LOG_NORMAL, "%s:%s, %s:%s, %s:%s\n",
			//	sname.c_str(), name.c_str(), shint.c_str(),
			//	hint.c_str(), sdess.c_str(), dess.c_str());
		}
		area_indices_map.insert(pair<string, AreaIndices*>(area, ai));
	}
	LOG_PRINT(LOG_NORMAL, "Load %d Area Indices From File %s\n",
		area_indices_map.size(), file_name.c_str());
}

int WeatherBusiHandler::CountBaseIndex(AreaWeather *area_weather)
{
	time_t rawtime;
	struct tm *timeinfo;
	char buf[32];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buf, sizeof(buf), "%Y%m%d", timeinfo);

	string today(buf);
	int size = area_weather->future_7_weather.size();
	int i = 0;
	for (; i < size; i ++)
	{
		if (!today.compare(area_weather->future_7_weather[i].date))
			return i;
	}
}

int WeatherBusiHandler::ConvertDate(string date, DateConf *dc)
{
	char *week_day[] = {"一","二","三","四","五","六","日","天","七"};
	int week_day_index[] = {1,2,3,4,5,6,7,7};
	int u_size = sizeof("周") - 1;//去除'\0'
	int i = 0;
	int s_size = date.size();
	int matched_week_day = 0;
	char *tmp = strstr(date.c_str(), "周");
	if (!tmp) return -1;
	LOG_PRINT(LOG_NORMAL, "tmp:%s\n", tmp + u_size);
	for (; i < 9; i ++)
	{
		if (!strncmp(week_day[i], tmp + u_size, u_size))
		{
			matched_week_day = week_day_index[i];
			break;
		}
	}

	time_t rawtime;
	struct tm *tminfo;
	time(&rawtime);
	tminfo = localtime(&rawtime);
	int this_week_day = tminfo->tm_wday;
	if (this_week_day == 0)
		this_week_day = 7;

	LOG_PRINT(LOG_NORMAL, "matched_week_day:%d, this_week_day:%d\n",
		matched_week_day, this_week_day);
	int ret = -1;
	if (!date.compare(0, sizeof("本周")-1, "本周") ||
		!date.compare(0, sizeof("周")-1, "周"))
	{
		ret = matched_week_day - this_week_day;
		if (ret >= 0)
		{
			dc->start = ret;
			dc->end = ret;
		}
	}
	else if (!date.compare(0, sizeof("下周")-1, "下周"))
	{
		ret =  matched_week_day - this_week_day + 7;
		if (ret >= 0)
		{
			dc->start = ret;
			dc->end = ret;
		}
	}
	else
		ret = -1;

	return ret;
}

//still has a problem
void DebugPB(WeatherResponse *w_resp)
{
	LOG_PRINT(LOG_NORMAL, "feedback:%d\n", w_resp->feedback());
	LOG_PRINT(LOG_NORMAL, "target:%s\n", w_resp->matched_target().c_str());
	int s1 = w_resp->matched_weather_size();
	int i = 0;
	LOG_PRINT(LOG_NORMAL, "------matched weather:size=%d------\n", s1);
	for (; i < s1; i ++)
	{
		Weather weather = w_resp->matched_weather(i);
		LOG_PRINT(LOG_NORMAL, "%d: date:%s\n", i, weather.date().c_str());
		LOG_PRINT(LOG_NORMAL, "%d: area:%s\n", i, weather.area().c_str());
		LOG_PRINT(LOG_NORMAL, "%d: max-temperature:%.2f\n", i, weather.temperature().max_temperature());
		LOG_PRINT(LOG_NORMAL, "%d: mix-temperature:%.2f\n", i, weather.temperature().max_temperature());
		LOG_PRINT(LOG_NORMAL, "%d: day_status:%s\n", i, weather.day_status().c_str());
		LOG_PRINT(LOG_NORMAL, "%d: night_status:%s\n", i, weather.night_status().c_str());
		LOG_PRINT(LOG_NORMAL, "%d: day_wind:%s\n", i, weather.day_status().c_str());
		LOG_PRINT(LOG_NORMAL, "%d: night_wind:%s\n", i, weather.night_status().c_str());
	}
	int s2 = w_resp->next_seven_weather_size();
	LOG_PRINT(LOG_NORMAL, "------future seven weather:size=%d------\n", s2);
	for (i = 0; i < s2; i ++)
	{
		Weather weather = w_resp->next_seven_weather(i);
		LOG_PRINT(LOG_NORMAL, "%d: date:%s\n", i, weather.date().c_str());
		LOG_PRINT(LOG_NORMAL, "%d: area:%s\n", i, weather.area().c_str());
		LOG_PRINT(LOG_NORMAL, "%d: max-temperature:%.2f\n", i, weather.temperature().max_temperature());
		LOG_PRINT(LOG_NORMAL, "%d: mix-temperature:%.2f\n", i, weather.temperature().max_temperature());
		LOG_PRINT(LOG_NORMAL, "%d: day_status:%s\n", i, weather.day_status().c_str());
		LOG_PRINT(LOG_NORMAL, "%d: night_status:%s\n", i, weather.night_status().c_str());
		LOG_PRINT(LOG_NORMAL, "%d: day_wind:%s\n", i, weather.day_status().c_str());
		LOG_PRINT(LOG_NORMAL, "%d: night_wind:%s\n", i, weather.night_status().c_str());
	}
}

string WeatherBusiHandler::FormatDayStr()
{
	time_t rawtime;
	struct tm *tminfo;
	char buf[128];

	time(&rawtime);
	tminfo = localtime(&rawtime);
	strftime(buf, sizeof(buf), "%Y%m%d", tminfo);

	string tmp(buf);
	return tmp;
}
