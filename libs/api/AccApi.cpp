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

int CAccApi::SendHeartbeat()
{
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "HEARTBEAT";
	packet.data = "NOW";

	int rc = SendRequest(m_Socket, packet, [](const KZPacket& data) {
		if (data.data == std::string(S_SUCCESS))
			return 0;
		return -1;
	});

	return rc;
}

int CAccApi::AddDevice(const DeviceInfo * info)
{
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "HEARTBEAT";
	packet.data = "NOW";

	int rc = SendRequest(m_Socket, packet, [](const KZPacket& data) {
		if (data.data == std::string(S_SUCCESS))
			return 0;
		return -1;
	});

	return rc;
}

int CAccApi::ModifyDevice(const DeviceInfo * info)
{
	return 0;
}

int CAccApi::DeleteDevice(const char * sn)
{
	return 0;
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

int CAccApi::AddUser(const UserInfo * info)
{
	return 0;
}

int CAccApi::ModifyUser(const UserInfo * info)
{
	return 0;
}

int CAccApi::DeleteUser(const char * id)
{
	return 0;
}

int CAccApi::ListUsers(UserInfo * list, int list_len)
{
	return 0;
}

int CAccApi::Allow(const char * id, const char * devsn, time_t valid_time)
{
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "ALLOW";
	AllowInfo info;
	memcpy(info.ID, id, RC_MAX_ID_LEN);
	memcpy(info.DevSN, devsn, RC_MAX_SN_LEN);
	info.ValidTime = valid_time;
	packet.data = std::string((char*)&info, sizeof(AllowInfo));

	int rc = SendRequest(m_Socket, packet, [](const KZPacket& data) {
		if (data.data == std::string(S_SUCCESS))
			return 0;
		return -1;
	});

	return rc;
}

int CAccApi::Deny(const char * id, const char * devsn)
{
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "DENY";
	DenyInfo info;
	memcpy(info.ID, id, RC_MAX_ID_LEN);
	memcpy(info.DevSN, devsn, RC_MAX_SN_LEN);
	packet.data = std::string((char*)&info, sizeof(AllowInfo));

	int rc = SendRequest(m_Socket, packet, [](const KZPacket& data) {
		if (data.data == std::string(S_SUCCESS))
			return 0;
		return -1;
	});

	return rc;
}

int CAccApi::CreateConnection(const ConnectionInfo * info)
{
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "CREATE";
	packet.data = std::string((char*)info, sizeof(AllowInfo));

	int rc = SendRequest(m_Socket, packet, [](const KZPacket& data) {
		return atoi(data.data.c_str());
	});

	return rc;
}

int CAccApi::DestroyConnection(int index)
{
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "DESTROY";
	std::stringstream ss;
	ss << index;
	packet.data = ss.str();

	int rc = SendRequest(m_Socket, packet, [](const KZPacket& data) {
		if (data.data == std::string(S_SUCCESS))
			return 0;
		return -1;
	});

	return rc;
	return 0;
}

