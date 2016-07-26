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
	return "Example";
}

extern "C"
long CreateHandle(char *config,
	PluginSendCB send,
	PluginCloseCB close,
	void* ptr)
{
	CPort* port =  new CPort(config, send, close, ptr);
	return (long)port;
}

extern "C"
int Destory(long Handle)
{
	CPort* port = (CPort*)Handle;
	if (port)
		delete port;
	return 0;
}

extern "C"
int Open(long Handle)
{
	CPort* port = (CPort*)Handle;
	if (port)
		port->Open();
	return 0;
}

extern "C"
int Close(long Handle)
{
	CPort* port = (CPort*)Handle;
	if (port)
		port->Close();
	return 0;
}

extern "C"
int Write(long Handle, const char* buf, size_t len)
{
	CPort* port = (CPort*)Handle;
	if (port)
		return port->Write(buf, len);
	return 0;
}
