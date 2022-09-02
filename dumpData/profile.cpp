#include <ctype.h>
#include "profile.h"

static FUNC_INI2LOG pFuncIni2Log = NULL;


void SetINI2LOGFunc(FUNC_INI2LOG func)
{
	pFuncIni2Log = func;
}

TIniFile::TIniFile()
{
	m_sections.clear();
}

TIniFile::~TIniFile()
{
	Close();
}
void TIniFile::Close()
{
	m_sections.clear();
}

bool TIniFile::Open(const char *filename)
{
	Close();

	FILE *fin;
	fin = fopen(filename, "rt");
	if (fin == NULL)
		return false;
	bool rtn = true;
	char tmp[1024];
	INI_SECTION sect;
	INI_ENTRY entry;
	bool insect = false;
	int nline = 0;
	while (rtn)
	{
		if (!fgets(tmp, 1023, fin))
			break;
		++nline;
		tmp[1023] = '\0';
		char *pc = percolate(tmp);
		int i;
		for (i = 0; pc[i] != '='; i++)
		{
			if (pc[i] == '\0')
				break;
		}
		if (pc[i] != '\0')
		{
			if (insect)
			{
				pc[i] = '\0'; ++i;
				char *ent = percolate(pc);
				if (ent[0] != '\0')
				{
					// ENTRY=DATA
					entry.entry = ent;  // ENTRY
					entry.data = pc + i;  // DATA
					sect.ventries.push_back(entry);
				}
				else
				{
					printf("INI READER: INVALID LINE @%d (=%s)\n", nline, pc + i);
					rtn = false;

				}
			}
			else
			{
				printf("INI READER: INVALID LINE @%d (Entry not in SECTION)\n", nline);
				rtn = false;
			}
		}
		else if (pc[0] == '[')
		{
			for (i = 1; pc[i] != '\0'; i++)
			{
				if (pc[i] == ']') break;
			}
			if (pc[i] == ']')
			{
				if (insect && sect.ventries.size() > 0)
				{
					m_sections.push_back(sect);
					sect.ventries.clear();
				}
				pc[i] = '\0';
				sect.section = pc + 1;   // SECTION : [xxxx]
				insect = true;
			}
			else
			{
				printf("INI READER: INVALID LINE @%d (Invalid SECTION)\n", nline);
				rtn = false;
			}
		}
	}
	if (insect && sect.ventries.size() > 0)
	{
		m_sections.push_back(sect);
		sect.ventries.clear();
	}
	fclose(fin);
	return(rtn);
}

const VINI_ENTRY* TIniFile::GetSection(const char * section)
{
	VINI_SECTION::iterator vsit;
	for (vsit = m_sections.begin(); vsit != m_sections.end(); vsit++)
	{
		if (vsit->section.compare(section) == 0)
		{
			return &vsit->ventries;
		}
	}
	return NULL;
}

int TIniFile::ReadString(const char *section, const char *entry, const char *defaultString, char *buffer, int bufLen)
{
	VINI_SECTION::iterator vsit;
	VINI_ENTRY::iterator veit;
	for (vsit = m_sections.begin(); vsit != m_sections.end(); vsit++)
	{
		if (vsit->section.compare(section) == 0)
		{
			for (veit = vsit->ventries.begin(); veit != vsit->ventries.end(); veit++)
			{
				if (veit->entry.compare(entry) == 0)
				{
                    if (veit->data.empty())
                    {
                        // 如果key找到了，但是没值就用默认字符串返回
                        strncpy(buffer, defaultString, bufLen - 1);
                    }
                    else
                    {
                        strncpy(buffer, veit->data.c_str(), bufLen - 1);
                    }
					buffer[bufLen - 1] = 0;

					if (pFuncIni2Log != NULL)
					{
						(*pFuncIni2Log)("INI:[%s] %s=%s", section, entry, buffer);
					}
                    return 0;
				}
			}
		}
	}
	strncpy(buffer, defaultString, bufLen - 1);
	buffer[bufLen - 1] = 0;

	if (pFuncIni2Log != NULL)
	{
		(*pFuncIni2Log)("INI:[%s] %s=%s", section, entry, buffer);
	}
	return -1;
}

int TIniFile::ReadInt(const char *section, const char *entry, int defaultInt)
{
    char buf[1024] = { 0 };
    char buf2[12] = { 0 };
	sprintf(buf2, "%d", defaultInt);  // -2100000000
    ReadString(section, entry, buf2, buf, sizeof(buf));
	defaultInt = atoi(buf);
	return defaultInt;
}

char * TIniFile::percolate(char *str)
{
	int i;
	char *pc;
	bool bcommentsign = true; // "#", ";"为注释符，只有在开头时有效
	for (i = 0; str[i] != 0; i++)
	{
		if (str[i] == '/' && str[i + 1] == '/')
		{
			str[i] = 0;
			break;
		}
		else
		{
			if (bcommentsign)
			{
				if (str[i] == '#' || str[i] == ';')
				{
					str[i] = 0;
					break;
				}
				else if (str[i] != ' ' && str[i] != '\t')
				{
					bcommentsign = false;
				}
			}
		}
	}
	for (i--; i >= 0; i--)
	{
		if (str[i] <= 0x20)
			str[i] = 0;
		else
			break;
	}
	for (pc = str; *pc != 0; pc++)
	{
		if (*pc > 0x20)
			break;
	}
	return pc;
}

char * TIniFile::LRTrim(char *str)
{
	char * tmp;
	char * tmp2;
	if (str == NULL)
	{
		return NULL;
	}
	tmp = str;
	while (*tmp)	tmp++;
	if (*str) tmp--;
	while (*tmp == ' ' || *tmp == '\t') tmp--;
	*(tmp + 1) = 0;
	tmp = str;
	tmp2 = str;
	while (*tmp2 == ' ' || *tmp2 == '\t') tmp2++;
	while (*tmp2 != 0) {
		*tmp = *tmp2; tmp++; tmp2++;
	}
	*tmp = 0;
	return str;
}

char * TIniFile::ReadTString(const char *section, const char *entry, const char *defaultString, char *buffer, int bufLen)
{
	ReadString(section, entry, defaultString, buffer, bufLen);
	return (LRTrim(buffer));
}
