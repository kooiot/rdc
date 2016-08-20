#pragma once

#include "vspd/ftvspc.h"
#include "DataDefs.h"
#include "Handlers.h"
#include <string>

class VSPort : public IPort
{
protected:
	friend class VSPortMgr;
	VSPort(RC_CHANNEL channel, IPortHandler* Handler, const std::string& name);
	~VSPort();

	virtual bool Open();
	virtual void Close();

	virtual int Write(void* buf, size_t len);
	virtual int Event(StreamEvent evt);

private:
	static LONG_PTR __cdecl OnVspcPortEvents(
		FtVspc_PortEvent Event,	// event type
		ULONG_PTR ulValue,	// value (depends of event type)
		LONG_PTR Context	// user value (set by function FtVspcApiInit)
		);

	void ShowError();

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
	std::string m_Name;
	FTVSPCHANDLE m_hPort;
};

