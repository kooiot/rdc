#include "AccApi.h"
#include <sstream>
#include <zmq.h>
#include <cassert>
#include <koo_zmq_helpers.h>
#include <koo_string_util.h>


#define RETURN_MSG_DATA_RC(data) \
	if (!isdigit(*(char*)zmq_msg_data((zmq_msg_t*)&data.data)) \
		&& *(char*)zmq_msg_data((zmq_msg_t*)&data.data) != '-') \
		return -1; \
	return atoi((char*)zmq_msg_data((zmq_msg_t*)&data.data));

class AutoLock {
public:
	std::mutex & m_Lock;
	AutoLock(std::mutex& lock) : m_Lock(lock) {
		m_Lock.lock();
	}
	~AutoLock() {
		m_Lock.unlock();
	}
};
int CAccApi::SendRequest(KZPacket& packet, std::function< int(KZPacket&)> cb)
{
	AutoLock al(m_Lock);
	int rc = koo_zmq_send_cmd(m_Socket, packet);
	if (rc == 0) {
		KZPacket data;
		rc = koo_zmq_recv_cmd(m_Socket, data);
		if (rc == 0) {
			if (packet.cmd == data.cmd)
				return cb != nullptr ? cb(data) : rc;
			else if (data.cmd == "TIMEOUT")
				return -10000;
			else
				return -20000;
		}
	}
	if (rc != 0) {
		int err = errno;
		if (EAGAIN == errno) {
			// Recreate the socket.
			bool br = _Connect();
			assert(br);
		}
	}
	return rc;
}

CAccApi::CAccApi(void* ctx) : m_CTX(ctx), m_Socket(NULL), m_pHBThread(NULL), m_bStop(false)
{
}

CAccApi::~CAccApi()
{
}

bool CAccApi::Connect(const char * sip, int port, const char * un, const char * pass)
{
	m_Pass = pass;
	m_User = un;

	std::stringstream ss;
	ss << "tcp://" << sip << ":" << port;
	m_Conn = ss.str();

	return _Connect();
}

bool CAccApi::_Connect() 
{
	{
		AutoLock al(m_Lock);
		if (m_Socket != NULL) {
			zmq_close(m_Socket);
			m_Socket = NULL;
		}

		m_Socket = zmq_socket(m_CTX, ZMQ_REQ);
		assert(m_Socket);
		int timeo = 5 * 1000;
		int rc = zmq_setsockopt(m_Socket, ZMQ_RCVTIMEO, &timeo, sizeof(int));
		timeo = 0;
		rc = zmq_setsockopt(m_Socket, ZMQ_LINGER, &timeo, sizeof(int));
		rc = zmq_connect(m_Socket, m_Conn.c_str());
		assert(rc != -1);
	}

	KZPacket login;
	login.id = m_User;
	login.cmd = "LOGIN";
	login.SetStr(m_Pass.c_str());

	int rc = SendRequest(login, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	if (rc != 0) {
		zmq_close(m_Socket);
		m_Socket = NULL;
	}
	if (rc == 0) {
		m_pHBThread = new std::thread([this]() {
			int Count = 0;
			int MaxCount = (20 * 1000) / 500;
			while (!m_bStop) {
				Sleep(500);
				if (m_bStop)
					break;
				if (++Count >= MaxCount) {
					Count = 0;
					SendHeartbeat();
				}
			}
		});
	}
	return rc == 0;
}

int CAccApi::Disconnect()
{
	KZPacket logout;
	logout.id = m_User;
	logout.cmd = "LOGOUT";
	logout.SetStr("LOGOUT");

	int rc = SendRequest(logout);
	
	m_bStop = true;
	if (m_pHBThread && m_pHBThread->joinable())
		m_pHBThread->join();

	m_Lock.lock();
	zmq_close(m_Socket);
	m_Socket = NULL;
	m_Lock.unlock();
	return rc;
}

int CAccApi::GetStreamServer(StreamProcess * info)
{
	KZPacket packet;
	packet.id = m_User;
	packet.cmd = "STREAM";
	packet.SetStr("GetStreamServer");

	int rc = SendRequest(packet, [info](KZPacket& data) {
		memcpy(info, data.GetData(), sizeof(StreamProcess));
		return 0;
	});

	return rc;
}

int CAccApi::SendHeartbeat()
{
	KZPacket packet;
	packet.id = m_User;
	packet.cmd = "HEARTBEAT";
	packet.SetStr("NOW");

	int rc = SendRequest(packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
}

int CAccApi::AddDevice(const DeviceInfo * info)
{
	KZPacket packet;
	packet.id = m_User;
	packet.cmd = "ADD_DEV";
	packet.SetData(info, sizeof(DeviceInfo));

	int rc = SendRequest(packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
}

int CAccApi::ModifyDevice(const DeviceInfo * info)
{
	KZPacket packet;
	packet.id = m_User;
	packet.cmd = "MOD_DEV";
	packet.SetData(info, sizeof(DeviceInfo));

	int rc = SendRequest(packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
}

int CAccApi::DeleteDevice(const char * sn)
{
	KZPacket packet;
	packet.id = m_User;
	packet.cmd = "DEL_DEV";
	packet.SetStr(sn);

	int rc = SendRequest(packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
}

int CAccApi::ListDevices(DeviceInfo * list, int list_len, bool only_online)
{
	int i = 0;
	KZPacket packet;
	packet.id = m_User;
	packet.cmd = "LIST_DEV";
	packet.SetStr(only_online ? "ONLINE" : "ALL");
	auto devs = new std::vector<std::string>();
	int rc = SendRequest(packet, [devs](KZPacket& data) {
		std::string strList = data.GetStr();
		if (strList.length() > 2)
			k_str_split(strList, *devs, "|");
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
	packet.id = m_User;
	packet.cmd = "DEV_INFO";
	packet.SetStr(sn);
	int rc = SendRequest(packet, [info](KZPacket& data) {
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
	packet.id = m_User;
	packet.cmd = "ADD_CLIENT";
	packet.SetData(info, sizeof(UserInfo));

	int rc = SendRequest(packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
}

int CAccApi::ModifyUser(const UserInfo * info)
{
	KZPacket packet;
	packet.id = m_User;
	packet.cmd = "MOD_CLIENT";
	packet.SetData(info, sizeof(UserInfo));

	int rc = SendRequest(packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
}

int CAccApi::DeleteUser(const char * id)
{
	KZPacket packet;
	packet.id = m_User;
	packet.cmd = "DEL_CLIENT";
	packet.SetStr(id);

	int rc = SendRequest(packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
}

int CAccApi::ListUsers(UserInfo * list, int list_len, bool only_online)
{
	int i = 0;
	KZPacket packet;
	packet.id = m_User;
	packet.cmd = "LIST_CLIENT";
	packet.SetStr(only_online ? "ONLINE" : "ALL");
	auto devs = new std::vector<std::string>();
	int rc = SendRequest(packet, [devs](KZPacket& data) {
		std::string strList = data.GetStr();
		if (strList.length() > 2)
			k_str_split(strList, *devs, "|");
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
	packet.id = m_User;
	packet.cmd = "CLIENT_INFO";
	packet.SetStr(id);
	int rc = SendRequest(packet, [info](KZPacket& data) {
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
	packet.id = m_User;
	packet.cmd = "ALLOW";
	AllowInfo info;
	memcpy(info.ID, id, RC_MAX_ID_LEN);
	memcpy(info.DevSN, devsn, RC_MAX_SN_LEN);
	info.ValidTime = valid_time;
	packet.SetData(&info, sizeof(AllowInfo));

	int rc = SendRequest(packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
}

int CAccApi::Deny(const char * id, const char * devsn)
{
	KZPacket packet;
	packet.id = m_User;
	packet.cmd = "DENY";
	DenyInfo info;
	memcpy(info.ID, id, RC_MAX_ID_LEN);
	memcpy(info.DevSN, devsn, RC_MAX_SN_LEN);
	packet.SetData(&info, sizeof(DenyInfo));

	int rc = SendRequest(packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
}

int CAccApi::CreateConnection(const ConnectionInfo * info)
{
	KZPacket packet;
	packet.id = m_User;
	packet.cmd = "CREATE";
	packet.SetData(info, sizeof(ConnectionInfo));

	int rc = SendRequest(packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
}

int CAccApi::DestroyConnection(int index)
{
	KZPacket packet;
	packet.id = m_User;
	packet.cmd = "DESTROY";
	std::stringstream ss;
	ss << index;
	packet.SetStr(ss.str().c_str());

	int rc = SendRequest(packet, [](KZPacket& data) {
		RETURN_MSG_DATA_RC(data)
	});

	return rc;
	return 0;
}

