#pragma once

#include <string>
#include <functional>
#include <mutex>
#include <thread>
#include "DataDefs.h"

struct KZPacket;
class CAccMgrApi {
public:
	CAccMgrApi(void* ctx);
	~CAccMgrApi();

public:
	bool Connect(const char* sip, int port, const char* un, const char* pass);
	int Disconnect();

	// Return Device Index Where Add
	int AddDevice(const DeviceInfo* info);
	int ModifyDevice(const DeviceInfo* info);
	int DeleteDevice(const char* sn);
	int ListDevices(DeviceInfo* list, int list_len, bool only_online = true);
	int GetDeviceInfo(const char* sn, DeviceInfo* info);

	// Return User Index Where Add
	int AddUser(const UserInfo* info);
	int ModifyUser(const UserInfo* info);
	int DeleteUser(const char* id);
	int ListUsers(UserInfo* list, int list_len, bool only_online = true);
	int GetUserInfo(const char * id, UserInfo* info);


	// Return Group Index Where Add
	int AddGroup(const GroupInfo* info);
	int ModifyGroup(const GroupInfo* info);
	int DeleteGroup(const char* name);
	int ListGroups(GroupInfo* list, int list_len);
	int GetGroupInfo(const char * name, GroupInfo* info);

	int ListGroupDevices(int group, int* list, int list_len);
	int AddDeviceToGroup(int group, int device);
	int RemoveDeviceToGroup(int group, int device);
private:
	bool _Connect();
	int SendRequest(KZPacket& packet, std::function< int(KZPacket&)> cb = nullptr);
private:
	std::mutex m_Lock;
	void* m_CTX;
	void* m_Socket;
	std::string m_Conn;
	std::string m_User;
	std::string m_Pass;
};
