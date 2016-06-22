#pragma once

#include <map>
#include <list>


struct StreamProcess {
	int Counter;
	const std::string StreamIP;
	int MapperPort;
	int ClientPort;
};

class CStreamServerMgr
{
public:
	CStreamServerMgr();
	~CStreamServerMgr();
public:
	void* Init(void* ctx, const char* bip, int port);
	void Close();
	void OnRecv();

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

