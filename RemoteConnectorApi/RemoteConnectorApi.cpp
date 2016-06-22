// RemoteConnectorApi.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "RemoteConnectorApi.h"
#include "AccApi.h"

extern "C"
ACC_API_HANDLE Connect(const char * sip, int port, const char * un, const char * pass)
{
	CAccApi* pApi = new CAccApi();
	if (pApi->Connect(sip, port, un, pass))
		return pApi;
	return NULL;
}

extern "C"
int Disconnect(ACC_API_HANDLE Api)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->Disconnect();
	return -1;
}

extern "C"
int ListDevices(ACC_API_HANDLE Api, DeviceInfo * list, int list_len)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->ListDevices(list, list_len);
	return -1;
}

extern "C"
int ListUsers(ACC_API_HANDLE Api, UserInfo * list, int list_len)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->ListUsers(list, list_len);
	return -1;
}

extern "C"
int ConnectSerial(ACC_API_HANDLE Api, const char * id, const char * devid, const SerialInfo & info)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->ConnectSerial(id, devid, info);
	return -1;
}

extern "C"
int CloseSerial(ACC_API_HANDLE Api, const char * id)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->CloseSerial(id);
	return -1;
}

extern "C"
int ConnectTCPC(ACC_API_HANDLE Api, const char * id, const char * devid, const TCPClientInfo & info)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->ConnectTCPC(id, devid, info);
	return -1;
}

extern "C"
int CloseTCPC(ACC_API_HANDLE Api, const char * id)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->CloseTCPC(id);
	return -1;
}

extern "C"
int ConnectUDP(ACC_API_HANDLE Api, const char * id, const char * devid, const UDPInfo & info)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->ConnectUDP(id, devid, info);
	return -1;
}

extern "C"
int CloseUDP(ACC_API_HANDLE Api, const char * id)
{
	CAccApi* pApi = (CAccApi*)Api;
	if (pApi)
		return pApi->CloseUDP(id);
	return -1;
}
