// Example.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <PluginDefs.h>
#include "PortMgr.h"
#include "PortHandler.h"

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
long CreateHandle(char *config,
	PluginSendCB send,
	PluginCloseCB close,
	void* ptr)
{
	CPortMgr::Instance().Init();

	CPortHandler* handler = new CPortHandler(send, close, ptr);
	IPort* port = CPortMgr::Instance().Create(handler, config);
	
	return (long)port;
}

extern "C"
int Destory(long Handle)
{
	IPort* port = (IPort*)Handle;
	if (port)
		CPortMgr::Instance().Destory(port);

	return 0;
}

extern "C"
int Open(long Handle)
{
	IPort* port = (IPort*)Handle;
	if (port)
		port->Open();
	return 0;
}

extern "C"
int Close(long Handle)
{
	IPort* port = (IPort*)Handle;
	if (port)
		port->Close();
	return 0;
}

extern "C"
int Write(long Handle, const char* buf, size_t len)
{
	IPort* port = (IPort*)Handle;
	if (port)
		return port->Write((void*)buf, len);
	return 0;
}
