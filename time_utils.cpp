/*
 * time_utils.cpp
 * Copyright (C) Tencent
 * Author: 2016 bookerbai <bookerbai@tencent.com>
 * Create time: 2016-06-18 16:47.
 * Distributed under terms of the Tencent license.
 */

#include "time_utils.h"
//#include "stdio.h"
namespace life_service{
    TimeCompareResult TimeUtils::compareStringTime(const string& time_str,
            const string& time_str_cmp,
            const string& str_format,
            const string& str_cmp_format){
        DateTime time1;
        DateTime time2;
        time1 = time2; //一定要赋值，不然缺失时间就用本身的补全，这样会出问题
        if(DateTime::TryParse(time_str, str_format, time1) &&
                    DateTime::TryParse(time_str_cmp, str_cmp_format, time2)){
            if(time1 > time2) return TIME_LARGE;
            if(time1 < time2) return TIME_SMALL;
            if(time1 == time2) return TIME_EQUAL;
        }
        return TIME_WRONG;
    }

    TimeCompareResult TimeUtils::compareStringTime(const string& time_str,
            const string& time_str_cmp,
            const string& str_format){
        return TimeUtils::compareStringTime(time_str, time_str_cmp, str_format, str_format);
    }

    bool TimeUtils::compareNow(const string& time_str, const string& str_format, double* seconds){
        DateTime time1;
        DateTime now(time1);
        if(DateTime::TryParse(time_str, str_format, time1)){
            TimeSpan ts = time1 - now;
            (*seconds) = ts.GetTotalSeconds();
            return true;
        }
        return false;
    }
    bool TimeUtils::compareNowDays(const string& time_str, const string& str_format, int * days){
        DateTime time1;
        DateTime now(time1);
        if(DateTime::TryParse(time_str, str_format, time1)){
            time1.SetHour(0);
            time1.SetMinute(0);
            time1.SetSecond(0);
            now.SetHour(0);
            now.SetMinute(0);
            now.SetSecond(0);
            TimeSpan ts = time1 - now;
            (*days) = ts.GetDays();
            return true;
        }
        return false;
    }


    bool TimeUtils::getDeltaMinutes(const string& time_str1, const string& time_str2, const string& str_format1, const string& str_format2, double* minutes){
        DateTime time1;
        DateTime time2(time1);
        if(DateTime::TryParse(time_str1, str_format1, time1) && DateTime::TryParse(time_str2, str_format2, time2)){
            TimeSpan ts =  time1 - time2;
            (*minutes) = ts.GetTotalMinutes();
            return true;
        }
        return false;
    }

    bool TimeUtils::getDeltaMinutes(const string& time_str1, const string& time_str2, const string& str_format, double* minutes){
        return TimeUtils::getDeltaMinutes(time_str1, time_str2, str_format, str_format, minutes);
    }

    bool TimeUtils::getDeltaDays(const string& time_str1,
            const string& time_str2, const string& str_format1, const string& str_format2, double* days){
        DateTime time1;
        DateTime time2(time1);
        if(DateTime::TryParse(time_str1, str_format1, time1) && DateTime::TryParse(time_str2, str_format2, time2)){
            TimeSpan ts =  time1 - time2;
            (*days) = ts.GetTotalDays();
            return true;
        }
        return false;

    }
    bool TimeUtils::getDeltaDays(const string& time_str1, const string& time_str2, const string& str_format, double* days){
        return TimeUtils::getDeltaDays(time_str1, time_str2, str_format, str_format, days);
    }
}





