#pragma once
#ifdef _WIN32
#include <stdlib.h>
#else
#define __max(a,b) (((a) > (b)) ? (a) : (b))
#define __min(a,b) (((a) < (b)) ? (a) : (b))
#endif // _WIN32
#ifdef _WIN32
#	pragma warning(disable: 4251)
#	pragma warning(disable: 4996)
#endif
#include "AtgoLogApi.h"
extern AtgoLogApi* g_Log;

#define GLOG_DEBUG(...) \
{\
	if (g_Log != nullptr) \
	{\
		char szBuffer[2048] = {0};\
		snprintf(szBuffer, sizeof(szBuffer),  __VA_ARGS__);\
		g_Log->onDebug(__FILE__, __LINE__, szBuffer);\
	}\
}

#define GLOG_WARN(...) \
{\
	if (g_Log != nullptr) \
	{\
		char szBuffer[2048] = {0};\
		snprintf(szBuffer, sizeof(szBuffer),  __VA_ARGS__);\
		g_Log->onWarning(__FILE__, __LINE__, szBuffer);\
	}\
}

#define GLOG_ERROR(...) \
{\
	if (g_Log != nullptr) \
	{\
		char szBuffer[2048] = {0};\
		snprintf(szBuffer, sizeof(szBuffer),  __VA_ARGS__);\
		g_Log->onError(__FILE__, __LINE__, szBuffer);\
	}\
}