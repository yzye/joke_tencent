#include "util.h"
#include <cstring>
#include <ctype.h>

// ��GBK������ַ���תСд
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

/* �ж��Ƿ�Ϊ�Ϸ������ַ�[������Ϊ�����ַ�] */
/* ����32�Ŀɼ��ַ�����ȥӢ������[����.] */
bool life_service::IsReservedChar(const char *c)
{
    return (*c > 32 && !IsNumber(c) && !IsLetter(c));
}

/* �ж��Ƿ�Ϊ����ַ� */
/* ���ô˺�����Ӧ��ȷ���ӵڶ����ַ���ʼ*/
bool life_service::IsPuncChar(const char *c)
{
    return (IsReservedChar(c) || (*c == '.' && !(IsNumberExact(c-1) && IsNumberExact(c+1))));
}


/* �ж��Ƿ�Ϊ������˫�ֽ��ַ�����([A4A0 - A5FF]����Ƭ�����������š����������㩖)*/
bool life_service::IsReservedGBK(const char *c)
{
	uint8_t c1 = (uint8_t)(*c);
	uint8_t c2 = (uint8_t)(*(c+1));
	
	//[A1A0-A1FF]ȫ�����ı����ţ�֧��һ����
	if (c1 == 0xA1)
	{
	/*	if ((c2 >= 0xB2 && c2 <= 0xBF) ||	//�� ���� �� �� �� �� �� �� �� �� ���� ��
			(c2 == 0xC2)               ||	//��
			(c2 >= 0xC4 && c2 <= 0xCA) ||	//�� �� �� �� �� �� ��
			(c2 >= 0xCD && c2 <= 0xE0) ||	//�� �� �� �� �ѡ� �� �� �� �� �� �� �� �� �� �� �� �� �� �� 
			(c2 >= 0xE6 && c2 <= 0xED))		//�� �� �� �� �� �� �� ��
		return true;
	*/
		if (A1char[c2-160] == 1)
			return true;
	}
	//[A2A0-A2FF]�������֣�ȫ��֧��
	if (c1 == 0xA2 && c2 >= 0xA0)
		return true;

        //[A3A0-A3FF] ȫ��Ӣ�ġ����֣����ţ�֧�֣�
        if (c1 == 0xA3)
        {
          if ( c2 == 0xA4   //��
              ||c2==0xAB
              ||c2==0xDF )  //
             return true;
        }
	//[A4A0-A5FF]�ձ�Ƭ������ȫ��֧�� [A960-A962]
	if ((c1 == 0xA4 || c1 == 0xA5) && (c2 >= 0xA0))
		return true;
        if(c1 == 0xA9 && (c2>=0x60 && c2<=0x62))
                return true;

        //[A6C1-A6D8][A6A1-A6B8] ϣ����ĸ
        if(c1 == 0xA6)
        {
          if( c2 == 0xB7 || c2==0xF5
             || (c2>= 0xC1 && c2 <= 0xD8)
             || (c2 >=0xA1 && c2 <= 0xB8))
            return true;
        }
       
       //����
      if(c1 == 0xA7)
      {
         if((c2>=0xA1 && c2 <=0xC1)
            ||(c2 >= 0xD1 && c2 <= 0xF1))
          return  true;
      }
     
      //ע��
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

      //[A840-A9A0]�Ǻ��ַ��š��ṹ���͡����������ڴ�����֧�������㩖
      //if ((c1 == 0xA9) && (c2 == 0x96))	//�����㩖
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


/* �ж��Ƿ�Ϊ�Ϸ���GBK˫�ֽ��ַ�[IsChinese��IsReservedGBK]*/
bool life_service::IsValidGBK(const char *c)
{
    return (IsChinese(c) || IsReservedGBK(c));
}


/* �ж��Ƿ�Ϊ�Ϸ����ַ��� */
bool life_service::IsValidCharWord(const char *c)
{
    //��Ӣ�ġ������ֿ�ͷ�Ĵ���Ϊ�ǺϷ��ַ���
    return (IsLetter(c) || IsNumberExact(c));
}


/* �ж��Ƿ�Ϊ�Ϸ������Ĵ� */
bool life_service::IsValidChineseWord(const char *c)
{
    //�ԺϷ�GBK��ͷ�Ĵ���Ϊ�ǺϷ����Ĵ�
    return IsValidGBK(c);
}
/*�ж��Ƿ�Ϊ�����Ĵ�*/
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

/* �ж��Ƿ�Ϊ�Ϸ�������� */

bool life_service::IsReservedWord(const char *c)
{
    //����Ĵ�
    if (*c == '.')
    {
        //return (*((TUINT32*)IndexDefine::TranToLower(c)) == *((TUINT32*)".net"));
        return ((*(c+1) == 'N' || *(c+1) == 'n') &&
                (*(c+2) == 'E' || *(c+2) == 'e') &&
                (*(c+3) == 'T' || *(c+3) == 't')); // ��Ҫ�ı����봮������
    }
    if (*c == '$')
    {
        return (IsNumberExact(c+1));
    }
    return false;
}


/* �ж��Ƿ�Ϊ�Ϸ��Ĵ� */
bool life_service::IsValidWord(const char *c)
{
    return (IsValidCharWord(c) || IsValidChineseWord(c) || IsReservedWord(c));
}


/* �ж��Ƿ�Ϊ���Ϸ��ɶ����Ĵ�[�����ո�] */
bool life_service::IsIgnoreWord(const char *c)
{
    return (!IsValidWord(c));
}


//�ж��Ƿ�Ϊ�����ִ�
bool life_service::IsNumberExactWord(const char *c)
{
    if (!IsValidCharWord(c))
        return false;

    const char* p = c;		
    while(*p != 0)
    {
        //������ǺϷ��ַ�
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

/* �ж��Ƿ�ΪӢ�����ִ� */
bool life_service::IsLetterNumberWord(const char *c)
{
    if (!IsValidCharWord(c))
        return false;

    const char* p = c + 1;
    int dot_cnt = 0;
    while(*p != 0)
    {
        //������ǺϷ��ַ�
        if (!IsValidChar(p))
            return false;

        //����Ǳ�����
        if (IsPuncChar(p))
            return false;	
        else if (*p == '.')
            dot_cnt ++;
        p++;
    }
    return (dot_cnt < 2);
}

/* �ж��Ƿ�ΪӢ�����ֱ��� */
bool life_service::IsLetterNumberPuncWord(const char *c)
{
    if (!IsValidCharWord(c))
        return false;

    const char* p = c + 1;
    bool bHasPunc = false;
    int dot_cnt = 0;
    while(*p != 0)
    {
        //������ǺϷ��ַ�
        if (!IsValidChar(p))
            return false;

        //����Ǳ�����
        if (IsPuncChar(p))
            bHasPunc = true;
        else if ( *p == '.')
            dot_cnt ++;

        p ++;
    }
    return (bHasPunc || dot_cnt > 1);
}


/* �ж��Ƿ�Ϊurl��Ӣ�����ֱ��� */
bool life_service::IsLetterNumberPuncUrlWord(const char *c, int* cnt/* = NULL*/)
{
    //���ô˺�����ǰ����IsLetterNumberPuncWord,������ε����ж�
    //if (!IsLetterNumberPuncWord(c))
    //	return false;

    if (cnt != NULL)
        *cnt = 0;
    const char* p = c + 1;
    while(*p != 0)
    {
        //���������@����Ϊemail��ַ������Ϊurl
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

    //����ַ����������Ӵ���ͷ����Ϊ��url
    if ((len > 5 && strncmp(c, "http:",  5) == 0) ||
        (len > 4 && strncmp(c, "ftp:",   4) == 0) ||
        (len > 6 && strncmp(c, "https:", 6) == 0) ||
        (len > 4 && strncmp(c, "mms:",   4) == 0) ||
        (len > 4 && strncmp(c, "www.",   4) == 0)  )
        flag = true;
    //return true;

    //����ַ����������Ӵ���β����Ϊ��url
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


/* �ж��Ƿ�Ϊemail��Ӣ�����ֱ��� */
bool life_service::IsLetterNumberPuncEmailWord(const char *c, int* cnt/* = NULL*/)
{
    //���ô˺�����ǰ����IsLetterNumberPuncWord,������ε����ж�
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

    if ( !exist) //�����������@������Ϊemail��ַ
        return false;

    int len = strlen(c);		

    //����ַ����������Ӵ���β����Ϊ��email
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


/* Ӣ�Ĵ�дתСд */
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
