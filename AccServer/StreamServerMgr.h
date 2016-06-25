#pragma once

#include <map>
#include <list>
#include <koo_zmq_helpers.h>

struct StreamProcess;
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

	bool AddStream(int id, StreamProcess* Process);
	StreamProcess* RemoveStream(int id);

private:
	void * m_pSocket;
	typedef std::map<int, StreamProcess*> StreamProcessMap;
	StreamProcessMap m_Streams;
};

