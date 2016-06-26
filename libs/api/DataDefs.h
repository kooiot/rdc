#pragma once

#ifndef _REMOTE_CONNECTIOR_DATA_DEFS_H_
#define _REMOTE_CONNECTIOR_DATA_DEFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef REMOTECONNECTORAPI_EXPORTS
#define REMOTECONNECTORAPI_API __declspec(dllexport)
#else
#define REMOTECONNECTORAPI_API __declspec(dllimport)
#endif

#define RC_MAX_ID_LEN 32
#define RC_MAX_NAME_LEN 64
#define RC_MAX_DESC_LEN 128
#define RC_MAX_PASSWD_LEN 32
#define RC_MAX_EMAIL_LEN 128
#define RC_MAX_PHONE_LEN 128
#define RC_MAX_SN_LEN 128
#define RC_MAX_IP_LEN 128

#define RC_MAX_MAX_USER_CONNECTION 16

	struct DeviceInfo
	{
		int Index;
		char SN[RC_MAX_SN_LEN];
		char Name[RC_MAX_NAME_LEN];
		char Desc[RC_MAX_DESC_LEN];
		time_t CreateTime; // In UTC
		time_t ValidTime; // In UTC
	};

	struct ConnectionInfo
	{
		char Name[RC_MAX_NAME_LEN];
		char Desc[RC_MAX_DESC_LEN];
		char DevSN[RC_MAX_SN_LEN];
	};
	struct UserInfo
	{
		int Index;
		int Level;
		char ID[RC_MAX_ID_LEN];
		char Name[RC_MAX_NAME_LEN];
		char Desc[RC_MAX_DESC_LEN];
		char Passwd[RC_MAX_PASSWD_LEN];
		char Email[RC_MAX_EMAIL_LEN];
		char Phone[RC_MAX_PHONE_LEN];
		time_t CreateTime; // In UTC
		time_t ValidTime; // In UTC
		ConnectionInfo Connections[RC_MAX_MAX_USER_CONNECTION];
	};

	struct IPInfo {
		char sip[RC_MAX_IP_LEN];
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
		char local[RC_MAX_NAME_LEN]; // Local Serial Port Name (COM1)
		char dev[RC_MAX_NAME_LEN]; // Remote Serial Port Name (/dev/ttyS1)
		int baudrate;
		int bytesize;
		int parity;
		int stopbits;
		int flowcontrol;
	};

	struct StreamProcess {
		int Counter;
		char StreamIP[128];
		int Port;
	};

#define S_SUCCESS	"__SUCCESS__"
#define S_FAILED	"__FAILED__"

#ifdef __cplusplus
}
#endif

#endif 