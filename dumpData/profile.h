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

   // 描述  : 从str中将左右空格和'\t'截取掉
   // 返回  : char * 
   // 参数  : char *str
   static char * LRTrim(char *str);
	//打开配置文件
	bool Open(const char *filename);
	const VINI_ENTRY* GetSection(const char* section);
	//读取一个int型entry的值
	int ReadInt( const char *section, const char *entry, int defaultInt);
	//读取一个string型的entry的值
	int ReadString( const char *section, const char *entry,const char *defaultString, char *buffer,int bufLen);
	//写一个string型的entry到指定的section中
	//写一个int型的entry到指定的section中
	//关闭配置文件
	void Close();

   TIniFile();
	~TIniFile();
private:
	char * percolate(char *str);
};


#endif
