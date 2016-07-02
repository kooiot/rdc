#pragma once

#include "StreamPort.h"
#include <enet/enet.h>
#include <serial/serial.h>
#include <string>
#include "../api/DataDefs.h"

class SerialStream  : public IStreamPort
{
public:
	SerialStream(ENetPeer* peer, const ConnectionInfo& info);
	~SerialStream();

	virtual bool Open();
	virtual void Close();

	virtual int OnClientData(void* data, size_t len);

private:
	ENetPeer* m_Peer;
	serial::Serial* m_Serial;
};

