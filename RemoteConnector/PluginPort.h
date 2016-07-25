#pragma once

#include "vspd/ftvspc.h"
#include "Handlers.h"
#include <PlugInApi.h>
#include <string>

class PluginPort : public IStreamHandler
{
public:
	PluginPort(RC_CHANNEL channel, IPortHandler& Handler, const PluginInfo& info, PluginApi* api);
	~PluginPort();

	virtual bool Open();
	virtual void Close();

	virtual int OnData(void* buf, size_t len);
	virtual int OnEvent(StreamEvent evt);

private:
	static int __stdcall SendCB(const char* buf, size_t len, void* ptr);
	static int __stdcall CloseCB(void* ptr);

	void Error(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		char* temp = new char[1024];
		vsprintf(temp, fmt, args);
		m_Handler.OnLog(m_nChannel, "ERROR", temp);
		va_end(args);
	}

	void Info(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		char* temp = new char[1024];
		vsprintf(temp, fmt, args);
		m_Handler.OnLog(m_nChannel, "INFO", temp);
		va_end(args);
	}
private:
	RC_CHANNEL m_nChannel;
	IPortHandler& m_Handler;
	PluginInfo m_Info;
	PluginApi* m_Api;
	long m_ApiHandle;
};

