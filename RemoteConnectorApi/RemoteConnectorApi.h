
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
	typedef int RC_CHANNEL;
	typedef void(__stdcall *stream_data_callback)(RC_CHANNEL channel, const unsigned char* buf, size_t len, void* prv);
	typedef void(__stdcall *stream_event_callback)(StreamEvent evt, void* prv);

	REMOTECONNECTORAPI_API int RC_Init();
	REMOTECONNECTORAPI_API int RC_Close();

	REMOTECONNECTORAPI_API RC_HANDLE RC_Connect(const char* sip, int port, const char* un, const char* pass);
	REMOTECONNECTORAPI_API int RC_Disconnect(RC_HANDLE api);

	// Return device count (-1 for error)
	REMOTECONNECTORAPI_API int RC_ListDevices(RC_HANDLE api, DeviceInfo* list, int list_len);
	// Return user count (-1 for error)
	REMOTECONNECTORAPI_API int RC_ListUsers(RC_HANDLE api, UserInfo* list, int list_len);

	REMOTECONNECTORAPI_API int RC_SetStreamCallback(RC_HANDLE api, stream_data_callback data, stream_event_callback evt, void* prv);
	// Return sent bytes (-1 for error
	REMOTECONNECTORAPI_API int RC_StreamSend(RC_HANDLE api, RC_CHANNEL channel, const unsigned char* buf, size_t len);

	REMOTECONNECTORAPI_API RC_CHANNEL RC_ConnectSerial(RC_HANDLE api, const char*  devid, const SerialInfo* info);
	REMOTECONNECTORAPI_API RC_CHANNEL RC_ConnectTCPC(RC_HANDLE api, const char*  devid, const TCPClientInfo* info);
	REMOTECONNECTORAPI_API RC_CHANNEL RC_ConnectUDP(RC_HANDLE api, const char*  devid, const UDPInfo* info);
	REMOTECONNECTORAPI_API int RC_CloseChannel(RC_HANDLE api, RC_CHANNEL channel);

#ifdef __cplusplus
}
#endif

#endif 
