#pragma once

#include <string>
#include <map>
#include <mutex>
#include <list>
#include <sqlite3.h>

#define AUTH_OK 0
#define AUTH_ERROR 1
#define AUTH_NO_EXITS 2
#define AUTH_TIMEOUT 3
#define INVALID_INDEX 0xFFFFFFFF
#define DEFAULT_VALID_TIME "2017-6-25 23:49:59"

struct UserInfo;
struct DbUserInfo {
	int Index;
	int Level;
	std::string ID;
	std::string Name;
	std::string Desc;
	std::string Email;
	std::string Phone;
	time_t CreateTime;
	time_t ValidTime;

	DbUserInfo();

	void ToUserInfo(UserInfo& info);
	void FromUserInfo(const UserInfo& info);
};

struct DeviceInfo;
struct DbDeviceInfo {
	int Index;
	std::string SN;
	std::string Name;
	std::string Desc;
	time_t CreateTime;
	time_t ValidTime;

	DbDeviceInfo();

	void ToDeviceInfo(DeviceInfo& info);
	void FromDeviceInfo(const DeviceInfo& info);
};

class CAccDatabase
{
public:
	CAccDatabase();
	~CAccDatabase();
	
	int Init();
	void Close();

	// Devices
	int IsValidDevice(const std::string& sn);

	// Users
	int Login(const std::string& id, const std::string& password);
	int Logout(const std::string& id);

	// Check Permissions
	int Access(const std::string& id, const std::string& sn);
	// Allow access (overwrite original permisson)
	int Allow(const std::string& id, const std::string& sn, time_t* _Time = NULL);
	// Remove the access
	int Deny(const std::string& id, const std::string& sn);

	int AddUser(const DbUserInfo& info, const std::string& passwd);
	int GetUser(const std::string& id, DbUserInfo& info);
	int UpdateUser(const DbUserInfo& info, const std::string& passwd);
	int DeleteUser(const std::string& id);
	int ListUsers(std::list<std::string>& list);

	int AddDevice(const DbDeviceInfo& info);
	int GetDevice(const std::string& sn, DbDeviceInfo& info);
	int UpdateDevice(const DbDeviceInfo& info);
	int DeleteDevice(const std::string& sn);
	int ListDevices(std::list<std::string>& list);

protected:
	int GetUserIndex(const std::string& id);
	int GetDeviceIndex(const std::string& id);
	int _Access(int user, int dev);
	int _Allow(int user, int dev, time_t* _Time = NULL);
	int _Deny(int user, int dev);
private:
	sqlite3 * m_pDB;
	std::mutex m_Lock;
};

