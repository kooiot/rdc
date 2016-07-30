
// stdafx.cpp : 只包括标准包含文件的源文件
// ServerCP.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"



time_t str2time(const std::string &strTime)
{
	if (strTime.empty())
		return 0;
	struct tm sTime;
	sscanf(strTime.c_str(), "%d-%d-%d %d:%d:%d", &sTime.tm_year, &sTime.tm_mon, &sTime.tm_mday, &sTime.tm_hour, &sTime.tm_min, &sTime.tm_sec);
	sTime.tm_year -= 1900;
	sTime.tm_mon -= 1;
	time_t ft = mktime(&sTime);
	return ft;
}
//time_t str2time_utc(const std::string& strTime) {
//	time_t t = str2time(strTime);
//	return t + 8 * 60 * 60;
//}

const std::string time2str(const time_t *_Time) {
	char buffer[80];
	time_t now = _Time != NULL ? *_Time : time(NULL);

	struct tm * timeinfo = localtime(&now);
	strftime(buffer, 80, "%F %T", timeinfo);

	return std::string(buffer);
}
//const std::string time2str_utc(const time_t *_Time) {
//	char buffer[80];
//	time_t now = _Time != NULL ? *_Time : time(NULL);
//
//	struct tm * timeinfo = gmtime(&now);
//	strftime(buffer, 80, "%F %T", timeinfo);
//
//	return std::string(buffer);
//}
