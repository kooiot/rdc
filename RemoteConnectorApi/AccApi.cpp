#include "stdafx.h"
#include "AccApi.h"

CAccApi::CAccApi()
{
}

CAccApi::~CAccApi()
{
}

bool CAccApi::Connect(const char * sip, int port, const char * un, const char * pass)
{
	return false;
}

int CAccApi::Disconnect()
{
	return 0;
}

int CAccApi::ListDevices(DeviceInfo * list, int list_len)
{
	return 0;
}

int CAccApi::ListUsers(UserInfo * list, int list_len)
{
	return 0;
}

int CAccApi::ConnectSerial(const char * id, const char * devid, const SerialInfo & info)
{
	return 0;
}

int CAccApi::CloseSerial(const char * id)
{
	return 0;
}

int CAccApi::ConnectTCPC(const char * id, const char * devid, const TCPClientInfo & info)
{
	return 0;
}

int CAccApi::CloseTCPC(const char * id)
{
	return 0;
}

int CAccApi::ConnectUDP(const char * id, const char * devid, const UDPInfo & info)
{
	return 0;
}

int CAccApi::CloseUDP(const char * id)
{
	return 0;
}
