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
	int Group;
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

struct GroupInfo;
struct DbGroupInfo {
	int Index;
	std::string Name;
	std::string Desc;
	time_t CreateTime;
	time_t ValidTime;
	
	DbGroupInfo();
	
	void ToGroupInfo(GroupInfo& info);
	void FromGroupInfo(const GroupInfo& info);
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

	int AddGroup(const DbGroupInfo& info);
	int GetGroup(const std::string& name, DbGroupInfo& info);
	int UpdateGroup(const DbGroupInfo& info);
	int DeleteGroup(const std::string& name);
	int ListGroups(std::list<std::string>& list);

	int ListGroupDevices(int group, std::list<int>& devices);
	int AddDeviceToGroup(int group, int device);
	int RemoveDeviceToGroup(int group, int device);

	int Access(const std::string & id, const std::string & sn);

protected:
	int GetUserIndex(const std::string& id);
	int GetUserLevel(const std::string& id);
	int GetDeviceIndex(const std::string& id);
	int GetGroupIndex(const std::string& id);
	int CheckDeviceInGroup(int group, int device);
private:
	sqlite3 * m_pDB;
	std::mutex m_Lock;
};

