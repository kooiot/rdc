
#pragma once

#ifndef _REMOTE_CONNECTIOR_API_H_
#define _REMOTE_CONNECTIOR_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef REMOTECONNECTORAPI_EXPORTS
#define REMOTECONNECTORAPI_API __declspec(dllexport)
#else
#define REMOTECONNECTORAPI_API __declspec(dllimport)
#endif

#define ACC_ID_LEN 32
#define ACC_NAME_LEN 64
#define ACC_DESC_LEN 128
#define ACC_IP_LEN 128
#define ACC_MAX_USER_CONNECTION 16

	struct DeviceInfo
	{
		char ID[ACC_ID_LEN];
		char Name[ACC_NAME_LEN];
		char Desc[ACC_DESC_LEN];
		bool IsOnline;
		int LastUpdate;
	};

	struct ConnectionInfo
	{
		char Name[ACC_NAME_LEN];
		char Desc[ACC_DESC_LEN];
		char DevID[ACC_ID_LEN];
	};
	struct UserInfo
	{
		char ID[ACC_ID_LEN];
		char Name[ACC_NAME_LEN];
		char Desc[ACC_DESC_LEN];
		char IP[ACC_IP_LEN];
		bool IsOnline;
		ConnectionInfo Connections[ACC_MAX_USER_CONNECTION];
	};

	struct IPInfo {
		char sip[ACC_IP_LEN];
		int port;
	};
	struct TCPClientInfo
	{
		IPInfo local;
		IPInfo server;
	};

	//struct TCPServerInfo
	//{
	//	IPInfo local;
	//	IPInfo bind;
	//};

	struct UDPInfo
	{
		IPInfo local;
		IPInfo bind;
		IPInfo server;
	};

	struct SerialInfo
	{
		char local[ACC_NAME_LEN]; // Local Serial Port Name (COM1)
		char dev[ACC_NAME_LEN]; // Remote Serial Port Name (/dev/ttyS1)
		int baudrate;
		int bytesize;
		int parity;
		int stopbits;
		int flowcontrol;
	};

	typedef void* ACC_API_HANDLE;

	REMOTECONNECTORAPI_API ACC_API_HANDLE Connect(const char* sip, int port, const char* un, const char* pass);
	REMOTECONNECTORAPI_API int Disconnect(ACC_API_HANDLE Api);

	REMOTECONNECTORAPI_API int ListDevices(ACC_API_HANDLE Api, DeviceInfo* list, int list_len);
	REMOTECONNECTORAPI_API int ListUsers(ACC_API_HANDLE Api, UserInfo* list, int list_len);
	REMOTECONNECTORAPI_API int ConnectSerial(ACC_API_HANDLE Api, const char*  id, const char*  devid, const SerialInfo& info);
	REMOTECONNECTORAPI_API int CloseSerial(ACC_API_HANDLE Api, const char*  id);
	REMOTECONNECTORAPI_API int ConnectTCPC(ACC_API_HANDLE Api, const char*  id, const char*  devid, const TCPClientInfo& info);
	REMOTECONNECTORAPI_API int CloseTCPC(ACC_API_HANDLE Api, const char*  id);
	REMOTECONNECTORAPI_API int ConnectUDP(ACC_API_HANDLE Api, const char*  id, const char*  devid, const UDPInfo& info);
	REMOTECONNECTORAPI_API int CloseUDP(ACC_API_HANDLE Api, const char*  id);

#ifdef __cplusplus
}
#endif

#endif 
