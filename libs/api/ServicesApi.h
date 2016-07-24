#pragma once

#include <string>
#include <functional>
#include <mutex>
#include <thread>
#include "DataDefs.h"

struct KZPacket;
class CServicesApi {
public:
	CServicesApi(void* ctx);
	~CServicesApi();

public:
	bool Connect();
	int Disconnect();

	int Add(const ServiceNode* node);
	int Modify(const ServiceNode* node);
	int Delete(const char* name);
	int List(ServiceNode* list, int list_len, bool only_online = true);
	int Start(const char* name);
	int Stop(const char* name);

private:
	bool _Connect();
	int SendRequest(KZPacket& packet, std::function< int(KZPacket&)> cb = nullptr);
private:
	void* m_CTX;
	void* m_Socket;
};
