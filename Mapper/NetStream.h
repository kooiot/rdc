#pragma once

#include <vector>
#include <thread>
#include <PortMgr.h>
#include "StreamPort.h"

class NetStream : public StreamPortBase, public IPortHandler
{
public:
	NetStream(CPortMgr& mgr, StreamPortInfo& info);
	~NetStream();

	// StreamPortBase
	virtual bool Open();
	virtual void Close();
	virtual int OnWrite(void* data, size_t len);

	// IPortHandler
	virtual int OnLog(RC_CHANNEL channel, const char* type, const char* content);
	virtual int OnRecv(RC_CHANNEL channel, void* buf, size_t len);
	virtual void OnClose(RC_CHANNEL channel);
	virtual void OnOpen(RC_CHANNEL channel, bool open);

private:
	CPortMgr& m_Mgr;
	IPort* m_Port;
};

