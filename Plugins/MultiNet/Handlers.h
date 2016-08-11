#pragma once

#include <cstdlib>

class IPortHandler {
public:
	virtual ~IPortHandler() {}
	virtual int OnLog(int channel, const char* type, const char* content) = 0;
	virtual int Send(int channel, void* buf, size_t len) = 0;
	virtual void Close() = 0;
};

class IPort {
public:
	IPort(int channel, IPortHandler* handler) : m_nChannel(channel), m_pHandler(handler) {}
	virtual ~IPort() { delete m_pHandler; }
	virtual int Write(void* buf, size_t len) = 0;

	virtual bool Open() = 0;
	virtual void Close() = 0;
protected:
	int m_nChannel;
	IPortHandler* m_pHandler;
};
