// RemoteConnectorApi.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "RemoteConnectorApi.h"
#include "AccApi.h"
#include <zmq.h>
#include <list>

void* g_ctx = NULL;
std::list<CAccApi*> g_HandleList;

extern "C"
int RC_Init()
{
	if (g_ctx == NULL)
		g_ctx = zmq_ctx_new();
	return 0;
}

extern "C"
int RC_Close()
{
	std::list<CAccApi*>::iterator ptr = g_HandleList.begin();
	for (; ptr != g_HandleList.end(); ++ptr) {
		CAccApi* pApi = *ptr;
		int rc = pApi->Disconnect();
		delete pApi;
	}

	if (g_ctx) {
		zmq_ctx_term(g_ctx);
	}
	return 0;
}


extern "C"
RC_HANDLE RC_Connect(const char * sip, int port, const char * un, const char * pass)
{
	CAccApi* pApi = new CAccApi(g_ctx);
	if (pApi->Connect(sip, port, un, pass)) {
		g_HandleList.push_back(pApi);
		return pApi;
	}
	delete pApi;
	return NULL;
}

extern "C"
int RC_Disconnect(RC_HANDLE Api)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi) {
		g_HandleList.remove(pApi);
		int rc = pApi->Disconnect();
		delete pApi;
		return 0;
	}
	return -1;
}

extern "C"
int RC_ListDevices(RC_HANDLE Api, DeviceInfo * list, int list_len)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->ListDevices(list, list_len);
	return -1;
}

extern "C"
int RC_ListUsers(RC_HANDLE Api, UserInfo * list, int list_len)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->ListUsers(list, list_len);
	return -1;
}

extern "C"
int RC_ConnectSerial(RC_HANDLE Api, const char * id, const char * devid, const SerialInfo & info)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->ConnectSerial(id, devid, info);
	return -1;
}

extern "C"
int RC_CloseSerial(RC_HANDLE Api, const char * id)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->CloseSerial(id);
	return -1;
}

extern "C"
int RC_ConnectTCPC(RC_HANDLE Api, const char * id, const char * devid, const TCPClientInfo & info)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->ConnectTCPC(id, devid, info);
	return -1;
}

extern "C"
int RC_CloseTCPC(RC_HANDLE Api, const char * id)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->CloseTCPC(id);
	return -1;
}

extern "C"
int RC_ConnectUDP(RC_HANDLE Api, const char * id, const char * devid, const UDPInfo & info)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->ConnectUDP(id, devid, info);
	return -1;
}

extern "C"
int RC_CloseUDP(RC_HANDLE Api, const char * id)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->CloseUDP(id);
	return -1;
}
