#pragma once

#include <map>
#include <vector>
#include <koo_zmq_helpers.h>
#include <DataDefs.h>

class CStreamServerMgr
{
public:
	CStreamServerMgr();
	~CStreamServerMgr();
public:
	void* Init(void* ctx, const char* bip, int port);
	void Close();
	void OnRecv();
private:
	void HandleKZPacket(const KZPacket & cmd, void * rep);

public:
	StreamProcess* Alloc();
	void Release(StreamProcess* pData);

	bool AddStream(int id, IPInfo* Process);
	bool RemoveStream(int id);
	bool UpdateStream(int id, IPInfo* Process);

private:
	void * m_pSocket;
	struct StreamProcessVector {
		int Online;
		int Counter;
		time_t Update;
		bool Used[RC_MAX_CONNECTION_PER_SERVER];
		StreamProcess List[RC_MAX_CONNECTION_PER_SERVER];
	};
	StreamProcessVector m_Streams[RC_MAX_STREAM_SERVER_COUNT];
};

