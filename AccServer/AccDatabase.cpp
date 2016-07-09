#include "AccDatabase.h"
#include <Windows.h>
#include <iostream>
#include <cassert>
#include <sstream>
#include "AccDatabase.h"

#include <DataDefs.h>

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

const std::string time2str(const time_t *_Time = NULL) {
	char buffer[80];
	time_t now = _Time != NULL ? *_Time : time(NULL);

	struct tm * timeinfo = localtime(&now);
	strftime(buffer, 80, "%F %T", timeinfo);

	return std::string(buffer);
}
const std::string time2str_utc(const time_t *_Time = NULL) {
	char buffer[80];
	time_t now = _Time != NULL ? *_Time : time(NULL);

	struct tm * timeinfo = gmtime(&now);
	strftime(buffer, 80, "%F %T", timeinfo);

	return std::string(buffer);
}

bool CheckValidTime(const std::string& valid_time) {

	time_t now = time(NULL);
	time_t valid = str2time_utc(valid_time);

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
	memcpy(info.ID, ID.c_str(), RC_MAX_ID_LEN);
	memcpy(info.Name, Name.c_str(), RC_MAX_NAME_LEN);
	memcpy(info.Desc, Desc.c_str(), RC_MAX_DESC_LEN);
	memcpy(info.Passwd, "YouCannotSeeMe", RC_MAX_PASSWD_LEN);
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
"[devid] INTEGER  NOT NULL,"
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
	std::string db_file = GetModuleFilePath() + "\\data.sqlite3.db";
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
		rc = sqlite3_exec(m_pDB, CREATE_PERMISSIONS, NULL, NULL, NULL);
		if (rc != SQLITE_OK)
			std::cerr << "CREATE_PERMISSIONS Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		assert(rc == SQLITE_OK);
		rc = sqlite3_exec(m_pDB, CREATE_ADMIN, NULL, NULL, NULL);
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
	if (valid_time.empty())
		return -AUTH_NO_EXITS;

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

	if (GetUserLevel(id) >= 99)
		return 0;

	return _Access(uid, devid);
}

int CAccDatabase::Allow(const std::string & id, const std::string & sn, time_t* _Time/* = NULL*/)
{
	if (!m_pDB)
		return -AUTH_ERROR;

	int uid = GetUserIndex(id);
	if (uid == INVALID_INDEX)
		return -AUTH_ERROR;

	int devid = GetDeviceIndex(sn);
	if (devid == INVALID_INDEX)
		return -AUTH_ERROR;

	return _Allow(uid, devid, _Time);
}

int CAccDatabase::Deny(const std::string & id, const std::string & sn)
{
	if (!m_pDB)
		return -AUTH_ERROR;

	int uid = GetUserIndex(id);
	if (uid == INVALID_INDEX)
		return -AUTH_ERROR;

	int devid = GetDeviceIndex(sn);
	if (devid == INVALID_INDEX)
		return -AUTH_ERROR;

	return _Deny(uid, devid);
}

int CAccDatabase::AddUser(const DbUserInfo& info, const std::string & passwd)
{
	if (!m_pDB)
		return AUTH_ERROR;

	m_Lock.lock();

	std::stringstream sql;
	sql << "insert into users (level,id,name,desc,passwd,email,phone,valid_time) ";
	sql << "values(" << info.Level << ",";
	sql << "'" << info.ID << "',";
	sql << "'" << info.Name << "',";
	sql << "'" << info.Desc << "',";
	sql << "'" << passwd << "',";
	sql << "'" << info.Email << "',";
	sql << "'" << info.Phone << "',";
	if (info.ValidTime != -1)
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
	sql << "select `index`,level,id,name,desc,email,phone,create_time,valid_time from users ";
	sql << "where id = '" << id << "'";

	int rc = sqlite3_exec(m_pDB, sql.str().c_str(), [](void* data, int row, char** vals, char** cols) {
		DbUserInfo * pInfo = (DbUserInfo*)data;
		try {
			int i = 0;
			pInfo->Index = atoi(vals[i++]);
			pInfo->Level = atoi(vals[i++]);
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
	sql << "replace into users (`index`,level,id,name,desc,passwd,email,phone,valid_time) ";
	sql << "values(" << info.Index << "," << info.Level << ",";
	sql << "'" << info.ID << "',";
	sql << "'" << info.Name << "',";
	sql << "'" << info.Desc << "',";
	sql << "'" << passwd << "',";
	sql << "'" << info.Email << "',";
	sql << "'" << info.Phone << "',";
	if (info.ValidTime != -1)
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
	if (info.ValidTime != -1)
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
	if (info.ValidTime != -1)
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

int CAccDatabase::_Access(int user, int dev)
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
			*valid_time = DEFAULT_VALID_TIME;
		}
		return 0;
	}, &valid_time, NULL);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();

	if (valid_time.empty())
		return -AUTH_NO_EXITS;

	if (!CheckValidTime(valid_time))
		return -AUTH_TIMEOUT;

	return AUTH_OK;
}

int CAccDatabase::_Allow(int user, int dev, time_t * _Time)
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

	if (rc != SQLITE_OK) {
		std::cerr << "SQL Error: " << sqlite3_errmsg(m_pDB) << std::endl;
		std::cerr << "SQL: " << sql.str() << std::endl;
	}

	m_Lock.unlock();

	if (rc != SQLITE_OK)
		return -AUTH_ERROR;

	return AUTH_OK;
}

int CAccDatabase::_Deny(int user, int dev)
{
	m_Lock.lock();

	std::stringstream sql;
	sql << "delete from permissions where uid=" << user << " and devid=" << dev;

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
