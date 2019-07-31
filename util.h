// ������ͷ�ļ�

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

//================================[���뷶Χ�ж�]=======================================//
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


/* �ж��Ƿ�Ϊ�ַ� */
inline bool IsChar(const char *c)
{
    return (*c > 0);
}


/* �ж��Ƿ�Ϊ�Ϸ���ĸ */
inline bool IsLetter(const char *c)
{
    return (((*c) >= 'a' && (*c) <= 'z') || ((*c) >= 'A' && (*c) <= 'Z') || ((*c) == '\''));
}


/* �ж��Ƿ�Ϊ�Ϸ����� */
inline bool IsNumber(const char *c)
{
    return (((*c) >= '0' && (*c) <= '9') || (*c) == '.');
}

/* �ж��Ƿ�Ϊ�Ϸ������� */
inline bool IsNumberExact(const char *c)
{
    return (*c >= '0' && *c <= '9');
}

/* �ж��Ƿ�Ϊ�Ϸ��ַ�[�Ϸ���ĸ+�Ϸ�����+�Ϸ������ַ�] */
inline bool IsValidChar(const char *c)
{
    //return (IsLetter(c) || IsNumber(c) || IsReservedChar(c));
    return (*c > 32);
}

/* �ж��Ƿ�Ϊ���ַ� */
/* ���ô˺�����ǰ�����Ѿ�ΪIsPuncChar */
inline bool IsPuncConnChar(const char *c)
{
    return (*c == '-' || *c == '_' || *c == '.');
}


/* �ж��Ƿ�Ϊ�Ϸ������ַ�[������Ϊ�����ַ�] */
/* ����32�Ŀɼ��ַ�����ȥӢ������[����.] */
bool IsReservedChar(const char *c);


/* �ж��Ƿ�Ϊ����ַ� */
/* ���ô˺�����Ӧ��ȷ���ӵڶ����ַ���ʼ*/
bool IsPuncChar(const char *c);


//////////////////////////////////////////////////////////////////////////
// Chinese Word
/* �ж��Ƿ�Ϊ˫�ֽ��ַ� */
inline bool IsNotChar(const char *c)
{
    char c1 = (*c);
    char c2 = (*(c+1));

    return ((c1 < 0) && (c2 != 0));
}


/* �ж��Ƿ�ΪGBK˫�ֽ��ַ�[8140-FEFE] */
inline bool IsGBK(const char *c)
{
    uint8_t c1 = (uint8_t)(*c);
    uint8_t c2 = (uint8_t)(*(c+1));

    return ((c1 >= 0x81) && (c2 >= 0x40) && (c1 <= 0xFE) && (c2 <= 0xFE));
}


/* �ж��Ƿ�Ϊ����([B0A1-F7FE],[8140-A0FE],[AA40-FEA0])*/
inline bool IsChinese(const char *c)
{
    uint8_t c1 = (uint8_t)(*c);
    uint8_t c2 = (uint8_t)(*(c+1));

    return (((c1 >= 0xB0) && (c2 >= 0xA1) && (c1 <= 0xF7) && (c2 <= 0xFE)) ||
        ((c1 >= 0x81) && (c2 >= 0x40) && (c1 <= 0xA0) && (c2 <= 0xFE)) ||
        ((c1 >= 0xAA) && (c2 >= 0x40) && (c1 <= 0xFE) && (c2 <= 0xA0)));
}


/* �ж��Ƿ�Ϊ������˫�ֽ��ַ�����([A4A0 - A5FF]����Ƭ�����������š����������㩖)*/
bool IsReservedGBK(const char *c);


/* �ж��Ƿ�Ϊ�Ϸ���GBK˫�ֽ��ַ�[IsChinese��IsReservedGBK]*/
bool IsValidGBK(const char *c);

//================================[����ʷ�Χ�ж�]=====================================//
/*
word   -- IsValidWord -- IsValidCharWord 
          |          |-- IsValidChineseWord
          |          |-- IsReservedWord
          |-- IsIgnoreWord
*/

/* �ж��Ƿ�Ϊ�Ϸ����ַ��� */
bool IsValidCharWord(const char *c);


/* �ж��Ƿ�Ϊ�Ϸ������Ĵ� */
bool IsValidChineseWord(const char *c);

/*�ж��Ƿ�Ϊ�����Ĵ�*/
bool IsStrictGBKChar(const char* p);

/* �ж��Ƿ�Ϊ�Ϸ�������� */
bool IsReservedWord(const char *c);


/* �ж��Ƿ�Ϊ�Ϸ��Ĵ� */
bool IsValidWord(const char *c);


/* �ж��Ƿ�Ϊ���Ϸ��ɶ����Ĵ�[�����ո�] */
bool IsIgnoreWord(const char *c);


//================================[�����⴦����ж�]===================================//
/*
IsValidCharWord -- IsLetterNumberWord -- IsLetterNumberSpecialWord
          |                          |-- else
		  |-- IsLetterNumberPuncWord  -- IsLetterNumberPuncSpecialWord							 
                                     |-- IsLetterNumberPuncUrlWord 
                                     |-- else	  
*/

//�ж��Ƿ�Ϊ���ִ�
bool IsNumberWord(const char *);

//�ж��Ƿ�Ϊ�����ִ�
bool IsNumberExactWord(const char *c);


/* �ж��Ƿ�ΪӢ�����ִ� */
bool IsLetterNumberWord(const char *c);

/* �ж��Ƿ�ΪӢ�����ֱ��� */
bool IsLetterNumberPuncWord(const char *c);



/* �ж��Ƿ�Ϊurl��Ӣ�����ֱ��� */
bool IsLetterNumberPuncUrlWord(const char *c, int* cnt = NULL);


/* �ж��Ƿ�Ϊemail��Ӣ�����ֱ��� */
bool IsLetterNumberPuncEmailWord(const char *c, int* cnt = NULL);


/* Ӣ�Ĵ�дתСд */
char* TranToLower(char* p);

}
#endif
