#pragma once

#ifndef _RDC_DATA_DEFS_H_
#define _RDC_DATA_DEFS_H_

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RC_MAX_ID_LEN 32
#define RC_MAX_NAME_LEN 64
#define RC_MAX_DESC_LEN 128
#define RC_MAX_PASSWD_LEN 32
#define RC_MAX_EMAIL_LEN 128
#define RC_MAX_PHONE_LEN 128
#define RC_MAX_SN_LEN 128
#define RC_MAX_IP_LEN 128

#define RC_MAX_ONLINE_DEVICE 2048
#define RC_MAX_ONLINE_USER 512

// Stream Server MAXs
#define RC_MAX_CONNECTION 16
#define RC_STREAM_SERVER_ID_BASE 10000
#define RC_MAX_STREAM_SERVER_COUNT 128
#define RC_MAX_CONNECTION_PER_SERVER 128

#define MAPPER_TYPE 1
#define CLIENT_TYPE 2

	typedef void* RC_HANDLE;
	typedef int RC_CHANNEL;

	struct IPInfo {
		char sip[RC_MAX_IP_LEN];
		int port;
	};
	struct TCPClientInfo
	{
		IPInfo local;
		IPInfo server;
	};

	struct TCPServerInfo
	{
		IPInfo local;
	};

	struct UDPInfo
	{
		IPInfo local;
		IPInfo server;
	};

	/*!
	* Enumeration defines the possible bytesizes for the serial port.
	*/
	typedef enum {
		fivebits = 5,
		sixbits = 6,
		sevenbits = 7,
		eightbits = 8
	} bytesize;

	/*!
	* Enumeration defines the possible parity types for the serial port.
	*/
	typedef enum {
		parity_none = 0,
		parity_odd = 1,
		parity_even = 2,
		parity_mark = 3,
		parity_space = 4
	} parity;

	/*!
	* Enumeration defines the possible stopbit types for the serial port.
	*/
	typedef enum {
		stopbits_one = 1,
		stopbits_two = 2,
		stopbits_one_point_five
	} stopbits;

	/*!
	* Enumeration defines the possible flowcontrol types for the serial port.
	*/
	typedef enum {
		flowcontrol_none = 0,
		flowcontrol_software,
		flowcontrol_hardware
	} flowcontrol;

	struct SerialInfo
	{
		char local[RC_MAX_NAME_LEN]; // Local Serial Port Name (COM1)
		char dev[RC_MAX_NAME_LEN]; // Remote Serial Port Name (/dev/ttyS1)
		int baudrate;
		bytesize bytesize;
		parity parity;
		stopbits stopbits;
		flowcontrol flowcontrol;
	};
	struct PluginInfo
	{
		char Name[16];
		char Data[512];
	};
	enum ConnectionType {
		CT_TEST = 0,
		CT_SERIAL,
		CT_TCPC,
		CT_UDP,
		CT_TCPS,
		CT_PLUGIN,
		CT_COUNT,
	};

	struct ConnectionInfo {
		ConnectionType Type;
		RC_CHANNEL Channel; // 客户端Channel(客户端唯一，设备端不能根据这个判断唯一性)
		char DevSN[RC_MAX_SN_LEN];
		union {
			SerialInfo Serial;
			TCPClientInfo TCPClient;
			UDPInfo UDP;
			TCPServerInfo TCPServer;
			PluginInfo Plugin;
		};
	};

	struct DeviceInfo
	{
		int Index;
		char SN[RC_MAX_SN_LEN];
		char Name[RC_MAX_NAME_LEN];
		char Desc[RC_MAX_DESC_LEN];
		time_t CreateTime; // In UTC
		time_t ValidTime; // In UTC
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
		ConnectionInfo Connections[RC_MAX_CONNECTION];
	};

	struct AllowInfo {
		char ID[RC_MAX_ID_LEN];
		char DevSN[RC_MAX_SN_LEN];
		time_t ValidTime; // In UTC
	};
	struct DenyInfo {
		char ID[RC_MAX_ID_LEN];
		char DevSN[RC_MAX_SN_LEN];
	};

	struct StreamProcess {
		int Index;
		int Mask; // For connection info
		char StreamIP[128];
		int Port;
		void* __inner;
	};

#define S_SUCCESS	"__SUCCESS__"
#define S_FAILED	"__FAILED__"

	enum StreamEvent {
		SE_CONNECT,
		SE_DISCONNECT,
		SE_CHANNEL_OPENED,
		SE_CHANNEL_CLOSED,
		SE_CHANNEL_NOT_SUPPORT,
		SE_CHANNEL_OPEN_FAILED,
		SE_CHANNEL_READ_ERROR,
		SE_CLOSE,
		SE_TIMEOUT,
	};
	struct StreamEventPacket {
		StreamEvent event;
		int channel;
		char msg[128];
	};
#ifdef __cplusplus
}
#endif

#endif //_RDC_DATA_DEFS_H_
