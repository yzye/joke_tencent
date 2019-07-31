#ifndef _WQA_BUSI_HANDLER_H_
#define _WQA_BUSI_HANDLER_H_

#include <iostream>
#include <string>
#include <map>

#include "common/file/file.h"
#include "common/file/local_file.h"
#include "common/file/file_define.h"
#include "common/system/timer/timer_manager.h"
#include "common/base/function/function.h"
#include "life_service/proto/weather.pb.h"
#include "qa_busi_handler_base.h"
#include "qa_log.h"
#include "qa_common_tools.h"
#include "wqa_define.h"

using namespace pb::dqa_weather;
using namespace std;

typedef struct day_weather {
	float max_temperature;
	float min_temperature;
	string date;
	string day_status;
	string night_status;
	string day_wind;
	string night_wind;
} DayWeather;

typedef struct area_weather {
	string area;
	string cur_date;
	float current_temperature;
	DayWeather today_weather;
	vector<DayWeather> future_7_weather;
} AreaWeather;

typedef struct date_conf {
	string date;
	int start;
	int end;
} DateConf;

typedef struct index {
	string name;
	string hint;
	string dess;
}Index;

typedef struct area_indices {
	string date;
	string area;
	vector<Index> indices;
} AreaIndices;

extern void DebugPB(WeatherResponse *w_resp);

class WeatherBusiHandler : public BusiHandlerBase
{
public:
	WeatherBusiHandler();
	//WeatherBusiHandler(DqaAnswerType ans_type);
	~WeatherBusiHandler();

	int PreHandle(string &query);
	int HandleRequest(ConditionContainer c_container, 
					Target target, MessageVec &mv);
	float GetWeight(GMessage *msg);
	string GetAnsText(GMessage *msg, Target target);
	string SerializeToString(GMessage *msg);
	void ReleaseMessage(MessageVec &mv);
	void FillPB(Weather *weather, DayWeather *day_weather); 
	int FindAreaWeather(string area, AreaWeather *area_weather);
	void LoadData(const uint64_t id);
	void LoadIndices();
private: 
	int CountBaseIndex(AreaWeather *area_weather);
	int ConvertDate(string date, DateConf *dc);
	string FormatDayStr();
private:
	//TimerManager m_timer_manager;
	vector<AreaWeather> real_time_area_weather;
	vector<AreaWeather> last_time_area_weather;
	map<string, AreaIndices*> area_indices_map;
	ConditionContainer cur_cond_container;
};

#endif
