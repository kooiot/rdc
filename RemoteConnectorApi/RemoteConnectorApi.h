
#pragma once

#ifndef _REMOTE_CONNECTIOR_API_H_
#define _REMOTE_CONNECTIOR_API_H_

#include <DataDefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef REMOTECONNECTORAPI_EXPORTS
#define REMOTECONNECTORAPI_API __declspec(dllexport)
#else
#define REMOTECONNECTORAPI_API __declspec(dllimport)
#endif

	typedef void* RC_HANDLE;
	typedef int RC_CONNECT_HANDLE;

	REMOTECONNECTORAPI_API int RC_Init();
	REMOTECONNECTORAPI_API int RC_Close();

	REMOTECONNECTORAPI_API RC_HANDLE RC_Connect(const char* sip, int port, const char* un, const char* pass);
	REMOTECONNECTORAPI_API int RC_Disconnect(RC_HANDLE Api);

	REMOTECONNECTORAPI_API int RC_ListDevices(RC_HANDLE Api, DeviceInfo* list, int list_len);
	REMOTECONNECTORAPI_API int RC_ListUsers(RC_HANDLE Api, UserInfo* list, int list_len);
	REMOTECONNECTORAPI_API RC_CONNECT_HANDLE RC_ConnectSerial(RC_HANDLE Api, const char*  devid, const SerialInfo* info);
	REMOTECONNECTORAPI_API int RC_CloseSerial(RC_HANDLE Api, RC_CONNECT_HANDLE conn);
	REMOTECONNECTORAPI_API RC_CONNECT_HANDLE RC_ConnectTCPC(RC_HANDLE Api, const char*  devid, const TCPClientInfo* info);
	REMOTECONNECTORAPI_API int RC_CloseTCPC(RC_HANDLE Api, RC_CONNECT_HANDLE conn);
	REMOTECONNECTORAPI_API RC_CONNECT_HANDLE RC_ConnectUDP(RC_HANDLE Api, const char*  devid, const UDPInfo* info);
	REMOTECONNECTORAPI_API int RC_CloseUDP(RC_HANDLE Api, RC_CONNECT_HANDLE conn);

#ifdef __cplusplus
}
#endif

#endif 
