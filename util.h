// 工具类头文件

#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>
#include <vector>
#include <stdint.h>
namespace life_service{

	const uint8_t A1char[100] = {
                0,0,0,0,1,1,1,0,0,1,
                1,0,0,1,0,0,0,0,0,0,
                1,1,1,1,1,1,1,1,0,0,
                1,1,0,0,1,1,1,0,1,1,
                1,1,1,0,0,1,0,1,1,0,
                1,1,0,1,1,1,1,1,1,1,
                0,0,1,1,1,0,0,0,1,0,
                1,0,0,1,1,1,0,1,0,0,
                0,0,0,0,0,0,0,1,0,0,
                1,0,0,1,0,0,0,0,0,0
        };


void ToLower(std::string & str);

void Split(const std::string &str, const std::string &delimer, std::vector<std::string> &parts);

int Strcmpcase(const char* pos1, const char* pos2);

//================================[编码范围判断]=======================================//
/*
IsChar -- IsValidChar -- |-- IsLetter
          |              |-- IsNumber
          |              |-- IsReservedChar
          |-- else

IsNotChar -- IsGBK -- |-- IsValidGBK --|-- IsChinese
          |           |                |-- IsReservedGBK
          |           |-- else
          |-- else
  
*/


/* 判断是否为字符 */
inline bool IsChar(const char *c)
{
    return (*c > 0);
}


/* 判断是否为合法字母 */
inline bool IsLetter(const char *c)
{
    return (((*c) >= 'a' && (*c) <= 'z') || ((*c) >= 'A' && (*c) <= 'Z') || ((*c) == '\''));
}


/* 判断是否为合法数字 */
inline bool IsNumber(const char *c)
{
    return (((*c) >= '0' && (*c) <= '9') || (*c) == '.');
}

/* 判断是否为合法纯数字 */
inline bool IsNumberExact(const char *c)
{
    return (*c >= '0' && *c <= '9');
}

/* 判断是否为合法字符[合法字母+合法数字+合法保留字符] */
inline bool IsValidChar(const char *c)
{
    //return (IsLetter(c) || IsNumber(c) || IsReservedChar(c));
    return (*c > 32);
}

/* 判断是否为连字符 */
/* 调用此函数的前提是已经为IsPuncChar */
inline bool IsPuncConnChar(const char *c)
{
    return (*c == '-' || *c == '_' || *c == '.');
}


/* 判断是否为合法保留字符[可能作为标点的字符] */
/* 大于32的可见字符，除去英文数字[包括.] */
bool IsReservedChar(const char *c);


/* 判断是否为标点字符 */
/* 调用此函数，应该确保从第二个字符开始*/
bool IsPuncChar(const char *c);


//////////////////////////////////////////////////////////////////////////
// Chinese Word
/* 判断是否为双字节字符 */
inline bool IsNotChar(const char *c)
{
    char c1 = (*c);
    char c2 = (*(c+1));

    return ((c1 < 0) && (c2 != 0));
}


/* 判断是否为GBK双字节字符[8140-FEFE] */
inline bool IsGBK(const char *c)
{
    uint8_t c1 = (uint8_t)(*c);
    uint8_t c2 = (uint8_t)(*(c+1));

    return ((c1 >= 0x81) && (c2 >= 0x40) && (c1 <= 0xFE) && (c2 <= 0xFE));
}


/* 判断是否为汉字([B0A1-F7FE],[8140-A0FE],[AA40-FEA0])*/
inline bool IsChinese(const char *c)
{
    uint8_t c1 = (uint8_t)(*c);
    uint8_t c2 = (uint8_t)(*(c+1));

    return (((c1 >= 0xB0) && (c2 >= 0xA1) && (c1 <= 0xF7) && (c2 <= 0xFE)) ||
        ((c1 >= 0x81) && (c2 >= 0x40) && (c1 <= 0xA0) && (c2 <= 0xFE)) ||
        ((c1 >= 0xAA) && (c2 >= 0x40) && (c1 <= 0xFE) && (c2 <= 0xA0)));
}


/* 判断是否为保留的双字节字符汉字([A4A0 - A5FF]日文片假名、书名号《》、中文零〇)*/
bool IsReservedGBK(const char *c);


/* 判断是否为合法的GBK双字节字符[IsChinese和IsReservedGBK]*/
bool IsValidGBK(const char *c);

//================================[编码词范围判断]=====================================//
/*
word   -- IsValidWord -- IsValidCharWord 
          |          |-- IsValidChineseWord
          |          |-- IsReservedWord
          |-- IsIgnoreWord
*/

/* 判断是否为合法的字符词 */
bool IsValidCharWord(const char *c);


/* 判断是否为合法的中文词 */
bool IsValidChineseWord(const char *c);

/*判断是否为纯中文词*/
bool IsStrictGBKChar(const char* p);

/* 判断是否为合法的特殊词 */
bool IsReservedWord(const char *c);


/* 判断是否为合法的词 */
bool IsValidWord(const char *c);


/* 判断是否为不合法可丢弃的词[包括空格] */
bool IsIgnoreWord(const char *c);


//================================[需特殊处理词判断]===================================//
/*
IsValidCharWord -- IsLetterNumberWord -- IsLetterNumberSpecialWord
          |                          |-- else
		  |-- IsLetterNumberPuncWord  -- IsLetterNumberPuncSpecialWord							 
                                     |-- IsLetterNumberPuncUrlWord 
                                     |-- else	  
*/

//判断是否为数字词
bool IsNumberWord(const char *);

//判断是否为纯数字词
bool IsNumberExactWord(const char *c);


/* 判断是否为英文数字词 */
bool IsLetterNumberWord(const char *c);

/* 判断是否为英文数字标点词 */
bool IsLetterNumberPuncWord(const char *c);



/* 判断是否为url的英文数字标点词 */
bool IsLetterNumberPuncUrlWord(const char *c, int* cnt = NULL);


/* 判断是否为email的英文数字标点词 */
bool IsLetterNumberPuncEmailWord(const char *c, int* cnt = NULL);


/* 英文大写转小写 */
char* TranToLower(char* p);

}
#endif
