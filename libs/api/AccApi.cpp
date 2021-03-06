#include "AccApi.h"
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
int CAccApi::SendRequest(KZPacket& packet, std::function< int(KZPacket&)> cb)
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

	KZPacket login(m_User, "LOGIN");
	login.set("pass", m_Pass);

	int rc = SendRequest(login);

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
	KZPacket logout(m_User, "LOGOUT");

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
	KZPacket packet(m_User, "STREAM");

	int rc = SendRequest(packet, [info](KZPacket& data) {
		KOO_PARSE_JSON(*info, data.get("result"));
		return 0;
	});

	return rc;
}

int CAccApi::ListDevices(DeviceInfo * list, int list_len, bool only_online)
{
	KZPacket packet(m_User, "LIST_DEV");
	packet.set("type", only_online ? "ONLINE" : "ALL");

	auto devs = new std::list<std::string>();
	int rc = SendRequest(packet, [devs](KZPacket& data) {
		if( KOO_PARSE_JSON(*devs, data.get("result")) )
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

int CAccApi::GetDeviceInfo(const char * sn, DeviceInfo * info)
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
int CAccApi::SendHeartbeat()
{
	KZPacket packet(m_User, "HEARTBEAT");
	packet.set("timestamp", "");

	int rc = SendRequest(packet);

	return rc;
}

int CAccApi::CreateConnection(const ConnectionInfo * info)
{
	KZPacket packet(m_User, "CREATE");
	packet.set("info", KOO_GEN_JSON(*info));

	int rc = SendRequest(packet);
	return rc;
}

int CAccApi::DestroyConnection(int index)
{
	KZPacket packet(m_User, "DESTROY");
	packet.set("channel", index);

	int rc = SendRequest(packet);
	return rc;
}

int CAccApi::ListConnection(int* channels)
{
	for (int i = 0; i < RC_MAX_CONNECTION; ++i) {
		channels[i] = -1;
	}

	KZPacket packet(m_User, "LIST_CHANNELS");
	packet.set("user", m_User);

	int rc = SendRequest(packet, [channels](KZPacket& data) {
		const json list = data.get("result");
		int i = 0;
		for (auto & j : list) {
			channels[i++] = j;
		}
		return 0;
	});
	return rc;
}

