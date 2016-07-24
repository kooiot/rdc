#pragma once

#include "StreamPort.h"
#include <PluginApi.h>
#include <PluginLoader.h>

class PluginStream  : public StreamPortBase
{
public:
	PluginStream(CPluginLoader& loader, StreamPortInfo& info);
	~PluginStream();

	virtual bool Open();
	virtual void Close();
	virtual int OnWrite(void* data, size_t len);
protected:
	static int __stdcall SendCB(const char* buf, size_t len, void* ptr);
	static int __stdcall CloseCB(void* ptr);
private:
	CPluginLoader& m_Loader;
	PluginApi* m_Plugin;
	long m_Instance;
};

