#pragma once
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <direct.h>
#else
#include <sys/time.h>
#define sprintf_s snprintf
#include <libgen.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#define __max(a,b) (((a) > (b)) ? (a) : (b))
#define __min(a,b) (((a) < (b)) ? (a) : (b))
typedef unsigned int UINT;
#endif
#include <time.h>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <set>
#include <stdexcept>
#include <iostream>
#include <algorithm>


#ifndef _DOUBLE_ERROR
#define _DOUBLE_ERROR 1E-10
#endif

class ToolGen {
public:
	static int msc2time(int mscx) {
		return ((mscx / 3600000 * 10000000) + ((mscx % 3600000) / 60000 * 100000) + (mscx % 60000));
	}
	static int time2msc(int timex) {
		return ((timex / 10000000 * 3600000) + ((timex % 10000000) / 100000 * 60000) + (timex % 100000));
	}
	static int msc2tms(int mscx) {
		int tmp = mscx - 34200000;
		if (tmp <= 7200000) {
			return tmp;
		} else if (tmp <= 12600000) {
			return 7200000;
		} else {
			return (tmp - 5400000);
		}
	}
	static int tms2msc(int tmsx) {
		return tmsx + (tmsx < 7200000 ? 34200000 : 39600000);
	}

	static int currentMsc() {
#ifdef _WIN32
		SYSTEMTIME st;
		GetLocalTime(&st);
		return st.wHour * 3600000 + st.wMinute * 60000 + st.wSecond * 1000 + st.wMilliseconds;
#else
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return (tv.tv_sec + 28800) % 86400 * 1000 + (tv.tv_usec / 1000);
#endif
	}

	static int currentTime() {
#ifdef _WIN32
		SYSTEMTIME st;
		GetLocalTime(&st);
		return (st.wHour * 10000000 + st.wMinute * 100000 + st.wSecond * 1000 + st.wMilliseconds);
#else
		return msc2time(currentMsc());
#endif
	}

	static time_t datetime2Timet(int date, int time)
	{
		struct tm t = { 0 };
		t.tm_year = date / 10000 - 1900;
		t.tm_mon = (date % 10000) / 100 - 1;
		t.tm_mday = (date % 100);
		t.tm_hour = time / 10000000;
		t.tm_min = (time % 10000000) / 100000;
		t.tm_sec = (time % 100000) / 1000;
		
		return mktime(&t);
	}

	static int currentDate() {
#ifdef _WIN32
		SYSTEMTIME st;
		GetLocalTime(&st);
		return (st.wYear * 10000 + st.wMonth * 100 + st.wDay);
#else
		struct tm *ptm;
		time_t t_time;
		t_time = time(NULL);
		ptm = localtime(&t_time);
		return ((ptm->tm_year + 1900) * 10000) + ((ptm->tm_mon + 1) * 100) + ptm->tm_mday;
#endif
	}

	static int currentWday() {
#ifdef _WIN32
		SYSTEMTIME st;
		GetLocalTime(&st);
		return st.wDayOfWeek;
#else
		struct tm *ptm;
		time_t t_time;
		t_time = time(NULL);
		ptm = localtime(&t_time);
		return ptm->tm_wday;
#endif
	}

	static int date2wday(int date) {
		//@date: YYYYMMDD
		//return: 0-6, sunday, mon,tue,...,sat
		int yy = date / 10000;
		int mm = date % 10000 / 100;
		int dd = date % 100;
		if (mm == 1 || mm == 2) {
			mm += 12;
			yy--;
		}
		return (dd + 2 * mm + (3 * mm + 3) / 5 + yy + yy / 4 - yy / 100 + yy / 400 + 1) % 7;
	}

	static void f_msSleep(int milSec) {
#ifdef _WIN32
		Sleep(milSec);
#else
		usleep(milSec * 1000);
#endif
	}

	static int f_typeLI(int curQty, int trdQty) {
		if (trdQty == 0) {
			return 0;
		} else if ((curQty * trdQty) >= 0) {
			return 3;
		} else if ((curQty * (curQty + trdQty)) >= 0) {
			return 2;
		} else {
			return 1;
		}
	}

	static double f_cf(double x, double cap, double floor) {
		return(x > cap ? (cap) : (x < floor ? floor : x));
	}

	static double f_approx(const double* x, const double* y, const int len, const double xout) {//len>=2
		int i;
		if (xout <= *x) return(*y);
		for (i = 1; i < len; i++) {
			if (xout <= *(x + i))
				return(((xout - *(x + i - 1)) * *(y + i) + (*(x + i) - xout) * *(y + i - 1)) / (*(x + i) - *(x + i - 1)));
		}
		return *(y + len - 1);
	}

	static int f_sign(int x) {
		return x == 0 ? 0 : (x > 0 ? 1 : -1);
	}

	static std::vector<std::string> f_dir(std::string dirx, bool fullName) {
		std::vector<std::string> fileList;
#ifdef _WIN32
		long hFile = 0;
		_finddata_t fileInfo;
		if ((hFile = _findfirst((dirx + "\\*").c_str(), &fileInfo)) == -1) {
			return fileList;
		}
		std::string tmp;
		do {
			tmp = fileInfo.name;
			if (tmp.compare("..") == 0 || tmp.compare(".") == 0) continue;
			if (fullName) {
				fileList.push_back(dirx + "/" + tmp);
			} else {
				fileList.push_back(tmp);
			}
		} while (_findnext(hFile, &fileInfo) == 0);
		_findclose(hFile);

#else
		DIR * pdir;
		struct dirent * ptr;
		if ((pdir = opendir(dirx.c_str())) == NULL) {
			return fileList;
		}
		while ((ptr = readdir(pdir)) != NULL) {
			std::string filex(ptr->d_name);
			struct stat sBuf;
			stat((dirx + "/" + filex).c_str(), &sBuf);
			if (filex.compare(".") == 0 || filex.compare("..") == 0) {
				continue;
			} else if (S_ISREG(sBuf.st_mode)) {
				if (fullName) {
					fileList.push_back(dirx + "/" + filex);
				} else {
					fileList.push_back(filex);
				}
			} else { 
				continue;
			}
		}
		closedir(pdir);
#endif
		return fileList;
	}

	static std::string f_gsub(const std::string & pattern, const std::string & to, const std::string & x) {
		std::string res = x;
		size_t ind = res.find(pattern);
		while (ind != std::string::npos) {
			res.replace(ind, pattern.size(), to);
			ind = res.find(pattern);
		}
		return res;
	}

	static void f_strsplit(const std::string& s, const std::string& delim, std::vector<std::string>& res) {
		res.clear();
		size_t indPre = 0;
		size_t ind = s.find_first_of(delim, indPre);
		while (ind != std::string::npos) {
			res.push_back(s.substr(indPre, ind - indPre));
			indPre = ind + delim.size();
			ind = s.find_first_of(delim, indPre);
		}
		if (ind - indPre > 0)
			res.push_back(s.substr(indPre, ind - indPre));
	}

	static void f_strsplit(const char* s, const char* delim, std::vector<std::string>& res) {
		std::string ss(s);
		std::string dd(delim);
		f_strsplit(ss, dd, res);
	}

	static void f_lcr(const std::vector<std::string>& xvec, double pos, std::vector<std::string>& res) {
		unsigned int ncharMax = 0;
		for (const auto& xx : xvec) {
			if (xx.size() > ncharMax) ncharMax = xx.size();
		}
		res.clear();
		for (const auto& xx : xvec) {
			std::string resx(ncharMax, ' ');
			resx.replace((int)((double)(ncharMax - xx.size()) * pos), xx.size(), xx);
			res.push_back(resx);
		}
	}

	static std::string f_title(const std::vector<std::string>& content, char symbol) {
		std::vector<std::string> cntLcr;
		f_lcr(content, 0.5, cntLcr);
		std::stringstream ss;
		std::string tmp1(cntLcr[0].size() + 6, symbol);
		ss << tmp1 << '\n';
		for (const auto& cntx : cntLcr) {
			ss << symbol << symbol << " " << cntx << " " << symbol << symbol << '\n';
		}
		ss << tmp1 << '\n';
		return ss.str();
	}

	static std::string f_title(const std::string& content, char symbol) {
		std::vector<std::string> cnt;
		cnt.push_back(content);
		return f_title(cnt, symbol);
	}

	static int f_readConfig(const std::string& fileName, std::map<std::string, std::string>& res) {//0 = success; -1, cannot find file; -2, illegal format
		std::fstream fin(fileName, std::fstream::in);
		if (!fin.is_open()) {
			return -1;
		}
		std::string tmpLine;
		std::vector<std::string> tmpSplit;
		while (getline(fin, tmpLine)) {
			if (*(tmpLine.c_str()) == '#' || tmpLine.size() == 0) continue;
			f_strsplit(tmpLine, "=", tmpSplit);
			if (tmpSplit.size() != 2) {
				return -2;
			}
			res.emplace(tmpSplit[0], tmpSplit[1]);
		}
		fin.close();
		return 0;
	}

	static std::string f_catCall(const std::string& call, char symbol) {
		int date = currentDate();
		int time = currentTime();
		char datetime[24];
		sprintf_s(datetime, 24, "%04d-%02d-%02d %02d:%02d:%02d.%03d", date / 10000, date % 10000 / 100, date % 100, time / 10000000, time / 100000 % 100, time / 1000 % 100, time % 1000);
		std::string dt(datetime);
		std::vector<std::string> callVec = { call, dt };
		return f_title(callVec, symbol);
	}

	static std::string f_int2string(int num) {
		std::stringstream res;
		res << num;
		return res.str();
	};

	static std::string f_dirname(const std::string & fullName) {
		std::string res;
#ifdef _WIN32
		size_t tmpInd = fullName.find_last_of("\\");
		if (tmpInd != std::string::npos) {
			res = fullName.substr(0, tmpInd);
		} else {
			res = ".\\";
		}
#else
		char * pChar = new char[fullName.length() + 1];
		sprintf(pChar, "%s", fullName.c_str());
		res = dirname(pChar);
		delete pChar;
#endif
		return res;
	}

	static void f_mkdir(const std::string & dirName) {
#ifdef _WIN32
		_mkdir(dirName.c_str());
#else
		system(("mkdir -p " + dirName).c_str());
#endif
	}

	static int f_capByTradableQty(int sendQty, int tradableQty, int lotSize = 100) {
		if (sendQty >= 0) {
			return sendQty / lotSize * lotSize;
		} else if (tradableQty + sendQty <= 0) {
			return -tradableQty;
		} else {
			return (sendQty / lotSize * lotSize) - ((-sendQty % lotSize) >= (tradableQty % lotSize) ? (tradableQty % lotSize) : 0);
		}
	}

	static std::string f_formatDate(int date, std::string format = "%Y-%m-%d") {
		std::string dateStr = f_int2string(date);
		if (dateStr.size() != 8) return "";
		return f_gsub("%d", dateStr.substr(6, 2), f_gsub("%m", dateStr.substr(4, 2), f_gsub("%Y", dateStr.substr(0, 4), format)));
	}

	static int f_activeMonIF(int curDate, int shiftWday = 4) {
		int curMday = curDate % 100;
		int firstNewDay = (curMday + shiftWday - date2wday(curDate)) % 7 + 14;
		int actMon = (curDate % 1000000) / 100;
		if (curMday >= firstNewDay) {//contract of next month
			if ((curDate % 10000) / 100 == 12) {
				actMon += 89;
			} else {
				actMon += 1;
			}
		}
		return actMon;
	}

	static std::vector<int> f_subDATEX(std::string file, int n, int endDate = 20991231, int startDate = 20100101, std::string format = "%Y-%m-%d") {
		file = f_gsub("//", "/", file);
		std::vector<int> res;
		std::set<std::string> fileSet = [](std::vector<std::string> fileList) {
			std::set<std::string> fileSet;
			for (const std::string& filex : fileList) fileSet.emplace(filex);
			return fileSet;
		}(f_dir(f_dirname(file), true));
		for (int datex = endDate; datex >= startDate && res.size() < n; datex--) {
			int dd = datex % 100;
			int mm = datex % 10000 / 100;
			if (dd < 1 || dd>31 || mm < 1 || mm>12) continue;
			if (fileSet.find(f_gsub("DATEX", f_formatDate(datex, format), file)) != fileSet.end())
				res.push_back(datex);
		}
		return res;
	}

	static std::string f_double2string(double x, std::string format) {
		char str[1024];
		sprintf_s(str, 1024, format.c_str(), x);
		return str;
	}
	static bool f_string2bool(std::string x) {
		std::transform(x.begin(), x.end(), x.begin(), ::tolower);
		std::istringstream is(x);
		bool b;
		is >> std::boolalpha >> b;
		return b;
	}
	template <class T> static std::string f_temp2string(T x) {
		std::stringstream res;
		res << x;
		return res.str();
	}
	static std::string f_getEnv(const std::string& name) {
		const char* pchar = getenv(name.c_str());
		if (pchar == nullptr) throw std::runtime_error("undefined env " + name);
		std::string res(pchar);
		if (res.find((char)13) != std::string::npos) throw std::runtime_error("Carriage Return found in ENV " + name + "; please run unix2dos");
		else return res;
	};
	static int f_signDouble(const double& x) {
		if (x < -_DOUBLE_ERROR) return -1;
		else if (x > _DOUBLE_ERROR) return 1;
		else return 0;
	}
	static void f_matrixMultiply(double* result, const double* left, const double* right, int nrow_left, int ncol_left, int ncol_right) {
		for (int rowx = 0; rowx < nrow_left; ++rowx) {
			for (int colx = 0; colx < ncol_right; ++colx) {
				result[rowx * ncol_right + colx] = 0;
				for (int jx = 0; jx < ncol_left; ++jx)
					result[rowx * ncol_right + colx] += left[rowx*ncol_left + jx] * right[jx*ncol_right + colx];
			}
		}
	}
	static void f_colDeweight(double* x, int nrow, int ncol) {
		for (int colx = 0; colx < ncol; ++colx) {
			double colSumP = 0, colSumN = 0;
			for (int rowx = 0; rowx < nrow; ++rowx) {
				double xp = x[rowx * ncol + colx];
				if (xp > _DOUBLE_ERROR) colSumP += xp;
				else if (xp < -_DOUBLE_ERROR) colSumN -= xp;
			}
			for (int rowx = 0; rowx < nrow; ++rowx) {
				double* xp = x + rowx * ncol + colx;
				if (*xp > _DOUBLE_ERROR) *xp /= colSumP;
				if (*xp < -_DOUBLE_ERROR) *xp /= colSumN;
			}
		}
	}
};
