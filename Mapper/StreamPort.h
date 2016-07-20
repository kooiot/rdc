#pragma once

#include <enet/enet.h>
#include <string>
#include <thread>
#include "../api/DataDefs.h"

class IStreamPort {
public:
	virtual ~IStreamPort() {};
	
	virtual bool Open() = 0;
	virtual void Close() = 0;

	virtual int OnClientData(void* data, size_t len) = 0;
};

class StreamPortBase : public IStreamPort
{
public:
	StreamPortBase(ENetPeer* peer, const ConnectionInfo& info, int mask);
	~StreamPortBase();

	virtual bool Open() = 0;
	virtual void Close() = 0;
	virtual int OnWrite(void* data, size_t len) = 0;

	virtual int SendData(void* data, size_t len);
	virtual int FireEvent(StreamEvent se, const char* msg = "");
private:
	virtual int OnClientData(void* data, size_t len);

protected:
	ConnectionInfo m_Info;
	ENetPeer* m_Peer;
	int m_Mask;
};
