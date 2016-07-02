// RemoteConnectorApi.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <zmq.h>
#include <list>

#include "RemoteConnectorApi.h"
#include "AccApi.h"
#include "StreamApi.h"
#include "StreamHandler.h"

void* g_ctx = NULL;

struct ApiHandle {
	CAccApi* Acc;
	CStreamApi* Stream;
	CStreamHandler* StreamHandler;
};
typedef std::list<ApiHandle*> ApiHandleList;
ApiHandleList g_HandleList;

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
	ApiHandleList::iterator ptr = g_HandleList.begin();
	for (; ptr != g_HandleList.end(); ++ptr) {
		CAccApi* pApi = (*ptr)->Acc;
		int rc = pApi->Disconnect();
		delete pApi;
		CStreamApi* pStream = (*ptr)->Stream;
		if (pStream)
			pStream->Disconnect();
		delete pStream;
		delete (*ptr)->StreamHandler;
		delete *ptr;
	}
	g_HandleList.clear();

	//if (g_ctx) {
	//	zmq_ctx_term(g_ctx);
	//}

	return 0;
}


extern "C"
RC_HANDLE RC_Connect(const char * sip, int port, const char * un, const char * pass)
{
	ApiHandle* pHandle = new ApiHandle();
	memset(pHandle, 0, sizeof(ApiHandle));
	pHandle->Acc = new CAccApi(g_ctx);
	if (pHandle->Acc->Connect(sip, port, un, pass)) {
		StreamProcess sp;
		int rc = pHandle->Acc->GetStreamServer(&sp);
		if (rc == 0) {
			pHandle->StreamHandler = new CStreamHandler();
			pHandle->Stream = new CStreamApi(*pHandle->StreamHandler, sp.Index, CLIENT_TYPE);
			bool br = pHandle->Stream->Connect(sp.StreamIP, sp.Port);
			if (br) {
				g_HandleList.push_back(pHandle);
				return pHandle;
			}
		}
	}

	delete pHandle->Acc;
	delete pHandle->Stream;
	delete pHandle->StreamHandler;
	return NULL;
}

extern "C"
int RC_Disconnect(RC_HANDLE api)
{
	ApiHandle* pHandle = (ApiHandle*)api;
	if (pHandle) {
		CAccApi* pApi = pHandle->Acc;
		int rc = pApi->Disconnect();
		delete pApi;
		CStreamApi* pStream = pHandle->Stream;
		if (pStream)
			pStream->Disconnect();
		delete pStream;
		return 0;
	}
	return -1;
}

extern "C"
int RC_ListDevices(RC_HANDLE api, DeviceInfo * list, int list_len)
{
	ApiHandle* pHandle = (ApiHandle*)api;
	if (pHandle && pHandle->Acc)
		return pHandle->Acc->ListDevices(list, list_len);
	return -1;
}

extern "C"
int RC_ListUsers(RC_HANDLE api, UserInfo * list, int list_len)
{
	ApiHandle* pHandle = (ApiHandle*)api;
	if (!pHandle)
		return -1;

	CAccApi* pApi = pHandle->Acc;
	if (pApi)
		return pApi->ListUsers(list, list_len);
	return -1;
}
extern "C"
int RC_SetStreamCallback(RC_HANDLE api, stream_data_callback data, stream_event_callback evt, void* prv)
{
	ApiHandle* pHandle = (ApiHandle*)api;
	if (!pHandle)
		return -1;
	if (pHandle->StreamHandler)
		pHandle->StreamHandler->SetCallbacks(data, evt, prv);

	return -1;
}

extern "C"
int RC_StreamSend(RC_HANDLE api, RC_CHANNEL channel, void * buf, size_t len)
{
	ApiHandle* pHandle = (ApiHandle*)api;
	if (!pHandle)
		return -1;

	CStreamApi* pStream = pHandle->Stream;
	if (!pStream)
		return -1;

	int rc = pStream->SendData(channel, buf, len);
	return rc;
}

extern "C"
RC_CHANNEL RC_ConnectSerial(RC_HANDLE api, const char * sn, const SerialInfo* info)
{
	ApiHandle* pHandle = (ApiHandle*)api;
	if (!pHandle)
		return -1;

	CAccApi* pApi = pHandle->Acc;
	if (pApi) {
		ConnectionInfo ci;
		ci.Type = CT_SERIAL;
		memcpy(&ci.Serial, info, sizeof(SerialInfo));
		memcpy(ci.DevSN, sn, RC_MAX_SN_LEN);

		return pApi->CreateConnection(&ci);
	}
	return -1;
}

extern "C"
RC_CHANNEL RC_ConnectTCPC(RC_HANDLE api, const char * sn, const  TCPClientInfo* info)
{
	ApiHandle* pHandle = (ApiHandle*)api;
	if (!pHandle)
		return -1;

	CAccApi* pApi = pHandle->Acc;
	if (pApi) {
		ConnectionInfo ci;
		ci.Type = CT_TCPC;
		memcpy(&ci.TCPClient, info, sizeof(TCPClientInfo));
		memcpy(ci.DevSN, sn, RC_MAX_SN_LEN);

		return pApi->CreateConnection(&ci);
	}
	return -1;
}

extern "C"
RC_CHANNEL RC_ConnectUDP(RC_HANDLE api, const char * sn, const UDPInfo* info)
{
	ApiHandle* pHandle = (ApiHandle*)api;
	if (!pHandle)
		return -1;

	CAccApi* pApi = pHandle->Acc;
	if (pApi) {
		ConnectionInfo ci;
		ci.Type = CT_UDP;
		memcpy(&ci.Serial, info, sizeof(UDPInfo));
		memcpy(ci.DevSN, sn, RC_MAX_SN_LEN);

		return pApi->CreateConnection(&ci);
	}
	return -1;
}

extern "C"
int RC_CloseChannel(RC_HANDLE api, RC_CHANNEL channel)
{
	ApiHandle* pHandle = (ApiHandle*)api;
	if (!pHandle)
		return -1;

	CAccApi* pApi = pHandle->Acc;
	if (pApi)
		return pApi->DestroyConnection(channel);
	return -1;
}