
#include "qa_common_tools.h"
#include "common/base/string/algorithm.h"

string ConvUtf8ToGbk(string utf8_str)
{
	string gbk_str;
	if (!ConvertUtf8ToGbk(utf8_str, &gbk_str))
	{
		//LOG_PRINT(LOG_NORMAL, "convert failed\n");
		gbk_str = utf8_str;
	}
	return gbk_str;
}

string ConvGbkToUtf8(string gbk_str)
{
	string utf8_str;
	if (!ConvertGbkToUtf8(gbk_str, &utf8_str))
	{
		//LOG_PRINT(LOG_NORMAL, "convert failed\n");
		utf8_str = gbk_str;
	}
	return utf8_str;
}

void SimilarReplace(string &query, string rd, string re)
{
	string gbk_query = ConvUtf8ToGbk(query);
	string gbk_rd = ConvUtf8ToGbk(rd);
	string gbk_re = ConvUtf8ToGbk(re);

	ReplaceAll(&gbk_query, rd, re);
	query = ConvGbkToUtf8(gbk_query);
}
