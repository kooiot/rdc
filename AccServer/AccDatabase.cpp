#ifndef RDC_LINUX_SYS
#include <Windows.h>
#endif
#include <iostream>
#include <cassert>
#include <sstream>
#include <cstring>
#include "AccDatabase.h"

#include <DataDefs.h>

#ifdef RDC_LINUX_SYS
#define MAX_PATH 512
int GetModuleFileName( char* sModuleName, char* sFileName, int nSize)
{
	int ret = 0;
	char* p = getenv("_");
	if (p != NULL)
	{
		if (sModuleName != NULL)
			strstr( p, sModuleName );
		if (p != NULL)
			strcpy( sFileName, p );
		ret = strlen(sFileName);
	}
	return ret;
}
#endif

std::string GetModuleFilePath()
{
	char szFile[MAX_PATH] = { 0 };
	int dwRet = ::GetModuleFileName(NULL, szFile, 255);
	if (dwRet != 0)
	{
		printf("Module File Name: %s \n", szFile);
		std::string str = szFile;
#ifndef RDC_LINUX_SYS
		size_t nPos = str.rfind('\\');
#else
		size_t nPos = str.rfind('/');
#endif
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

const std::string time2str(const time_t *_Time = NULL) {
	char buffer[80];
	time_t now = _Time != NULL ? *_Time : time(NULL);

	struct tm * timeinfo = localtime(&now);
	strftime(buffer, 80, "%F %T", timeinfo);

	return std::string(buffer);
}
//const std::string time2str_utc(const time_t *_Time = NULL) {
//	char buffer[80];
//	time_t now = _Time != NULL ? *_Time : time(NULL);
//
//	struct tm * timeinfo = gmtime(&now);
//	strftime(buffer, 80, "%F %T", timeinfo);
//
//	return std::string(buffer);
//}

bool CheckValidTime(const std::string& valid_time) {

	time_t now = time(NULL);
	time_t valid = str2time(valid_time);

	return valid > now;
}

inline std::string SQL2STD_STRING(const char* sql) {
	return std::string(sql != NULL ? sql : "");
}


DbUserInfo::DbUserInfo() : Index(INVALID_INDEX), Level(1)
{
}

void DbUserInfo::ToUserInfo(UserInfo & info)
{
	info.Index = Index;
	info.Level = Level;
	info.Group = Group;
	memcpy(info.ID, ID.c_str(), RC_MAX_ID_LEN);
	memcpy(info.Name, Name.c_str(), RC_MAX_NAME_LEN);
	memcpy(info.Desc, Desc.c_str(), RC_MAX_DESC_LEN);
	memcpy(info.Passwd, "****", RC_MAX_PASSWD_LEN);
	memcpy(info.Email, Email.c_str(), RC_MAX_EMAIL_LEN);
	memcpy(info.Phone, Phone.c_str(), RC_MAX_PHONE_LEN);
	info.CreateTime = CreateTime;
	info.ValidTime = ValidTime;
}

std::string ConverCharToString(const char* str, int max_size)
{
	char temp[256];
	snprintf(temp, max_size + 1, "%s", str);
	return std::string(temp);
}
void DbUserInfo::FromUserInfo(const UserInfo & info)
{
	Index = info.Index;
	Level = info.Level;
	Group = info.Group;
	ID = ConverCharToString(info.ID, RC_MAX_ID_LEN);
	Name = ConverCharToString(info.Name, RC_MAX_NAME_LEN);
	Desc = ConverCharToString(info.Desc, RC_MAX_DESC_LEN);
	Email = ConverCharToString(info.Email, RC_MAX_EMAIL_LEN);
	Phone = ConverCharToString(info.Phone, RC_MAX_PHONE_LEN);
	CreateTime = info.CreateTime;
	ValidTime = info.ValidTime;
}

DbDeviceInfo::DbDeviceInfo() : Index(INVALID_INDEX)
{
}

void DbDeviceInfo::ToDeviceInfo(DeviceInfo & info)
{
	info.Index = Index;
	memcpy(info.SN, SN.c_str(), RC_MAX_SN_LEN);
	memcpy(info.Name, Name.c_str(), RC_MAX_NAME_LEN);
	memcpy(info.Desc, Desc.c_str(), RC_MAX_DESC_LEN);
	info.CreateTime = CreateTime;
	info.ValidTime = ValidTime;
}

void DbDeviceInfo::FromDeviceInfo(const DeviceInfo & info)
{
	Index = info.Index;
	SN = ConverCharToString(info.SN, RC_MAX_SN_LEN);
	Name = ConverCharToString(info.Name, RC_MAX_NAME_LEN);
	Desc = ConverCharToString(info.Desc, RC_MAX_DESC_LEN);
	CreateTime = info.CreateTime;
	ValidTime = info.ValidTime;
}

DbGroupInfo::DbGroupInfo() : Index(INVALID_INDEX)
{
}

void DbGroupInfo::ToGroupInfo(GroupInfo & info)
{
	info.Index = Index;
	memcpy(info.Name, Name.c_str(), RC_MAX_NAME_LEN);
	memcpy(info.Desc, Desc.c_str(), RC_MAX_DESC_LEN);
	info.CreateTime = CreateTime;
	info.ValidTime = ValidTime;
}

void DbGroupInfo::FromGroupInfo(const GroupInfo & info)
{
	Index = info.Index;
	Name = ConverCharToString(info.Name, RC_MAX_NAME_LEN);
	Desc = ConverCharToString(info.Desc, RC_MAX_DESC_LEN);
	CreateTime = info.CreateTime;
	ValidTime = info.ValidTime;
}


static const char* CREATE_GROUPS = ""
"CREATE TABLE IF NOT EXISTS [groups]("
"[index] INTEGER  NOT NULL PRIMARY KEY AUTOINCREMENT,"
"[name] VARCHAR(64)  UNIQUE NOT NULL,"
"[desc] VARCHAR(128)  NULL,"
"[create_time] TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,"
"[valid_time] TIMESTAMP  NULL"
")";

static const char* CREATE_GROUP_DEVICES = ""
"CREATE TABLE IF NOT EXISTS [group_devices]("
"[index] INTEGER  NOT NULL PRIMARY KEY AUTOINCREMENT,"
"[grpid] INTEGER  NOT NULL,"
"[devid] INTEGER  NOT NULL"
")";

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
"[group] INTEGER  DEFAULT '-1' NOT NULL,"
"[valid_time] TIMESTAMP  NULL"
")";

static const char* CREATE_ADMIN = ""
"INSERT INTO [users] (id,name,desc,passwd,level) VALUES('admin','管理员','系统管理员','admin',99)";

CAccDatabase::CAccDatabase() : m_pDB(nullptr)
{
}


CAccDatabase::~CAccDatabase()
{
}

int CAccDatabase::Init()
{
	m_Lock.lock();
#ifndef RDC_LINUX_SYS
	std::string db_file = GetModuleFilePath() + "\\data.sqlite3.db";
#else
	std::string db_file = GetModuleFilePath() + "/data.sqlite3.db";
#endif
	int rc = sqlite3_open(db_file.c_str(), &m_pDB);
	if (SQLITE_OK == rc) {
		rc = sqlite3_exec(m_pDB, CREATE_DEVICES, NULL, NULL, NULL);
		if (rc != SQLITE_OK)
			std::cerr << "CREATE_DEVICES Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		assert(rc == SQLITE_OK);
		rc = sqlite3_exec(m_pDB, CREATE_USERS, NULL, NULL, NULL);
		if (rc != SQLITE_OK)
			std::cerr << "CREATE_USERS Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		assert(rc == SQLITE_OK);
		rc = sqlite3_exec(m_pDB, CREATE_GROUPS, NULL, NULL, NULL);
		if (rc != SQLITE_OK)
			std::cerr << "CREATE_GROUPS Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		assert(rc == SQLITE_OK);
		rc = sqlite3_exec(m_pDB, CREATE_GROUP_DEVICES, NULL, NULL, NULL);
		if (rc != SQLITE_OK)
			std::cerr << "CREATE_GROUP_DEVICES Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		assert(rc == SQLITE_OK);
		rc = sqlite3_exec(m_pDB, CREATE_ADMIN, NULL, NULL, NULL);
		if (rc != SQLITE_OK)
			std::cerr << "CREATE_ADMIN Error: " << sqlite3_errmsg(m_pDB) << std::endl;
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

void CAccDatabase::Close()
{
	if (!m_pDB)
		return;

	m_Lock.lock();
	sqlite3_close(m_pDB);
	m_pDB = nullptr;
	m_Lock.unlock();
}

int CAccDatabase::IsValidDevice(const std::string & sn)
{
	if (!m_pDB)
		return -AUTH_ERROR;

	m_Lock.lock();

	std::stringstream sql;
	sql << "select valid_time from devices where sn='" << sn << "'";

	std::string valid_time;
	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		std::string* valid_time = (std::string*)data;
		if (vals[0] != NULL) {
			*valid_time = vals[0];
		}
		else {
			*valid_time = DEFAULT_VALID_TIME;
		}
		return 0;
	}, &valid_time, NULL);

	if (rc != SQLITE_OK)
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;

	m_Lock.unlock();
	if (valid_time.empty())
		return -AUTH_NO_EXITS;

	if (!CheckValidTime(valid_time))
		return -AUTH_TIMEOUT;

	return AUTH_OK;
}

int CAccDatabase::Login(const std::string & id, const std::string & password)
{
	if (!m_pDB)
		return -AUTH_ERROR;

	m_Lock.lock();

	std::stringstream sql;
	sql << "select valid_time from users where id='" << id << "' and passwd='" << password << "' ";
	std::string a = sql.str();
	const char* pa = a.c_str();

	std::string valid_time;
	int nRet = 0;
	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		std::string* valid_time = (std::string*)data;
		if (vals[0] != NULL) {
			*valid_time = vals[0];
		} 
		else {
			*valid_time = DEFAULT_VALID_TIME;
		}
		return 0;
	}, &valid_time, NULL);

	if (rc != SQLITE_OK)
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;

	m_Lock.unlock();

	if (rc != AUTH_OK)
		return rc;

	if (valid_time.empty())
		return AUTH_NO_EXITS;

	if (!CheckValidTime(valid_time))
		return -AUTH_TIMEOUT;

	return AUTH_OK;
}

int CAccDatabase::Logout(const std::string & id)
{
	if (!m_pDB)
		return -AUTH_ERROR;
	return AUTH_OK;
}

int CAccDatabase::AddUser(const DbUserInfo& info, const std::string & passwd)
{
	if (!m_pDB)
		return AUTH_ERROR;

	m_Lock.lock();

	std::stringstream sql;
	sql << "insert into users (level,`group`,id,name,desc,passwd,email,phone,valid_time) ";
	sql << "values(" << info.Level << "," << info.Group << ",";
	sql << "'" << info.ID << "',";
	sql << "'" << info.Name << "',";
	sql << "'" << info.Desc << "',";
	sql << "'" << passwd << "',";
	sql << "'" << info.Email << "',";
	sql << "'" << info.Phone << "',";
	if (info.ValidTime != 0)
		sql << "'" << time2str(&info.ValidTime) << "')";
	else
		sql << "NULL)";

	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), NULL, NULL, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}
	
	m_Lock.unlock();
	if (rc == SQLITE_OK)
		rc = GetUserIndex(info.ID);

	return rc;
}

int CAccDatabase::GetUser(const std::string & id, DbUserInfo& info)
{
	if (!m_pDB)
		return AUTH_ERROR;

	m_Lock.lock();

	info.Index = INVALID_INDEX;
	std::stringstream sql;
	sql << "select `index`,level,`group`,id,name,desc,email,phone,create_time,valid_time from users ";
	sql << "where id = '" << id << "'";

	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		DbUserInfo * pInfo = (DbUserInfo*)data;
		try {
			int i = 0;
			pInfo->Index = atoi(vals[i++]);
			pInfo->Level = atoi(vals[i++]);
			pInfo->Group = atoi(vals[i++]);
			pInfo->ID = SQL2STD_STRING(vals[i++]);
			pInfo->Name = SQL2STD_STRING(vals[i++]);
			pInfo->Desc = SQL2STD_STRING(vals[i++]);
			pInfo->Email = SQL2STD_STRING(vals[i++]);
			pInfo->Phone = SQL2STD_STRING(vals[i++]);
			pInfo->CreateTime = str2time(SQL2STD_STRING(vals[i++]));
			pInfo->ValidTime = str2time(SQL2STD_STRING(vals[i++]));
		}
		catch (...) {
			return -1;
		}
		return 0;
	}, &info, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();

	if (rc == 0 && info.Index != INVALID_INDEX)
		return 0;
	return -1;
}

int CAccDatabase::UpdateUser(const DbUserInfo& info, const std::string & passwd)
{
	if (!m_pDB)
		return AUTH_ERROR;
	if (info.Index == INVALID_INDEX)
		return INVALID_INDEX;

	m_Lock.lock();

	std::stringstream sql;
	if (passwd != "****")
		sql << "replace into users (`index`,level,`group`,id,name,desc,passwd,email,phone,valid_time) ";
	else
		sql << "replace into users (`index`,level,`group`,id,name,desc,email,phone,valid_time) ";
	sql << "values(" << info.Index << "," << info.Level << "," << info.Group << ",";
	sql << "'" << info.ID << "',";
	sql << "'" << info.Name << "',";
	sql << "'" << info.Desc << "',";
	if (passwd != "****")
		sql << "'" << passwd << "',";
	sql << "'" << info.Email << "',";
	sql << "'" << info.Phone << "',";
	if (info.ValidTime != 0)
		sql << "'" << time2str(&info.ValidTime) << "')";
	else
		sql << "NULL)";

	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), NULL, NULL, NULL);
	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();

	return rc;
}

int CAccDatabase::DeleteUser(const std::string & id)
{
	if (!m_pDB)
		return AUTH_ERROR;

	m_Lock.lock();

	std::stringstream sql;
	sql << "delete from users where id='" << id << "'";

	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), NULL, NULL, NULL);
	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();

	return rc;
}

int CAccDatabase::ListUsers(std::list<std::string>& list)
{
	if (!m_pDB)
		return AUTH_ERROR;

	m_Lock.lock();

	std::stringstream sql;
	sql << "select id from users order by `index`";

	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		std::list<std::string> * pList = (std::list<std::string>*)data;
		pList->push_back(std::string(vals[0]));
		return 0;
	}, &list, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();
	return rc;
}

int CAccDatabase::AddDevice(const DbDeviceInfo& info)
{
	if (!m_pDB)
		return AUTH_ERROR;

	m_Lock.lock();

	std::stringstream sql;
	sql << "insert into devices (sn,name,desc,valid_time) ";
	sql << "values('" << info.SN << "',";
	sql << "'" << info.Name << "',";
	sql << "'" << info.Desc << "',";
	if (info.ValidTime != 0)
		sql << "'" << time2str(&info.ValidTime) << "')";
	else
		sql << "NULL)";

	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), NULL, NULL, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();

	if (rc == SQLITE_OK)
		rc = GetDeviceIndex(info.SN);

	return rc;
}

int CAccDatabase::GetDevice(const std::string & sn, DbDeviceInfo& info)
{
	if (!m_pDB)
		return AUTH_ERROR;

	m_Lock.lock();

	info.Index = INVALID_INDEX;
	std::stringstream sql;
	sql << "select `index`,sn,name,desc,create_time,valid_time from devices where sn='" << sn << "'";

	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		DbDeviceInfo * pInfo = (DbDeviceInfo*)data;
		try {
			int i = 0;
			pInfo->Index = atoi(vals[i++]);
			pInfo->SN = SQL2STD_STRING(vals[i++]);
			pInfo->Name = SQL2STD_STRING(vals[i++]);
			pInfo->Desc = SQL2STD_STRING(vals[i++]);
			pInfo->CreateTime = str2time(SQL2STD_STRING(vals[i++]));
			pInfo->ValidTime = str2time(SQL2STD_STRING(vals[i++]));
		}
		catch (...) {
			return -1;
		}
		return 0;
	}, &info, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();

	if (rc == 0 && info.Index != INVALID_INDEX)
		return 0;
	return -1;
}

int CAccDatabase::UpdateDevice(const DbDeviceInfo& info)
{
	if (!m_pDB)
		return AUTH_ERROR;
	if (info.Index == INVALID_INDEX)
		return INVALID_INDEX;

	m_Lock.lock();

	std::stringstream sql;
	sql << "replace into devices (`index`,sn,name,desc,valid_time) ";
	sql << "values(" << info.Index << ",";
	sql << "'" << info.SN << "',";
	sql << "'" << info.Name << "',";
	sql << "'" << info.Desc << "',";
	if (info.ValidTime != 0)
		sql << "'" << time2str(&info.ValidTime) << "')";
	else
		sql << "NULL)";

	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), NULL, NULL, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();

	return rc;
}

int CAccDatabase::DeleteDevice(const std::string & sn)
{
	if (!m_pDB)
		return AUTH_ERROR;

	m_Lock.lock();

	std::stringstream sql;
	sql << "delete from devices where sn='" << sn << "'";

	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), NULL, NULL, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();

	return rc;
}

int CAccDatabase::ListDevices(std::list<std::string>& list)
{
	if (!m_pDB)
		return AUTH_ERROR;

	m_Lock.lock();

	std::stringstream sql;
	sql << "select sn from devices order by `index`";

	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		std::list<std::string> * pList = (std::list<std::string>*)data;
		pList->push_back(std::string(vals[0]));
		return 0;
	}, &list, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();
	return rc;
}

int CAccDatabase::AddGroup(const DbGroupInfo & info)
{
	if (!m_pDB)
		return AUTH_ERROR;

	m_Lock.lock();

	std::stringstream sql;
	sql << "insert into groups (name,desc,valid_time) ";
	sql << "values('" << info.Name << "',";
	sql << "'" << info.Desc << "',";
	if (info.ValidTime != 0)
		sql << "'" << time2str(&info.ValidTime) << "')";
	else
		sql << "NULL)";

	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), NULL, NULL, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();

	if (rc == SQLITE_OK)
		rc = GetGroupIndex(info.Name);

	return rc;
}

int CAccDatabase::GetGroup(const std::string & name, DbGroupInfo & info)
{
	if (!m_pDB)
		return AUTH_ERROR;

	m_Lock.lock();

	info.Index = INVALID_INDEX;
	std::stringstream sql;
	sql << "select `index`,name,desc,create_time,valid_time from groups where name='" << name << "'";

	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		DbGroupInfo * pInfo = (DbGroupInfo*)data;
		try {
			int i = 0;
			pInfo->Index = atoi(vals[i++]);
			pInfo->Name = SQL2STD_STRING(vals[i++]);
			pInfo->Desc = SQL2STD_STRING(vals[i++]);
			pInfo->CreateTime = str2time(SQL2STD_STRING(vals[i++]));
			pInfo->ValidTime = str2time(SQL2STD_STRING(vals[i++]));
		}
		catch (...) {
			return -1;
		}
		return 0;
	}, &info, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();

	if (rc == 0 && info.Index != INVALID_INDEX)
		return 0;
	return -1;
}

int CAccDatabase::UpdateGroup(const DbGroupInfo & info)
{
	if (!m_pDB)
		return AUTH_ERROR;
	if (info.Index == INVALID_INDEX)
		return INVALID_INDEX;

	m_Lock.lock();

	std::stringstream sql;
	sql << "replace into groups (`index`,name,desc,valid_time) ";
	sql << "values(" << info.Index << ",";
	sql << "'" << info.Name << "',";
	sql << "'" << info.Desc << "',";
	if (info.ValidTime != 0)
		sql << "'" << time2str(&info.ValidTime) << "')";
	else
		sql << "NULL)";

	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), NULL, NULL, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();

	return rc;
}

int CAccDatabase::DeleteGroup(const std::string & name)
{
	if (!m_pDB)
		return AUTH_ERROR;

	m_Lock.lock();

	std::stringstream sql;
	sql << "delete from groups where name='" << name << "'";

	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), NULL, NULL, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();

	return rc;
}

int CAccDatabase::ListGroups(std::list<std::string>& list)
{
	if (!m_pDB)
		return AUTH_ERROR;

	m_Lock.lock();

	std::stringstream sql;
	sql << "select name from groups order by `index`";

	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		std::list<std::string> * pList = (std::list<std::string>*)data;
		pList->push_back(std::string(vals[0]));
		return 0;
	}, &list, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();
	return rc;
}

int CAccDatabase::ListGroupDevices(int group, std::list<int>& devices)
{
	if (!m_pDB)
		return AUTH_ERROR;

	m_Lock.lock();

	std::stringstream sql;
	sql << "select devid from group_devices where grpid=" << group << " order by `index`";

	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		std::list<int> * pList = (std::list<int>*)data;
		pList->push_back(atoi(vals[0]));
		return 0;
	}, &devices, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();
	return rc;
}

int CAccDatabase::AddDeviceToGroup(int group, int device)
{
	if (!m_pDB)
		return AUTH_ERROR;

	int rc = CheckDeviceInGroup(group, device);
	if (rc != 0)
		return rc;

	m_Lock.lock();

	std::stringstream sql;
		sql << "insert into group_devices (grpid, devid) values(" << group << ", " << device << ")";

	std::string valid_time;
	rc = sqlite3_exec(m_pDB, sql.str().c_str(), NULL, NULL, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();

	if (rc != SQLITE_OK)
		return -AUTH_ERROR;

	return AUTH_OK;
}

int CAccDatabase::RemoveDeviceToGroup(int group, int device)
{
	m_Lock.lock();

	std::stringstream sql;
	sql << "delete from group_devices where grpid=" << group << " and devid=" << device;

	std::string valid_time;
	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), NULL, NULL, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();

	if (rc != SQLITE_OK)
		return -AUTH_ERROR;

	return AUTH_OK;
}

int CAccDatabase::Access(const std::string & id, const std::string & sn)
{
	if (!m_pDB)
		return -AUTH_ERROR;
	int uid = GetUserIndex(id);
	if (uid == INVALID_INDEX)
		return -AUTH_ERROR;
	int devid = GetDeviceIndex(sn);
	if (devid == INVALID_INDEX)
		return -AUTH_ERROR;

	if (GetUserLevel(id) >= UL_SYS_ADMIN)
		return 0;

	// FIXME:
	return 0;
}

int CAccDatabase::GetUserIndex(const std::string & id)
{
	if (!m_pDB)
		return INVALID_INDEX;

	m_Lock.lock();

	std::stringstream sql;
	sql << "select `index` from users where id='" << id << "'";

	std::string valid_time;
	int nID = -1;
	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		int* nID = (int*)data;
		if (vals[0] != NULL) {
			*nID = atoi(vals[0]);
		}
		else {
			*nID = INVALID_INDEX;
		}
		return 0;
	}, &nID, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();
	return nID;
}

int CAccDatabase::GetUserLevel(const std::string & id)
{
	if (!m_pDB)
		return INVALID_INDEX;

	m_Lock.lock();

	std::stringstream sql;
	sql << "select level from users where id='" << id << "'";

	std::string valid_time;
	int nLevel = -1;
	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		int* nID = (int*)data;
		if (vals[0] != NULL) {
			*nID = atoi(vals[0]);
		}
		return 0;
	}, &nLevel, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();
	return nLevel;
}

int CAccDatabase::GetDeviceIndex(const std::string & id)
{
	if (!m_pDB)
		return INVALID_INDEX;

	m_Lock.lock();

	std::stringstream sql;
	sql << "select `index` from devices where sn='" << id << "'";

	std::string valid_time;
	int nID = -1;
	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		int* nID = (int*)data;
		if (vals[0] != NULL) {
			*nID = atoi(vals[0]);
		}
		else {
			*nID = INVALID_INDEX;
		}
		return 0;
	}, &nID, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();
	return nID;
}

int CAccDatabase::GetGroupIndex(const std::string & id)
{
	if (!m_pDB)
		return INVALID_INDEX;

	m_Lock.lock();

	std::stringstream sql;
	sql << "select `index` from groups where name='" << id << "'";

	std::string valid_time;
	int nID = -1;
	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		int* nID = (int*)data;
		if (vals[0] != NULL) {
			*nID = atoi(vals[0]);
		}
		else {
			*nID = INVALID_INDEX;
		}
		return 0;
	}, &nID, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();
	return nID;
}

int CAccDatabase::CheckDeviceInGroup(int group, int device)
{
	m_Lock.lock();

	std::stringstream sql;
	sql << "select index from group_devcies where grpid=" << group << " and devid=" << device;

	std::string valid_time;
	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		return 0;
	}, &valid_time, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();

	return rc;
}