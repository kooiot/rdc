#pragma once

#include <enet/enet.h>
#include <string>
#include <thread>
#include "../api/DataDefs.h"

class IStreamPort {
public:
	virtual ~IStreamPort() {};
	
	virtual void Start() = 0;
	virtual bool Open() = 0;
	virtual void Close() = 0;

	virtual int OnClientData(void* data, size_t len) = 0;
};

class StreamPortBase : public IStreamPort
{
public:
	StreamPortBase(ENetPeer* peer, const ConnectionInfo& info, int mask);
	~StreamPortBase();

	virtual void Start();
	virtual bool Open();
	virtual void Close();

	virtual void Run() = 0;
	virtual int OnWrite(void* data, size_t len) = 0;

	virtual int SendData(void* data, size_t len);

private:
	virtual int OnClientData(void* data, size_t len);

protected:
	ConnectionInfo m_Info;
	ENetPeer* m_Peer;
	int m_Mask;

	std::thread* m_pThread;
	bool m_bAbort;
};