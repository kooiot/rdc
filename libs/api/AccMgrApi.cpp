#include "AccMgrApi.h"
#include <sstream>
#include <zmq.h>
#include <cassert>
#include <koo_zmq_helpers.h>
#include <koo_string_util.h>
#include "DataJson.h"

#ifdef RDC_LINUX_SYS
#define Sleep(x) usleep(x * 1000)
#endif

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
int CAccMgrApi::SendRequest(KZPacket& packet, std::function< int(KZPacket&)> cb)
{
	int rc = 0;
	{
		AutoLock al(m_Lock);
		rc = koo_zmq_send(m_Socket, packet);
		if (rc >= 0) {
			KZPacket data;
			rc = koo_zmq_recv(m_Socket, data);
			if (rc == 0) {
				if (packet.cmd() == data.cmd())
					if (cb != nullptr)
						return cb(data);
					else
						return data.get("result");
				else if (data.cmd() == "TIMEOUT")
					return -10000;
				else
					return -20000;
			}
		}
	}
	if (rc != 0) {
		int err = errno;
		if (EAGAIN == errno && packet.cmd() != "LOGIN") {
			// Recreate the socket.
			bool br = _Connect();
			assert(br);
		}
	}
	return rc;
}

CAccMgrApi::CAccMgrApi(void* ctx) : m_CTX(ctx), m_Socket(NULL)
{
}

CAccMgrApi::~CAccMgrApi()
{
}

bool CAccMgrApi::Connect(const char * sip, int port, const char * un, const char * pass)
{
	m_Pass = pass;
	m_User = un;

	std::stringstream ss;
	ss << "tcp://" << sip << ":" << port;
	m_Conn = ss.str();

	return _Connect();
}

bool CAccMgrApi::_Connect() 
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

	KZPacket login(m_User, "LOGIN");
	login.set("pass", m_Pass);

	int rc = SendRequest(login);

	if (rc != 0) {
		zmq_close(m_Socket);
		m_Socket = NULL;
	}
	
	return rc == 0;
}

int CAccMgrApi::Disconnect()
{
	KZPacket logout(m_User, "LOGOUT");

	int rc = SendRequest(logout);

	m_Lock.lock();
	zmq_close(m_Socket);
	m_Socket = NULL;
	m_Lock.unlock();
	return rc;
}

int CAccMgrApi::AddDevice(const DeviceInfo * info)
{
	KZPacket packet(m_User, "ADD_DEV");
	packet.set("info", KOO_GEN_JSON(*info));

	int rc = SendRequest(packet);

	return rc;
}

int CAccMgrApi::ModifyDevice(const DeviceInfo * info)
{
	KZPacket packet(m_User, "MOD_DEV");
	packet.set("info", KOO_GEN_JSON(*info));

	int rc = SendRequest(packet);

	return rc;
}

int CAccMgrApi::DeleteDevice(const char * sn)
{
	KZPacket packet(m_User, "DEL_DEV");
	packet.set("sn", sn);

	int rc = SendRequest(packet);

	return rc;
}

int CAccMgrApi::ListDevices(DeviceInfo * list, int list_len, bool only_online)
{
	KZPacket packet(m_User, "LIST_DEV");
	packet.set("type", only_online ? "ONLINE" : "ALL");

	auto devs = new std::list<std::string>();
	int rc = SendRequest(packet, [devs](KZPacket& data) {
		if (KOO_PARSE_JSON(*devs, data.get("result")))
			return 0;
		return -1;
	});

	int i = 0;
	if (rc == 0) {
		for (auto & dev : *devs) {
			rc = GetDeviceInfo(dev.c_str(), &list[i++]);
			if (rc != 0)
				break;
		}
	}
	delete devs;
	return i;
}

int CAccMgrApi::GetDeviceInfo(const char * sn, DeviceInfo * info)
{
	KZPacket packet(m_User, "DEV_INFO");
	packet.set("sn", sn);

	int rc = SendRequest(packet, [info](KZPacket& data) {
		if (KOO_PARSE_JSON(*info, data.get("result")))
			return 0;
		return -1;
	});
	
	return rc;
}

int CAccMgrApi::AddUser(const UserInfo * info)
{
	KZPacket packet(m_User, "ADD_CLIENT");
	packet.set("info", KOO_GEN_JSON(*info));

	int rc = SendRequest(packet);
	return rc;
}

int CAccMgrApi::ModifyUser(const UserInfo * info)
{
	KZPacket packet(m_User, "MOD_CLIENT");
	packet.set("info", KOO_GEN_JSON(*info));

	int rc = SendRequest(packet);

	return rc;
}

int CAccMgrApi::DeleteUser(const char * id)
{
	KZPacket packet(m_User, "DEL_CLIENT");
	packet.set("client", id);

	int rc = SendRequest(packet);
	return rc;
}

int CAccMgrApi::ListUsers(UserInfo * list, int list_len, bool only_online)
{
	KZPacket packet(m_User, "LIST_CLIENT");
	packet.set("type", only_online ? "ONLINE" : "ALL");

	auto users = new std::list<std::string>();
	int rc = SendRequest(packet, [users](KZPacket& data) {
		if (KOO_PARSE_JSON(*users, data.get("result")))
			return 0;
		return -1;
	});
	int i = 0;
	if (rc == 0) {
		for (auto & user : *users) {
			rc = GetUserInfo(user.c_str(), &list[i++]);
			if (rc != 0)
				break;
		}
	}
	delete users;
	return i;
}

int CAccMgrApi::GetUserInfo(const char * id, UserInfo * info)
{
	KZPacket packet(m_User, "CLIENT_INFO");
	packet.set("client", id);
	int rc = SendRequest(packet, [info](KZPacket& data) {
		if (KOO_PARSE_JSON(*info, data.get("result")))
			return 0;
		return -1;
	});

	return rc;
}

int CAccMgrApi::AddGroup(const GroupInfo * info)
{
	KZPacket packet(m_User, "ADD_GROUP");
	packet.set("info", KOO_GEN_JSON(*info));

	int rc = SendRequest(packet);
	return rc;
}

int CAccMgrApi::ModifyGroup(const GroupInfo * info)
{
	KZPacket packet(m_User, "MOD_GROUP");
	packet.set("info", KOO_GEN_JSON(*info));

	int rc = SendRequest(packet);

	return rc;
}

int CAccMgrApi::DeleteGroup(const char * name)
{
	KZPacket packet(m_User, "DEL_GROUP");
	packet.set("group", name);

	int rc = SendRequest(packet);
	return rc;
}

int CAccMgrApi::ListGroups(GroupInfo * list, int list_len)
{
	KZPacket packet(m_User, "LIST_GROUP");

	auto groups = new std::list<std::string>();
	int rc = SendRequest(packet, [groups](KZPacket& data) {
		if (KOO_PARSE_JSON(*groups, data.get("result")))
			return 0;
		return -1;
	});
	int i = 0;
	if (rc == 0) {
		for (auto & g : *groups) {
			rc = GetGroupInfo(g.c_str(), &list[i++]);
			if (rc != 0)
				break;
		}
	}
	delete groups;
	return i;
}

int CAccMgrApi::GetGroupInfo(const char * name, GroupInfo * info)
{
	KZPacket packet(m_User, "GROUP_INFO");
	packet.set("group", name);
	int rc = SendRequest(packet, [info](KZPacket& data) {
		if (KOO_PARSE_JSON(*info, data.get("result")))
			return 0;
		return -1;
	});

	return rc;
}

int CAccMgrApi::ListGroupDevices(int group, int * list, int list_len)
{
	KZPacket packet(m_User, "LIST_GROUP_DEVICES");
	packet.set("group", group);

	auto devices = new std::list<int>();
	int rc = SendRequest(packet, [devices](KZPacket& data) {
		if (KOO_PARSE_JSON(*devices, data.get("result")))
			return 0;
		return -1;
	});
	int i = 0;
	if (rc == 0) {
		for (auto & dev : *devices) {
			list[i++] = dev;
		}
	}
	delete devices;
	return i;
}

int CAccMgrApi::AddDeviceToGroup(int group, int device)
{
	KZPacket packet(m_User, "ADD_DEV_TO_GROUP");
	packet.set("group", group);
	packet.set("device", device);

	int rc = SendRequest(packet);
	return rc;
}

int CAccMgrApi::RemoveDeviceToGroup(int group, int device)
{
	KZPacket packet(m_User, "REMOVE_DEV_TO_GROUP");
	packet.set("group", group);
	packet.set("device", device);

	int rc = SendRequest(packet);
	return rc;
}
