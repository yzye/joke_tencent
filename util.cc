#include "util.h"
#include <cstring>
#include <ctype.h>

// 对GBK编码的字符串转小写
void life_service::ToLower(std::string & str)
{
    if (str.size() == 0) return;

    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str.at(i) < 0)
        {
            ++i;
        }
        else
        {
            str[i] = (char)tolower(str.at(i));
        }
    }
}

void life_service::Split(const std::string &str, const std::string &delimer, std::vector<std::string> &parts)
{
    parts.clear();
    size_t sidx = 0;
    size_t eidx = str.find(delimer, sidx);
    while (std::string::npos != eidx)
    {   
        parts.push_back(str.substr(sidx, eidx - sidx));
        sidx = eidx + delimer.length();
        eidx = str.find(delimer, sidx);
    }   
    if (std::string::npos != sidx)
    {   
        parts.push_back(str.substr(sidx));
    }   
}

int life_service::Strcmpcase(const char* pos1, const char* pos2)
{
	/*
	std::string str1(pos1);
	std::string str2(pos2);
	if (str1 == str2)
	{
		return 0;
	}
	return -1;
	*/
	
    if (strlen(pos1) != strlen(pos2)) 
    {
        return -1;
    }

    for(unsigned int i=0; i<strlen(pos1); i++)
    {
        if(toupper(*(pos1+i)) != toupper(*(pos2+i)))
        {
            return -1;
        }
    }

    return 0;
	
}

/* 判断是否为合法保留字符[可能作为标点的字符] */
/* 大于32的可见字符，除去英文数字[包括.] */
bool life_service::IsReservedChar(const char *c)
{
    return (*c > 32 && !IsNumber(c) && !IsLetter(c));
}

/* 判断是否为标点字符 */
/* 调用此函数，应该确保从第二个字符开始*/
bool life_service::IsPuncChar(const char *c)
{
    return (IsReservedChar(c) || (*c == '.' && !(IsNumberExact(c-1) && IsNumberExact(c+1))));
}


/* 判断是否为保留的双字节字符汉字([A4A0 - A5FF]日文片假名、书名号《》、中文零〇)*/
bool life_service::IsReservedGBK(const char *c)
{
	uint8_t c1 = (uint8_t)(*c);
	uint8_t c2 = (uint8_t)(*(c+1));
	
	//[A1A0-A1FF]全角中文标点符号，支持一部分
	if (c1 == 0xA1)
	{
	/*	if ((c2 >= 0xB2 && c2 <= 0xBF) ||	//〔 〕〈 〉 《 》 「 」 『 』 〖 〗【 】
			(c2 == 0xC2)               ||	//÷
			(c2 >= 0xC4 && c2 <= 0xCA) ||	//∧ ∨ ∑ ∏ ∪ ∩ ∈
			(c2 >= 0xCD && c2 <= 0xE0) ||	//⊥ ∥ ∠ ⌒ ⊙∫ ∮ ≡ ≌ ≈ ∽ ∝ ≠ ≮ ≯ ≤ ≥ ∞ ∵ ∴ 
			(c2 >= 0xE6 && c2 <= 0xED))		//℃ ＄ ¤ ￠ ￡ ‰ § №
		return true;
	*/
		if (A1char[c2-160] == 1)
			return true;
	}
	//[A2A0-A2FF]罗马数字，全部支持
	if (c1 == 0xA2 && c2 >= 0xA0)
		return true;

        //[A3A0-A3FF] 全角英文、数字，符号，支持￥
        if (c1 == 0xA3)
        {
          if ( c2 == 0xA4   //￥
              ||c2==0xAB
              ||c2==0xDF )  //
             return true;
        }
	//[A4A0-A5FF]日本片假名，全部支持 [A960-A962]
	if ((c1 == 0xA4 || c1 == 0xA5) && (c2 >= 0xA0))
		return true;
        if(c1 == 0xA9 && (c2>=0x60 && c2<=0x62))
                return true;

        //[A6C1-A6D8][A6A1-A6B8] 希腊字母
        if(c1 == 0xA6)
        {
          if( c2 == 0xB7 || c2==0xF5
             || (c2>= 0xC1 && c2 <= 0xD8)
             || (c2 >=0xA1 && c2 <= 0xB8))
            return true;
        }
       
       //俄文
      if(c1 == 0xA7)
      {
         if((c2>=0xA1 && c2 <=0xC1)
            ||(c2 >= 0xD1 && c2 <= 0xF1))
          return  true;
      }
     
      //注音
      if(c1 == 0xA8)
      {
        if((c2 >=0xC5 && c2 <=0xE9)
           ||(c2 >= 0xA1 && c2 <= 0xBA)
           ||(c2 >= 0x87 && c2 <= 0x8C)
           ||(c2 >= 0x78 && c2 <= 0x7E)
           ||(c2 >= 0x47 && c2 <= 0x4C)
           ||(c2 >= 0x8D && c2 <= 0x90)
           || c2==0x80 || c2==0x44 || c2==0x75 || c2==0x76 || c2==0x93
          )
         return true;
      }

      //[A840-A9A0]非汉字符号、结构符和“〇”排列在此区，支持中文零〇
      //if ((c1 == 0xA9) && (c2 == 0x96))	//中文零〇
        //	return true;

      if(c1 == 0xA9)
      {
         if((c2>=0x49 && c2<=0x54)
            ||(c2 >=0x68 && c2<=0x6E)
            || c2 == 0x85 || c2 == 0x56
            || c2 == 0x7E || c2 == 0x5C || c2 == 0x96
            || c2 == 0x59 || c2 == 0x5A || c2 == 0x7D || c2 == 0x87
            )
           return true;
      }

	return false;
}


/* 判断是否为合法的GBK双字节字符[IsChinese和IsReservedGBK]*/
bool life_service::IsValidGBK(const char *c)
{
    return (IsChinese(c) || IsReservedGBK(c));
}


/* 判断是否为合法的字符词 */
bool life_service::IsValidCharWord(const char *c)
{
    //以英文、纯数字开头的词认为是合法字符词
    return (IsLetter(c) || IsNumberExact(c));
}


/* 判断是否为合法的中文词 */
bool life_service::IsValidChineseWord(const char *c)
{
    //以合法GBK开头的词认为是合法中文词
    return IsValidGBK(c);
}
/*判断是否为纯中文词*/
bool life_service::IsStrictGBKChar(const char* p)
{
        uint8_t c1 =  (uint8_t)(*p);
	    uint8_t c2 = (uint8_t)(*(p+1));

        return(((c1 >= 0xA1 && c1 <= 0xA9) && (c2 >= 0xA0/* && c2 <= 0xFF*/)) ||
               ((c1 >= 0xA8 && c1 <= 0xA9) && (c2 >= 0x40/* && c2 <= 0xFF*/)) ||
               ((c1 >= 0xB0 && c1 <= 0xF7) && (c2 >= 0xA0/* && c2 <= 0xFF*/)) ||
               ((c1 >= 0x81 && c1 <= 0xA0) && (c2 >= 0x40/* && c2 <= 0xFF*/)) ||
               ((c1 >= 0xAA && c1 <= 0xFE) && (c2 >= 0x40 && c2 <= 0xAF)));
    }

/* 判断是否为合法的特殊词 */

bool life_service::IsReservedWord(const char *c)
{
    //特殊的词
    if (*c == '.')
    {
        //return (*((TUINT32*)IndexDefine::TranToLower(c)) == *((TUINT32*)".net"));
        return ((*(c+1) == 'N' || *(c+1) == 'n') &&
                (*(c+2) == 'E' || *(c+2) == 'e') &&
                (*(c+3) == 'T' || *(c+3) == 't')); // 不要改变输入串的内容
    }
    if (*c == '$')
    {
        return (IsNumberExact(c+1));
    }
    return false;
}


/* 判断是否为合法的词 */
bool life_service::IsValidWord(const char *c)
{
    return (IsValidCharWord(c) || IsValidChineseWord(c) || IsReservedWord(c));
}


/* 判断是否为不合法可丢弃的词[包括空格] */
bool life_service::IsIgnoreWord(const char *c)
{
    return (!IsValidWord(c));
}


//判断是否为纯数字词
bool life_service::IsNumberExactWord(const char *c)
{
    if (!IsValidCharWord(c))
        return false;

    const char* p = c;		
    while(*p != 0)
    {
        //如果不是合法字符
        if (!IsNumberExact(p))
            return false;			
        p++;
    }
    return true;
}


bool life_service::IsNumberWord(const char *c)
{
  if(!IsValidCharWord(c))
    return false;
   
  const char *p = c;
  while( *p != 0)
  {
     if(!IsNumber(p))
        return false;
     p++;
  }
   return true;
}

/* 判断是否为英文数字词 */
bool life_service::IsLetterNumberWord(const char *c)
{
    if (!IsValidCharWord(c))
        return false;

    const char* p = c + 1;
    int dot_cnt = 0;
    while(*p != 0)
    {
        //如果不是合法字符
        if (!IsValidChar(p))
            return false;

        //如果是标点符号
        if (IsPuncChar(p))
            return false;	
        else if (*p == '.')
            dot_cnt ++;
        p++;
    }
    return (dot_cnt < 2);
}

/* 判断是否为英文数字标点词 */
bool life_service::IsLetterNumberPuncWord(const char *c)
{
    if (!IsValidCharWord(c))
        return false;

    const char* p = c + 1;
    bool bHasPunc = false;
    int dot_cnt = 0;
    while(*p != 0)
    {
        //如果不是合法字符
        if (!IsValidChar(p))
            return false;

        //如果是标点符号
        if (IsPuncChar(p))
            bHasPunc = true;
        else if ( *p == '.')
            dot_cnt ++;

        p ++;
    }
    return (bHasPunc || dot_cnt > 1);
}


/* 判断是否为url的英文数字标点词 */
bool life_service::IsLetterNumberPuncUrlWord(const char *c, int* cnt/* = NULL*/)
{
    //调用此函数的前提是IsLetterNumberPuncWord,因此屏蔽掉次判断
    //if (!IsLetterNumberPuncWord(c))
    //	return false;

    if (cnt != NULL)
        *cnt = 0;
    const char* p = c + 1;
    while(*p != 0)
    {
        //如果包含有@，作为email地址，不作为url
        if (*p == '@')
            return false;
        if (*p == '.' && cnt != NULL)
            (*cnt) ++;
        p ++;
    }

    int slashnum = 0;
    char* tmpend = const_cast<char*>(c) + strlen(c) - 1;
    bool flag = false;

    while( *tmpend == '/')
    {
        *tmpend = 0; 
        tmpend --;
        slashnum ++;
    }

    int len = strlen(c);

    //如果字符串以下列子串开头，认为是url
    if ((len > 5 && strncmp(c, "http:",  5) == 0) ||
        (len > 4 && strncmp(c, "ftp:",   4) == 0) ||
        (len > 6 && strncmp(c, "https:", 6) == 0) ||
        (len > 4 && strncmp(c, "mms:",   4) == 0) ||
        (len > 4 && strncmp(c, "www.",   4) == 0)  )
        flag = true;
    //return true;

    //如果字符串以下列子串结尾，认为是url
    if (!flag && ((len > 4 && strncmp(c+len-4, ".com",  4)  == 0) ||
        (len > 4 && strncmp(c+len-4, ".edu",  4)  == 0) ||
        (len > 4 && strncmp(c+len-4, ".gov",  4)  == 0) ||
        (len > 4 && strncmp(c+len-4, ".net",  4)  == 0) ||
        (len > 4 && strncmp(c+len-4, ".org",  4)  == 0) ||
        (len > 4 && strncmp(c+len-4, ".com",  4)  == 0) ||
        (len > 3 && strncmp(c+len-3, ".cn",   3)  == 0) ||
        (len > 4 && strncmp(c+len-4, ".htm",  4)  == 0) ||
        (len > 4 && strncmp(c+len-4, ".asp",  4)  == 0) ||
        (len > 4 && strncmp(c+len-4, ".jsp",  4)  == 0) ||
        (len > 4 && strncmp(c+len-4, ".php",  4)  == 0) ||
        (len > 4 && strncmp(c+len-4, ".mht",  4)  == 0) ||
        (len > 5 && strncmp(c+len-5, ".html", 5)  == 0) ||
        (len > 5 && strncmp(c+len-5, ".aspx", 5)  == 0) ||
        (len > 6 && strncmp(c+len-6, ".shtml",6)  == 0) ) )

        flag = true;

    tmpend = const_cast<char*>(c) + len;

    while ( slashnum > 0)
    {
        *tmpend = '/';
        tmpend ++;
        slashnum --;
    }
    return flag;	
}


/* 判断是否为email的英文数字标点词 */
bool life_service::IsLetterNumberPuncEmailWord(const char *c, int* cnt/* = NULL*/)
{
    //调用此函数的前提是IsLetterNumberPuncWord,因此屏蔽掉次判断
    //if (!IsLetterNumberPuncWord(c))
    //	return false;

    if (cnt != NULL)
        *cnt = 0;
    const char* p = c + 1;
    bool exist = false;
    while(*p != 0)
    {
        if (*p == '@')
            exist = true;
        if (*p == '.' && cnt != NULL)
            (*cnt) ++;
        p ++;
    }

    if ( !exist) //如果不包含有@，不作为email地址
        return false;

    int len = strlen(c);		

    //如果字符串以下列子串结尾，认为是email
    if ((len > 4 && strncmp(c+len-4, ".com",  4)  == 0) ||
        (len > 4 && strncmp(c+len-4, ".edu",  4)  == 0) ||
        (len > 4 && strncmp(c+len-4, ".gov",  4)  == 0) ||
        (len > 4 && strncmp(c+len-4, ".net",  4)  == 0) ||
        (len > 4 && strncmp(c+len-4, ".org",  4)  == 0) ||
        (len > 4 && strncmp(c+len-4, ".com",  4)  == 0) ||
        (len > 3 && strncmp(c+len-3, ".cn",   3)  == 0) )

        return true;
    return false;
}


/* 英文大写转小写 */
char* life_service::TranToLower(char* p)
{
    char* src = p;

    while(*p != 0)
    {
        if (*p > 0)
        {
            if (*p >= 'A' && *p <= 'Z')
                *p += 32;
            p ++;
        }
        else //if (*p < 0)
        {
            p ++;
            if (*p != 0)
                p++;
        }
    }

    return src;
}
