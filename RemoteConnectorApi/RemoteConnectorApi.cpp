// RemoteConnectorApi.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <zmq.h>
#include <list>

#include "RemoteConnectorApi.h"
#include "AccApi.h"

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
RC_CONNECT_HANDLE RC_ConnectSerial(RC_HANDLE Api, const char * devid, const SerialInfo* info)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi) {
		ConnectionInfo ci;
		ci.Type = CT_SERIAL;
		memcpy(&ci.Serial, info, sizeof(SerialInfo));
		memcpy(ci.DevSN, devid, RC_MAX_SN_LEN);

		return pApi->CreateConnection(&ci);
	}
	return -1;
}

extern "C"
int RC_CloseSerial(RC_HANDLE Api, RC_CONNECT_HANDLE conn)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->DestroyConnection(conn);
	return -1;
}

extern "C"
RC_CONNECT_HANDLE RC_ConnectTCPC(RC_HANDLE Api, const char * devid, const  TCPClientInfo* info)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi) {
		ConnectionInfo ci;
		ci.Type = CT_TCPC;
		memcpy(&ci.TCPClient, info, sizeof(TCPClientInfo));
		memcpy(ci.DevSN, devid, RC_MAX_SN_LEN);

		return pApi->CreateConnection(&ci);
	}
	return -1;
}

extern "C"
int RC_CloseTCPC(RC_HANDLE Api, RC_CONNECT_HANDLE conn)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->DestroyConnection(conn);
	return -1;
}

extern "C"
RC_CONNECT_HANDLE RC_ConnectUDP(RC_HANDLE Api, const char * devid, const UDPInfo* info)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi) {
		ConnectionInfo ci;
		ci.Type = CT_UDP;
		memcpy(&ci.Serial, info, sizeof(UDPInfo));
		memcpy(ci.DevSN, devid, RC_MAX_SN_LEN);

		return pApi->CreateConnection(&ci);
	}
	return -1;
}

extern "C"
int RC_CloseUDP(RC_HANDLE Api, RC_CONNECT_HANDLE conn)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->DestroyConnection(conn);
	return -1;
}
