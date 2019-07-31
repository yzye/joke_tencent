/*
 * time_utils.h
 * Copyright (C) Tencent
 * Author: 2016 bookerbai <bookerbai@tencent.com>
 * Create time: 2016-06-18 16:26.
 * ��ʱ����صĺ����������ַ�����ʽ�ı�׼ʱ�䣬�Ƚ�����ʱ��Ĵ�С
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
���ڱ�׼��ʽģʽ���Զ����ʽģʽ:
1.��׼��ʽģʽ����׼��ʽģʽ����һ���ַ���ʾ���μ�MSDN�ĵ���ms-help://MS.MSDNQTR.v90.chs/dv_fxfund/html/bb79761a-ca08-44ee-b142-b06b3e2fc22b.htm
��ʽģʽ    ��������/˵��
d           ShortDatePattern
D           LongDatePattern
f           �������ں�ʱ�䣨�����ںͶ�ʱ�䣩
F           FullDateTimePattern�������ںͳ�ʱ�䣩
g           ���棨�����ںͶ�ʱ�䣩
G           ���棨�����ںͳ�ʱ�䣩
m��M        MonthDayPattern
o��O        ��������/ʱ��ģʽ�������ָ�ʽģʽ�£���ʽ���û��������ʼ��ʹ�ù̶�������
r��R        RFC1123Pattern�������ָ�ʽģʽ�£���ʽ���û��������ʼ��ʹ�ù̶�������
s           ʹ�ñ���ʱ��� SortableDateTimePattern������ ISO 8601���������ָ�ʽģʽ�£���ʽ���û��������ʼ��ʹ�ù̶�������
t           ShortTimePattern
T           LongTimePattern
u           ʹ��ͨ��ʱ����ʾ��ʽ�� UniversalSortableDateTimePattern�������ָ�ʽģʽ�£���ʽ���û��������ʼ��ʹ�ù̶�������
U           ʹ��ͨ��ʱ����������ں�ʱ�䣨�����ںͳ�ʱ�䣩
y��Y        YearMonthPattern

2.�Զ����ʽģʽ��������ģʽ�ַ�Ӧ����������������һ���ַ����Զ���ģʽ��Ҫ��ǰ�����%���������ڱ�׼��ʽģʽ
�μ�MSDN�ĵ���ms-help://MS.MSDNQTR.v90.chs/dv_fxfund/html/98b374e3-0cc2-4c78-ab44-efb671d71984.htm
��ʽģʽ    ˵��
d��%d       ���е�ĳһ�졣һλ��������û��ǰ���㡣����˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%d����
dd      ���е�ĳһ�졣һλ��������һ��ǰ���㡣
ddd     ����ĳ�����д���ƣ��� AbbreviatedDayNames �ж��塣
dddd    ����ĳ����������ƣ��� DayNames �ж��塣
f��%f   ���С������Ϊһλ���������ֱ��ضϡ�����˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%f����
ff      ���С������Ϊ��λ���������ֱ��ضϡ�
fff     ���С������Ϊ��λ���������ֱ��ضϡ�
ffff    ���С������Ϊ��λ���������ֱ��ضϡ�
fffff   ���С������Ϊ��λ���������ֱ��ضϡ�
ffffff  ���С������Ϊ��λ���������ֱ��ضϡ�
fffffff ���С������Ϊ��λ���������ֱ��ضϡ�
F��%F   ��ʾ���С�����ֵ������Ч���֡����������Ϊ�㣬����ʾ�κ����ݡ�����˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%F����
FF      ��ʾ���С�����ֵ����������Ч���֡����ǣ�����ʾβ����㣨���������֣���
FFF     ��ʾ���С�����ֵ����������Ч���֡����ǣ�����ʾβ����㣨���������֣���
FFFF    ��ʾ���С�����ֵ��ĸ������Ч���֡����ǣ�����ʾβ����㣨�ĸ������֣���
FFFFF   ��ʾ���С�����ֵ���������Ч���֡����ǣ�����ʾβ����㣨��������֣���
FFFFFF  ��ʾ���С�����ֵ����������Ч���֡����ǣ�����ʾβ����㣨���������֣���
FFFFFFF ��ʾ���С�����ֵ��߸������Ч���֡����ǣ�����ʾβ����㣨�߸������֣���
gg      ʱ�ڻ��Ԫ�����Ҫ���ø�ʽ�����ڲ����й�����ʱ�ڻ��Ԫ�ַ���������Ը�ģʽ��
h��%h   12 Сʱ�Ƶ�Сʱ��һλ����Сʱ��û��ǰ���㡣����˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%h����
hh      12 Сʱ�Ƶ�Сʱ��һλ����Сʱ��һ��ǰ���㡣
H��%H   24 Сʱ�Ƶ�Сʱ��һλ����Сʱ��û��ǰ���㡣����˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%H����
HH      24 Сʱ�Ƶ�Сʱ��һλ����Сʱ��һ��ǰ���㡣
K       Kind ���ԵĲ�ֵͬ�������ء�Utc ��δָ����
m��%m   ���ӡ�һλ���ķ�����û��ǰ���㡣����˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%m����
mm      ���ӡ�һλ���ķ�����һ��ǰ���㡣
M��%M   �·����֡�һλ�����·�û��ǰ���㡣����˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%M����
MM      �·����֡�һλ�����·���һ��ǰ���㡣
MMM     �·ݵ���д���ƣ��� AbbreviatedMonthNames �ж��塣
MMMM    �·ݵ��������ƣ��� MonthNames �ж��塣
s��%s   �롣һλ��������û��ǰ���㡣����˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%s����
ss      �롣һλ��������һ��ǰ���㡣
t��%t   �� AMDesignator �� PMDesignator �ж���� AM/PM ָʾ��ĵ�һ���ַ���������ڣ�������˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%t����
tt      �� AMDesignator �� PMDesignator �ж���� AM/PM ָʾ�������ڣ���������Ҫά�� AM �� PM ֮��Ĳ�������ԣ�Ӧ�ó���Ӧʹ�ô˸�ʽģʽ��������Ϊ������ AM �� PM ָʾ���Ĳ����Ϊ�ڶ����ַ������ǵ�һ���ַ���
y��%y   ��������Ԫ����ݡ������������Ԫ�����С�� 10������ʾ������ǰ�������ݡ�����˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%y����
yy      ��������Ԫ����ݡ������������Ԫ�����С�� 10������ʾ����ǰ�������ݡ�
yyy     ��λ������ݡ�������С�� 100������Դ�ǰ����ķ�ʽ��ʾ��
yyyy    ������Ԫ����λ����λ������ݣ�ȡ������ʹ�õ������������ڲ�����λ������ݣ���ʹ��ǰ������䡣̩�������ͳ�����������λ������ݡ����ھ�����λ����������ѡ��yyyy��ģʽ���û�����������ǰ�������������λ������������������ձ�����̨������������Ϊʼ�ն������û�ѡ���ˡ�yy����
yyyyy   ��λ������ݡ����ڲ�����λ������ݣ���ʹ��ǰ������䡣��������������ձ�����̨������������Ϊʼ�ն������û�ѡ���ˡ�yy����
yyyyyy  ��λ������ݡ����ڲ�����λ������ݣ���ʹ��ǰ������䡣��������������ձ�����̨������������Ϊʼ�ն������û�ѡ���ˡ�yy������ģʽ�ɲ������ӡ�y�����Ӷ��γ�һ���������ַ�������ʱ����Ҫʹ�ø����ǰ���㡣
z��%z   ʱ��ƫ��������+����-���������Сʱ����һλ����Сʱ��û��ǰ���㡣���磬̫ƽ���׼ʱ���ǡ�-8��������˸�ʽģʽû����������ʽģʽ��ϣ�Ӧ�ó���ָ����%z����
zz      ʱ��ƫ��������+����-���������Сʱ����һλ����Сʱ��һ��ǰ���㡣���磬̫ƽ���׼ʱ���ǡ�-08����
zzz     ����ʱ��ƫ��������+����-���������Сʱ�ͷ��ӣ���һλ����Сʱ���ͷ�������ǰ���㡣���磬̫ƽ���׼ʱ���ǡ�-08:00����
:       �� TimeSeparator �ж����Ĭ��ʱ��ָ�����
/       �� DateSeparator �ж����Ĭ�����ڷָ�����
% c     ���� c �Ǹ�ʽģʽ���������ʹ�ã�����Ҫʹ�ø�ʽģʽ��d������f������F������h������m������s������t������y������z������H������M������Ӧ�ó���Ӧָ����%d������%f������%F������%h������%m������%s������%t������%y������%z������%H����%M���������ʽģʽ��ԭ���ַ���������ʽģʽ�ϲ��������ʡ�ԡ�%���ַ���
\ c     ���� c �������ַ�����ԭ����ʾ�ַ�����Ҫ��ʾ��б���ַ���Ӧ�ó���Ӧʹ�á�\\����
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
            //���������ַ���ʱ���ʱ�����ص����ܳɹ���ȡʱ��ʱ������minutes����
            static bool getDeltaMinutes(const string& time_str1, const string& time_str2, const string& str_format1, const string& str_format2, double* minutes);
            static bool getDeltaMinutes(const string& time_str1, const string& time_str2, const string& str_format, double* minutes);
            static bool getDeltaDays(const string& time_str1, const string& time_str2, const string& str_format1, const string& str_format2, double* day);
            static bool getDeltaDays(const string& time_str1, const string& time_str2, const string& str_format, double* day);
            static bool compareNowDays(const string& time_str, const string& str_format, int * days);
    };
}

#endif /* !_TENCENT_TIME_UTILS_H */

