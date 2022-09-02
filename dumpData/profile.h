#ifndef __PROFILE_H
#define __PROFILE_H

typedef void (*FUNC_INI2LOG)(const char *fmt,...);
#include <string>
#include <vector>
#include <string.h>

typedef struct {
   std::string entry;
   std::string data;
} INI_ENTRY;

typedef std::vector<INI_ENTRY> VINI_ENTRY;
typedef struct 
{
   std::string section;
   VINI_ENTRY  ventries;
} INI_SECTION;
typedef std::vector<INI_SECTION> VINI_SECTION;



void SetINI2LOGFunc(FUNC_INI2LOG func);

class TIniFile
{
   VINI_SECTION m_sections;

public:

   char * ReadTString(const char *section,const char *entry,const char *defaultString,char *buffer,int bufLen);

   // ����  : ��str�н����ҿո��'\t'��ȡ��
   // ����  : char * 
   // ����  : char *str
   static char * LRTrim(char *str);
	//�������ļ�
	bool Open(const char *filename);
	const VINI_ENTRY* GetSection(const char* section);
	//��ȡһ��int��entry��ֵ
	int ReadInt( const char *section, const char *entry, int defaultInt);
	//��ȡһ��string�͵�entry��ֵ
	int ReadString( const char *section, const char *entry,const char *defaultString, char *buffer,int bufLen);
	//дһ��string�͵�entry��ָ����section��
	//дһ��int�͵�entry��ָ����section��
	//�ر������ļ�
	void Close();

   TIniFile();
	~TIniFile();
private:
	char * percolate(char *str);
};


#endif
