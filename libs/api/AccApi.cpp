#include "AccApi.h"
#include <sstream>
#include <zmq.h>
#include <cassert>
#include <koo_zmq_helpers.h>
#include <koo_string_util.h>


#define RETURN_MSG_DATA_RC(data) \
	if (!isdigit(*(char*)zmq_msg_data((zmq_msg_t*)&data.data))) \
		return -1; \
	return atoi((char*)zmq_msg_data((zmq_msg_t*)&data.data));

int SendRequest(void* socket, KZPacket& packet, std::function< int(KZPacket&)> cb = nullptr)
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
	int timeo = 5 * 1000;
	int rc = zmq_setsockopt(m_Socket, ZMQ_RCVTIMEO, &timeo, sizeof(int));
	timeo = 0;
	rc = zmq_setsockopt(m_Socket, ZMQ_LINGER, &timeo, sizeof(int));
	rc = zmq_connect(m_Socket, ss.str().c_str());
	assert(rc != -1);

	KZPacket login;
	login.id = un;
	login.cmd = "LOGIN";
	login.SetStr(pass);

	rc = SendRequest(m_Socket, login, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});
		
	return rc == 0;
}

int CAccApi::Disconnect()
{
	KZPacket logout;
	logout.id = m_ID;
	logout.cmd = "LOGOUT";
	logout.SetStr("LOGOUT");

	int rc = SendRequest(m_Socket, logout);
	zmq_close(m_Socket);
	m_Socket = NULL;

	return rc;
}

int CAccApi::GetStreamServer(StreamProcess * info)
{
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "STREAM";
	packet.SetStr("GetStreamServer");

	int rc = SendRequest(m_Socket, packet, [info](KZPacket& data) {
		memcpy(info, data.GetData(), sizeof(StreamProcess));
		return 0;
	});

	return rc;
}

int CAccApi::SendHeartbeat()
{
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "HEARTBEAT";
	packet.SetStr("NOW");

	int rc = SendRequest(m_Socket, packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
}

int CAccApi::AddDevice(const DeviceInfo * info)
{
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "ADD_DEV";
	packet.SetData(info, sizeof(DeviceInfo));

	int rc = SendRequest(m_Socket, packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
}

int CAccApi::ModifyDevice(const DeviceInfo * info)
{
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "MOD_DEV";
	packet.SetData(info, sizeof(DeviceInfo));

	int rc = SendRequest(m_Socket, packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
}

int CAccApi::DeleteDevice(const char * sn)
{
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "DEL_DEV";
	packet.SetStr(sn);

	int rc = SendRequest(m_Socket, packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
}

int CAccApi::ListDevices(DeviceInfo * list, int list_len, bool only_online)
{
	int i = 0;
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "LIST_DEV";
	packet.SetStr(only_online ? "ONLINE" : "ALL");
	auto devs = new std::vector<std::string>();
	int rc = SendRequest(m_Socket, packet, [devs](KZPacket& data) {
		k_str_split(data.GetStr(), *devs, "|");
		return 0;
	});
	if (rc == 0) {
		for (; i < list_len && i < devs->size(); ++i) {
			rc = GetDeviceInfo((*devs)[i].c_str(), &list[i]);
			if (rc != 0)
				break;
		}
	}
	delete devs;
	return i;
}

int CAccApi::GetDeviceInfo(const char * sn, DeviceInfo * info)
{
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "DEV_INFO";
	packet.SetStr(sn);
	int rc = SendRequest(m_Socket, packet, [info](KZPacket& data) {
		if (zmq_msg_size(&data.data) == sizeof(DeviceInfo)) {
			memcpy(info, zmq_msg_data(&data.data), sizeof(DeviceInfo));
			return 0;
		}
		return -1;
	});
	
	return rc;
}

int CAccApi::AddUser(const UserInfo * info)
{
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "ADD_CLINET";
	packet.SetData(info, sizeof(UserInfo));

	int rc = SendRequest(m_Socket, packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
}

int CAccApi::ModifyUser(const UserInfo * info)
{
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "MOD_CLINET";
	packet.SetData(info, sizeof(UserInfo));

	int rc = SendRequest(m_Socket, packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
}

int CAccApi::DeleteUser(const char * id)
{
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "DEL_CLINET";
	packet.SetStr(id);

	int rc = SendRequest(m_Socket, packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
}

int CAccApi::ListUsers(UserInfo * list, int list_len, bool only_online)
{
	int i = 0;
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "LIST_CLIENT";
	packet.SetStr(only_online ? "ONLINE" : "ALL");
	auto devs = new std::vector<std::string>();
	int rc = SendRequest(m_Socket, packet, [devs](KZPacket& data) {
		k_str_split(data.GetStr(), *devs, "|");
		return 0;
	});
	if (rc == 0) {
		for (; i < list_len && i < devs->size(); ++i) {
			rc = GetUserInfo((*devs)[i].c_str(), &list[i]);
			if (rc != 0)
				break;
		}
	}
	delete devs;
	return i;
}

int CAccApi::GetUserInfo(const char * id, UserInfo * info)
{
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "CLIENT_INFO";
	packet.SetStr(id);
	int rc = SendRequest(m_Socket, packet, [info](KZPacket& data) {
		if (zmq_msg_size(&data.data) == sizeof(UserInfo)) {
			memcpy(info, zmq_msg_data(&data.data), sizeof(UserInfo));
			return 0;
		}
		return -1;
	});

	return rc;
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
	packet.SetData(&info, sizeof(AllowInfo));

	int rc = SendRequest(m_Socket, packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
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
	packet.SetData(&info, sizeof(DenyInfo));

	int rc = SendRequest(m_Socket, packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
}

int CAccApi::CreateConnection(const ConnectionInfo * info)
{
	KZPacket packet;
	packet.id = m_ID;
	packet.cmd = "CREATE";
	packet.SetData(&info, sizeof(ConnectionInfo));

	int rc = SendRequest(m_Socket, packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
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
	packet.SetStr(ss.str().c_str());

	int rc = SendRequest(m_Socket, packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
	return 0;
}

