#pragma once

#include "StreamPort.h"

class TestStream  : public StreamPortBase
{
public:
	TestStream(StreamPortInfo& info);
	~TestStream();

	virtual bool Open();
	virtual void Close();

	virtual void Run();
	virtual int OnWrite(void* data, size_t len);
private:
	std::thread* m_pThread;
	bool m_bAbort;
};

