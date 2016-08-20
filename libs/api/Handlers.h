#pragma once

#include <cstdlib>

class IPortHandler {
public:
	virtual ~IPortHandler() {}
	virtual int OnLog(RC_CHANNEL channel, const char* type, const char* content) = 0;
	virtual int Send(RC_CHANNEL channel, void* buf, size_t len) = 0;
	virtual void Close() = 0;
};

class IPort {
public:
	IPort(RC_CHANNEL channel, IPortHandler* handler) : m_nChannel(channel), m_pHandler(handler) {}
	virtual ~IPort() { delete m_pHandler; }

	virtual bool Open() = 0;
	virtual void Close() = 0;

	virtual int Write(void* buf, size_t len) = 0;
	virtual int Event(StreamEvent evt) { return 0; }

	// Fire close to port hoster
	virtual void OnClose() {
		m_pHandler->Close();
	}
protected:
	RC_CHANNEL m_nChannel;
	IPortHandler* m_pHandler;
};