/*
 * time_utils.h
 * Copyright (C) Tencent
 * Author: 2016 bookerbai <bookerbai@tencent.com>
 * Create time: 2016-06-18 16:26.
 * 跟时间相关的函数，输入字符串格式的标准时间，比较两个时间的大小
 * Distributed under terms of the Tencent license.
 */

#ifndef _TENCENT_TIME_UTILS_H
#define _TENCENT_TIME_UTILS_H
#include "common/system/time/datetime.h"
#include <string>
using namespace std;

namespace life_service{
/**
 * copy from common/system/time/datetime.h
日期标准格式模式和自定义格式模式:
1.标准格式模式，标准格式模式都用一个字符表示。参见MSDN文档：ms-help://MS.MSDNQTR.v90.chs/dv_fxfund/html/bb79761a-ca08-44ee-b142-b06b3e2fc22b.htm
格式模式    关联属性/说明
d           ShortDatePattern
D           LongDatePattern
f           完整日期和时间（长日期和短时间）
F           FullDateTimePattern（长日期和长时间）
g           常规（短日期和短时间）
G           常规（短日期和长时间）
m、M        MonthDayPattern
o、O        往返日期/时间模式；在这种格式模式下，格式设置或分析操作始终使用固定区域性
r、R        RFC1123Pattern；在这种格式模式下，格式设置或分析操作始终使用固定区域性
s           使用本地时间的 SortableDateTimePattern（基于 ISO 8601）；在这种格式模式下，格式设置或分析操作始终使用固定区域性
t           ShortTimePattern
T           LongTimePattern
u           使用通用时间显示格式的 UniversalSortableDateTimePattern；在这种格式模式下，格式设置或分析操作始终使用固定区域性
U           使用通用时间的完整日期和时间（长日期和长时间）
y、Y        YearMonthPattern

2.自定义格式模式，其他非模式字符应该用引号括起来。一个字符的自定义模式需要在前面加上%，以区别于标准格式模式
参见MSDN文档：ms-help://MS.MSDNQTR.v90.chs/dv_fxfund/html/98b374e3-0cc2-4c78-ab44-efb671d71984.htm
格式模式    说明
d、%d       月中的某一天。一位数的日期没有前导零。如果此格式模式没有与其他格式模式组合，应用程序将指定“%d”。
dd      月中的某一天。一位数的天有一个前导零。
ddd     周中某天的缩写名称，在 AbbreviatedDayNames 中定义。
dddd    周中某天的完整名称，在 DayNames 中定义。
f、%f   秒的小数精度为一位。其余数字被截断。如果此格式模式没有与其他格式模式组合，应用程序将指定“%f”。
ff      秒的小数精度为两位。其余数字被截断。
fff     秒的小数精度为三位。其余数字被截断。
ffff    秒的小数精度为四位。其余数字被截断。
fffff   秒的小数精度为五位。其余数字被截断。
ffffff  秒的小数精度为六位。其余数字被截断。
fffffff 秒的小数精度为七位。其余数字被截断。
F、%F   显示秒的小数部分的最高有效数字。如果该数字为零，则不显示任何内容。如果此格式模式没有与其他格式模式组合，应用程序将指定“%F”。
FF      显示秒的小数部分的两个最高有效数字。但是，不显示尾随的零（两个零数字）。
FFF     显示秒的小数部分的三个最高有效数字。但是，不显示尾随的零（三个零数字）。
FFFF    显示秒的小数部分的四个最高有效数字。但是，不显示尾随的零（四个零数字）。
FFFFF   显示秒的小数部分的五个最高有效数字。但是，不显示尾随的零（五个零数字）。
FFFFFF  显示秒的小数部分的六个最高有效数字。但是，不显示尾随的零（六个零数字）。
FFFFFFF 显示秒的小数部分的七个最高有效数字。但是，不显示尾随的零（七个零数字）。
gg      时期或纪元。如果要设置格式的日期不具有关联的时期或纪元字符串，则忽略该模式。
h、%h   12 小时制的小时。一位数的小时数没有前导零。如果此格式模式没有与其他格式模式组合，应用程序将指定“%h”。
hh      12 小时制的小时。一位数的小时有一个前导零。
H、%H   24 小时制的小时。一位数的小时数没有前导零。如果此格式模式没有与其他格式模式组合，应用程序将指定“%H”。
HH      24 小时制的小时。一位数的小时有一个前导零。
K       Kind 属性的不同值，即本地、Utc 或未指定。
m、%m   分钟。一位数的分钟数没有前导零。如果此格式模式没有与其他格式模式组合，应用程序将指定“%m”。
mm      分钟。一位数的分钟有一个前导零。
M、%M   月份数字。一位数的月份没有前导零。如果此格式模式没有与其他格式模式组合，应用程序将指定“%M”。
MM      月份数字。一位数的月份有一个前导零。
MMM     月份的缩写名称，在 AbbreviatedMonthNames 中定义。
MMMM    月份的完整名称，在 MonthNames 中定义。
s、%s   秒。一位数的秒数没有前导零。如果此格式模式没有与其他格式模式组合，应用程序将指定“%s”。
ss      秒。一位数的秒有一个前导零。
t、%t   在 AMDesignator 或 PMDesignator 中定义的 AM/PM 指示项的第一个字符（如果存在）。如果此格式模式没有与其他格式模式组合，应用程序将指定“%t”。
tt      在 AMDesignator 或 PMDesignator 中定义的 AM/PM 指示项（如果存在）。对于需要维护 AM 与 PM 之间的差异的语言，应用程序应使用此格式模式。以日语为例，其 AM 和 PM 指示符的差异点为第二个字符，而非第一个字符。
y、%y   不包含纪元的年份。如果不包含纪元的年份小于 10，则显示不具有前导零的年份。如果此格式模式没有与其他格式模式组合，应用程序将指定“%y”。
yy      不包含纪元的年份。如果不包含纪元的年份小于 10，则显示具有前导零的年份。
yyy     三位数的年份。如果年份小于 100，则会以带前导零的方式显示。
yyyy    包括纪元的四位或五位数的年份（取决于所使用的日历）。对于不够四位数的年份，将使用前导零填充。泰国佛历和朝鲜历采用五位数的年份。对于具有五位数的日历，选择“yyyy”模式的用户将看到不带前导零的所有这五位数。例外情况：对于日本历和台湾日历，其行为始终都像是用户选择了“yy”。
yyyyy   五位数的年份。对于不够五位数的年份，将使用前导零填充。例外情况：对于日本历和台湾日历，其行为始终都像是用户选择了“yy”。
yyyyyy  六位数的年份。对于不够六位数的年份，将使用前导零填充。例外情况：对于日本历和台湾日历，其行为始终都像是用户选择了“yy”。此模式可不断续加“y”，从而形成一个更长的字符串，这时将需要使用更多的前导零。
z、%z   时区偏移量（“+”或“-”后面仅跟小时）。一位数的小时数没有前导零。例如，太平洋标准时间是“-8”。如果此格式模式没有与其他格式模式组合，应用程序将指定“%z”。
zz      时区偏移量（“+”或“-”后面仅跟小时）。一位数的小时有一个前导零。例如，太平洋标准时间是“-08”。
zzz     完整时区偏移量（“+”或“-”后面跟有小时和分钟）。一位数的小时数和分钟数有前导零。例如，太平洋标准时间是“-08:00”。
:       在 TimeSeparator 中定义的默认时间分隔符。
/       在 DateSeparator 中定义的默认日期分隔符。
% c     其中 c 是格式模式（如果单独使用）。若要使用格式模式“d”、“f”、“F”、“h”、“m”、“s”、“t”、“y”、“z”、“H”、或“M”本身，应用程序应指定“%d”、“%f”、“%F”、“%h”、“%m”、“%s”、“%t”、“%y”、“%z”、“%H”或“%M”。如果格式模式与原义字符或其他格式模式合并，则可以省略“%”字符。
\ c     其中 c 是任意字符。照原义显示字符。若要显示反斜杠字符，应用程序应使用“\\”。
*/

    enum TimeCompareResult{
      TIME_LARGE,
      TIME_SMALL,
      TIME_EQUAL,
      TIME_WRONG,
    };
    class TimeUtils{
        public:
            static TimeCompareResult compareStringTime(const string& time_str, const string& time_str_cmp, const string& str_format, const string& str_cmp_format);
            static TimeCompareResult compareStringTime(const string& time_str, const string& time_str_cmp, const string& all_format);
            static bool compareNow(const string& time_str, const string& time_format, double* seconds);
            //计算两个字符串时间的时间差，返回的是能成功获取时间差，时间差放在minutes里面
            static bool getDeltaMinutes(const string& time_str1, const string& time_str2, const string& str_format1, const string& str_format2, double* minutes);
            static bool getDeltaMinutes(const string& time_str1, const string& time_str2, const string& str_format, double* minutes);
            static bool getDeltaDays(const string& time_str1, const string& time_str2, const string& str_format1, const string& str_format2, double* day);
            static bool getDeltaDays(const string& time_str1, const string& time_str2, const string& str_format, double* day);
            static bool compareNowDays(const string& time_str, const string& str_format, int * days);
    };
}

#endif /* !_TENCENT_TIME_UTILS_H */

