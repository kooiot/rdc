#include "stdafx.h"
#include "AccApi.h"
#include <sstream>
#include <zmq.h>
#include <cassert>
#include <koo_zmq_helpers.h>
#include <koo_string_util.h>

int SendRequest(void* socket, const KZPacket& packet, std::function< int(const KZPacket&)> cb = nullptr)
{
	int rc = koo_zmq_send_cmd(socket, packet);
	if (rc == 0) {
		KZPacket data;
		rc = koo_zmq_recv_cmd(socket, data);
		if (rc == 0) {
			assert(packet.cmd == data.cmd);
			return cb != nullptr ? cb(data) : rc;
		}
	}
	return rc;
}

CAccApi::CAccApi(void* ctx) : m_CTX(ctx), m_Socket(NULL)
{
}

CAccApi::~CAccApi()
{
}

bool CAccApi::Connect(const char * sip, int port, const char * un, const char * pass)
{
	m_ID = un;

	std::stringstream ss;
	ss << "tcp://" << sip << ":" << port;
	m_Socket = zmq_socket(m_CTX, ZMQ_REQ);
	assert(m_Socket);
	int rc = zmq_connect(m_Socket, ss.str().c_str());
	assert(rc != -1);
	int timeo = 5 * 1000;
	rc = zmq_setsockopt(m_Socket, ZMQ_RCVTIMEO, &timeo, sizeof(int));
	KZPacket login;
	login.id = un;
	login.cmd = "LOGIN";
	login.data = pass;

	rc = SendRequest(m_Socket, login, [](const KZPacket& data) {
		if (data.data == std::string(S_SUCCESS))
			return 0;
		return -1;
	});
		
	return rc == 0;
}

int CAccApi::Disconnect()
{
	KZPacket logout;
	logout.id = m_ID;
	logout.cmd = "LOGOUT";
	logout.data = "";

	int rc = SendRequest(m_Socket, logout);
	zmq_close(m_Socket);
	m_Socket = NULL;

	return rc;
}

int CAccApi::ListDevices(DeviceInfo * list, int list_len)
{
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "LIST_DEV";
	packet.data = "";
	auto devs = new std::vector<std::string>();
	int rc = SendRequest(m_Socket, packet, [devs](const KZPacket& data) {
		k_str_split(data.data, *devs, "|");
		return 0;
	});
	if (rc != 0)
		return rc;

	int i = 0;
	for (; i < list_len && i < devs->size(); ++i) {
		memset(&list[i], 0, sizeof(DeviceInfo));
		snprintf(list[i].SN, RC_MAX_ID_LEN, "%s", (*devs)[i].c_str());
	}
	delete devs;
	return i;
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
