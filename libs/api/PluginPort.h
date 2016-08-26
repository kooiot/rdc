#pragma once

#include "DataDefs.h"
#include "Handlers.h"
#include "PluginApi.h"
#include <string>
#include <cstdarg>

class PluginPort : public IPort
{
public:
	PluginPort(RC_CHANNEL channel, IPortHandler* Handler, const PluginInfo& info, PluginApi* api);
	~PluginPort();

	virtual bool Open();
	virtual void Close();

	virtual int Write(void* buf, size_t len);
	virtual int Event(StreamEvent evt);

private:
	static int __stdcall SendCB(const char* buf, size_t len, void* ptr);
	static int __stdcall CloseCB(void* ptr);

	void Error(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		char* temp = new char[1024];
		vsprintf(temp, fmt, args);
		m_pHandler->OnLog(m_nChannel, "ERROR", temp);
		va_end(args);
	}

	void Info(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		char* temp = new char[1024];
		vsprintf(temp, fmt, args);
		m_pHandler->OnLog(m_nChannel, "INFO", temp);
		va_end(args);
	}
private:
	PluginInfo m_Info;
	PluginApi* m_Api;
	PLUGIN_HANDLE m_ApiHandle;
};

