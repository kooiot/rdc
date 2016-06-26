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

	int ListDevices(DeviceInfo* list, int list_len);
	int ListUsers(UserInfo* list, int list_len);
	int ConnectSerial(const char*  id, const char*  devid, const SerialInfo& info);
	int CloseSerial(const char*  id);
	int ConnectTCPC(const char*  id, const char*  devid, const TCPClientInfo& info);
	int CloseTCPC(const char*  id);
	int ConnectUDP(const char*  id, const char*  devid, const UDPInfo& info);
	int CloseUDP(const char*  id);

private:
	void* m_CTX;
	void* m_Socket;
	std::string m_ID;
};