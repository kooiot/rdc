#pragma once

#include <string>
#include "DataDefs.h"

class CAccApi {
public:
	CAccApi(void* ctx);
	~CAccApi();

public:
	bool Connect(const char* sip, int port, const char* un, const char* pass);
	int Disconnect();

	int SendHeartbeat();

	int AddDevice(const DeviceInfo* info);
	int ModifyDevice(const DeviceInfo* info);
	int DeleteDevice(const char* sn);
	int ListDevices(DeviceInfo* list, int list_len);

	int AddUser(const UserInfo* info);
	int ModifyUser(const UserInfo* info);
	int DeleteUser(const char* id);
	int ListUsers(UserInfo* list, int list_len);

	int Allow(const char* id, const char* devsn, time_t valid_time);
	int Deny(const char* id, const char* devsn);

	// Return the Connection Channel Index
	int CreateConnection(const ConnectionInfo* info);
	int DestroyConnection(int index);

private:
	void* m_CTX;
	void* m_Socket;
	std::string m_ID;
};
