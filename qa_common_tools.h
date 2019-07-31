#ifndef _QA_COMMON_TOOLS_H_
#define _QA_COMMON_TOOLS_H_

#include <iostream>
#include <string>
#include "qa_log.h"
#include "common/encoding/charset_converter.h"

using namespace std;

extern string ConvUtf8ToGbk(string utf8_str);

extern string ConvGbkToUtf8(string gbk_str);

extern void SimilarReplace(string &query, string rd, string re);
#endif
