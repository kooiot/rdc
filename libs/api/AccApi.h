#pragma once

#include <string>
#include <functional>
#include <mutex>
#include <thread>
#include "DataDefs.h"

struct KZPacket;
class CAccApi {
public:
	CAccApi(void* ctx);
	~CAccApi();

public:
	bool Connect(const char* sip, int port, const char* un, const char* pass);
	int Disconnect();

	int GetStreamServer(StreamProcess* info);

	int SendHeartbeat();

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

	int Allow(const char* id, const char* devsn, time_t valid_time);
	int Deny(const char* id, const char* devsn);

	// Return the Connection Channel Index
	int CreateConnection(const ConnectionInfo* info);
	int DestroyConnection(int index);
	int ListConnection(int* channels); // RC_MAX_CONNECTION
private:
	bool _Connect();
	int SendRequest(KZPacket& packet, std::function< int(KZPacket&)> cb = nullptr);
private:
	std::mutex m_Lock;
	std::thread* m_pHBThread;
	bool m_bStop;
	void* m_CTX;
	void* m_Socket;
	void* m_PubSocket;
	std::string m_Conn;
	std::string m_User;
	std::string m_Pass;
};
