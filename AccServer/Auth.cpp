#include "Auth.h"
#include <Windows.h>
#include <iostream>
#include <cassert>
#include <sstream>

std::string GetModuleFilePath()
{
	TCHAR szFile[MAX_PATH] = { 0 };
	DWORD dwRet = ::GetModuleFileName(NULL, szFile, 255);
	if (dwRet != 0)
	{
		std::string str = szFile;
		size_t nPos = str.rfind('\\');
		if (nPos != std::string::npos)
		{
			str = str.substr(0, nPos);
		}
		return str;
	}
	return "";
}

time_t str2time(const std::string &strTime)
{
	struct tm sTime;
	sscanf(strTime.c_str(), "%d-%d-%d %d:%d:%d", &sTime.tm_year, &sTime.tm_mon, &sTime.tm_mday, &sTime.tm_hour, &sTime.tm_min, &sTime.tm_sec);
	sTime.tm_year -= 1900;
	sTime.tm_mon -= 1;
	time_t ft = mktime(&sTime);
	return ft;
}
time_t str2time_utc(const std::string& strTime) {
	time_t t = str2time(strTime);
	return t + 8 * 60 * 60;
}

const std::string time2str(time_t *_Time) {
	char buffer[80];
	time_t now = time(_Time);
	struct tm * timeinfo = localtime(&now);
	strftime(buffer, 80, "%F %T", timeinfo);

	return std::string(buffer);
}
const std::string time2str_utc(time_t *_Time) {
	char buffer[80];
	time_t now = time(_Time);
	struct tm * timeinfo = gmtime(&now);
	strftime(buffer, 80, "%F %T", timeinfo);

	return std::string(buffer);
}

bool CheckValidTime(const std::string& valid_time) {

	time_t now = time(NULL);
	time_t valid = str2time_utc(valid_time);

	return valid > now;
}

static const char* CREATE_DEVICES = ""
"CREATE TABLE IF NOT EXISTS [devices]("
"[index] INTEGER  NOT NULL PRIMARY KEY AUTOINCREMENT,"
"[sn] VARCHAR(128)  UNIQUE NOT NULL,"
"[name] VARCHAR(64)  NULL,"
"[desc] VARCHAR(128)  NULL,"
"[create_time] TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,"
"[valid_time] TIMESTAMP  NULL"
")";

static const char* CREATE_USERS = ""
"CREATE TABLE IF NOT EXISTS [users]("
"[index] INTEGER  NOT NULL PRIMARY KEY AUTOINCREMENT,"
"[id] VARCHAR(32)  UNIQUE NOT NULL,"
"[name] VARCHAR(64)  NULL,"
"[desc] VARCHAR(128)  NULL,"
"[passwd] VARCHAR(128)  NOT NULL,"
"[email] TEXT  NULL,"
"[phone] TEXT  NULL,"
"[create_time] TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,"
"[level] INTEGER  NOT NULL,"
"[valid_time] TIMESTAMP  NULL"
")";

static const char* CREATE_PERMISSIONS = ""
"CREATE TABLE IF NOT EXISTS [permissions]("
"[index] INTEGER  NOT NULL PRIMARY KEY AUTOINCREMENT,"
"[uid] INTEGER  NOT NULL,"
"[devid] INTEGER  NOT NULL"
"[valid_time] TIMESTAMP  NULL"
")";


CAuth::CAuth() : m_pDB(nullptr)
{
}


CAuth::~CAuth()
{
}

int CAuth::Init()
{
	m_Lock.lock();
	std::string db_file = GetModuleFilePath() + "\\data.sqlite3.db";
	int rc = sqlite3_open(db_file.c_str(), &m_pDB);
	if (SQLITE_OK == rc) {
		rc = sqlite3_exec(m_pDB, CREATE_DEVICES, NULL, NULL, NULL);
		assert(rc == SQLITE_OK);
		rc = sqlite3_exec(m_pDB, CREATE_USERS, NULL, NULL, NULL);
		assert(rc == SQLITE_OK);
		rc = sqlite3_exec(m_pDB, CREATE_PERMISSIONS, NULL, NULL, NULL);
		assert(rc == SQLITE_OK);
	}
	else {
		std::cerr << "Failed open file:" << db_file << std::endl;
		std::cerr << "Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		sqlite3_close(m_pDB);
		m_pDB = nullptr;
	}
	m_Lock.unlock();

	return rc;
}

void CAuth::Close()
{
	if (!m_pDB)
		return;

	m_Lock.lock();
	sqlite3_close(m_pDB);
	m_pDB = nullptr;
	m_Lock.unlock();
}

int CAuth::IsValidMapper(const std::string & mapper_id)
{
	if (!m_pDB)
		return -AUTH_ERROR;

	m_Lock.lock();

	std::stringstream sql;
	sql << "select valid_time from devices where sn='" << mapper_id << "'";

	std::string valid_time;
	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		std::string* valid_time = (std::string*)data;
		if (vals[0] != NULL) {
			*valid_time = vals[0];
		}
		else {
			*valid_time = "2017-6-25 23:49:59";
		}
		return 0;
	}, &valid_time, NULL);

	m_Lock.unlock();
	if (valid_time.empty())
		return -AUTH_NO_EXITS;

	if (!CheckValidTime(valid_time))
		return -AUTH_TIMEOUT;

	return AUTH_OK;
}

int CAuth::Login(const std::string & id, const std::string & password)
{
	if (!m_pDB)
		return -AUTH_ERROR;

	m_Lock.lock();

	std::stringstream sql;
	sql << "select valid_time from users where id='" << id << "' and passwd='" << password << "'";

	std::string valid_time;
	int nRet = 0;
	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		std::string* valid_time = (std::string*)data;
		if (vals[0] != NULL) {
			*valid_time = vals[0];
		} 
		else {
			*valid_time = "2017-6-25 23:49:59";
		}
		return 0;
	}, &valid_time, NULL);

	m_Lock.unlock();
	if (valid_time.empty())
		return -AUTH_NO_EXITS;

	if (!CheckValidTime(valid_time))
		return -AUTH_TIMEOUT;

	return AUTH_OK;
}

int CAuth::Logout(const std::string & id)
{
	if (!m_pDB)
		return -AUTH_ERROR;
	return AUTH_OK;
}

int CAuth::Access(const std::string & id, const std::string & mapper_id)
{
	if (!m_pDB)
		return -AUTH_ERROR;

}

int CAuth::Allow(const std::string & id, const std::string & mapper_id, time_t* _Time = NULL)
{
	if (!m_pDB)
		return -AUTH_ERROR;

}

int CAuth::Deny(const std::string & id, const std::string & mapper_id)
{
	if (!m_pDB)
		return -AUTH_ERROR;

}

int CAuth::GetClientIndex(const std::string & id)
{
	if (!m_pDB)
		return 0xFFFFFFFF;

	m_Lock.lock();

	std::stringstream sql;
	sql << "select index from users where id='" << id << "'";

	std::string valid_time;
	int nID = -1;
	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		int* nID = (int*)data;
		if (vals[0] != NULL) {
			*nID = atoi(vals[0]);
		}
		else {
			*nID = 0xFFFFFFFF;
		}
		return 0;
	}, &nID, NULL);

	m_Lock.unlock();
	return nID;
}

int CAuth::GetMapperIndex(const std::string & id)
{
	if (!m_pDB)
		return 0xFFFFFFFF;

	m_Lock.lock();

	std::stringstream sql;
	sql << "select index from devices where id='" << id << "'";

	std::string valid_time;
	int nID = -1;
	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		int* nID = (int*)data;
		if (vals[0] != NULL) {
			*nID = atoi(vals[0]);
		}
		else {
			*nID = 0xFFFFFFFF;
		}
		return 0;
	}, &nID, NULL);

	m_Lock.unlock();
	return nID;
}

int CAuth::_Access(int user, int dev)
{
	m_Lock.lock();

	std::stringstream sql;
	sql << "select valid_time from permissions where uid=" << user << " and devid=" << dev;

	std::string valid_time;
	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		std::string* valid_time = (std::string*)data;
		if (vals[0] != NULL) {
			*valid_time = vals[0];
		}
		else {
			*valid_time = "2017-6-25 23:49:59";
		}
		return 0;
	}, &valid_time, NULL);

	m_Lock.unlock();
	if (valid_time.empty())
		return -AUTH_NO_EXITS;

	if (!CheckValidTime(valid_time))
		return -AUTH_TIMEOUT;

	return AUTH_OK;
}

int CAuth::_Allow(int user, int dev, time_t * _Time)
{
	int rc = _Access(user, dev);
	if (rc != -AUTH_NO_EXITS) {
		rc = _Deny(user, dev);
	}

	m_Lock.lock();

	std::stringstream sql;
	if (_Time == NULL) {
		sql << "insert into valid_time (uid, devid) values(" << user << ", " << dev << ")";
	}
	else {
		sql << "insert into valid_time (uid, devid, valid_time) values(";
		sql << user << ", " << dev << ", " << time2str_utc(_Time) << ")";
	}

	std::string valid_time;
	rc = sqlite3_exec(m_pDB, sql.str().c_str(), NULL, NULL, NULL);
	if (rc != SQLITE_OK)
		return -AUTH_ERROR;

	return AUTH_OK;
}

int CAuth::_Deny(int user, int dev)
{
	m_Lock.lock();

	std::stringstream sql;
	sql << "delete from permissions where uid=" << user << " and devid=" << dev;

	std::string valid_time;
	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), NULL, NULL, NULL);
	if (rc != SQLITE_OK)
		return -AUTH_ERROR;

	return AUTH_OK;
}
