// Example.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <PluginDefs.h>
#include "Port.h"

extern "C" 
RDC_PLUGIN_TYPE GetType()
{
	return (RDC_PLUGIN_TYPE)(RPT_MAPPER | RPT_CLIENT);
}

extern "C"
const char* GetName()
{
	return "MultiNet";
}

extern "C"
const char* GetInfo()
{
	return "Mutiple Network Connections via One Channel";
}

extern "C"
PLUGIN_HANDLE CreateHandle(char *config,
	PluginSendCB send,
	PluginCloseCB close,
	void* ptr)
{
	CPort* port =  new CPort(config, send, close, ptr);
	return port;
}

extern "C"
int Destory(PLUGIN_HANDLE Handle)
{
	CPort* port = (CPort*)Handle;
	if (port)
		delete port;
	return 0;
}

extern "C"
int Open(PLUGIN_HANDLE Handle)
{
	CPort* port = (CPort*)Handle;
	if (port)
		port->Open();
	return 0;
}

extern "C"
int Close(PLUGIN_HANDLE Handle)
{
	CPort* port = (CPort*)Handle;
	if (port)
		port->Close();
	return 0;
}

extern "C"
int Write(PLUGIN_HANDLE Handle, const char* buf, size_t len)
{
	CPort* port = (CPort*)Handle;
	if (port)
		return port->Write(buf, len);
	return 0;
}

