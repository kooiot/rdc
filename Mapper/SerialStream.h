#pragma once

#include "StreamPort.h"
#include <serial/serial.h>

class SerialStream  : public StreamPortBase
{
public:
	SerialStream(ENetPeer* peer, const ConnectionInfo& info, int mask);
	~SerialStream();

	virtual bool Open();
	virtual void Close();

	virtual void Run();
	virtual int OnWrite(void* data, size_t len);

private:
	serial::Serial* m_Serial;

};

