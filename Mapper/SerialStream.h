#pragma once

#include "StreamPort.h"
#include <serial/serial.h>

class SerialStream  : public StreamPortBase
{
public:
	SerialStream(StreamPortInfo& info);
	~SerialStream();

	virtual bool Open();
	virtual void Close();

	virtual void Run();
	virtual int OnWrite(void* data, size_t len);

private:
	serial::Serial* m_Serial;

	std::thread* m_pThread;
	bool m_bAbort;
};

