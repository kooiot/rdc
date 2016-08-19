#pragma once

#include <enet/enet.h>
#include <string>
#include <mutex>
#include <thread>
#include "../api/DataDefs.h"

class IStreamPort {
public:
	virtual ~IStreamPort() {};
	
	virtual bool Open() = 0;
	virtual void Close() = 0;

	virtual int OnClientData(void* data, size_t len) = 0;
};
class StreamMgr;
struct StreamPortInfo {
	StreamMgr* Mgr;
	ENetPeer* Peer;
	ConnectionInfo ConnInfo;
	int Mask;
};
class StreamPortBase : public IStreamPort
{
public:
	StreamPortBase(StreamPortInfo& info);
	~StreamPortBase();

	virtual bool Open() = 0;
	virtual void Close() = 0;
	virtual int OnWrite(void* data, size_t len) = 0;

	virtual int SendData(void* data, size_t len);
	virtual int FireEvent(StreamEvent se, const char* msgfmt = "", ...);
	virtual int OnStreamClose();
private:
	virtual int _FireEvent(StreamEvent se, const char* msg);
	virtual int OnClientData(void* data, size_t len);

protected:
	StreamPortInfo m_Info;
	std::mutex* m_pLock;
};
