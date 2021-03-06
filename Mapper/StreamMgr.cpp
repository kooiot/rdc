#include "StreamMgr.h"
#include <cassert>
#include "SerialStream.h"
#include "TestStream.h"
#include "PluginStream.h"
#include "NetStream.h"

extern CPluginLoader g_PluginLoader;

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

StreamMgr::StreamMgr(uv_loop_t * uv_loop)
	: m_UVLoop(uv_loop), m_ClientHost(NULL), m_pThread(NULL), m_bAbort(false)
{
	memset(m_Peers, 0, sizeof(ENetPeer*) * RC_MAX_CONNECTION);
	memset(m_StreamServers, 0, sizeof(StreamProcess*) * RC_MAX_CONNECTION);
}


StreamMgr::~StreamMgr()
{
}

bool StreamMgr::Init()
{
	m_bAbort = false;
	m_ClientHost = enet_host_create(NULL, RC_MAX_CONNECTION, RC_MAX_CONNECTION + 1, 0, 0);
	if (m_ClientHost == NULL)
	{
		fprintf(stderr,
			"An error occurred while trying to create an ENet client host.\n");
		return false;
	}
	m_pThread = new std::thread([this]() {
		ENetEvent event;

		while (!m_bAbort) {
			if (enet_host_service(m_ClientHost, &event, 500) > 0)
			{
				switch (event.type)
				{
				case ENET_EVENT_TYPE_CONNECT:
					printf("Connection to %d:%d succeeded.\n",
						event.peer->address.host,
						event.peer->address.port);
					OnConnected(event.peer);
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					printf("A packet of length %llu was received from %ld on channel %u.\n",
						event.packet->dataLength,
						(long)event.peer->data,
						event.channelID);
					//putc('+', stderr);
					OnData(event.peer, event.channelID, event.packet->data, event.packet->dataLength);

					/* Clean up the packet now that we're done using it. */
					enet_packet_destroy(event.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					printf("ENet peer disconnected, count %ld event.data %d.\n", (long)event.peer->data, event.data);
					OnDisconnected(event.peer);
				}
			}
			ProcessPending();
		}
		enet_host_destroy(m_ClientHost);
	});

	m_PortMgr.Init();

	return true;
}

bool StreamMgr::Close()
{
	// TODO: Close all connections
	m_bAbort = true;
	if (m_pThread && m_pThread->joinable())
		m_pThread->join();
	return false;
}

bool StreamMgr::OnConnected(ENetPeer * peer)
{
	fprintf(stdout, "ENet Peer connected!.\n");

	AutoLock lock(m_Lock);

	std::map<ENetPeer*, std::list<IStreamPort*>>::iterator ptr = m_PendingPorts.find(peer);
	if (ptr == m_PendingPorts.end())
		return true;
	std::list<IStreamPort*>::iterator lptr = ptr->second.begin();
	for (; lptr != ptr->second.end(); ++lptr) {
		(*lptr)->Open();
	}

	m_PendingPorts.erase(peer);
	return true;
}

void StreamMgr::OnDisconnected(ENetPeer * peer)
{
	StreamProcess sp;
	int nIndex = -1;
	{
		AutoLock lock(m_Lock);
		for (int i = 0; i < RC_MAX_CONNECTION; ++i) {
			if (m_Peers[i] == peer) {
				nIndex = i;
				memcpy(&sp, m_StreamServers[i], sizeof(StreamProcess));
				break;
			}
		}
	}
	
	if (nIndex == -1) {
		return;
	}

	for (int i = 0; i < RC_MAX_CONNECTION; ++i) {
		Destroy(sp, i);
	}
}

bool StreamMgr::OnData(ENetPeer* peer, int channel, void * data, size_t len)
{
	if (channel == RC_MAX_CONNECTION) {
		return true;
	}
	AutoLock lock(m_Lock);

	std::pair<ENetPeer*, int> key = std::make_pair(peer, channel);
	PeerChannel2PortMap::iterator ptr = m_PeerChannel2Port.find(key);
	if (ptr == m_PeerChannel2Port.end()) {
		fprintf(stderr, "No StreamPort for enet peer channel.\n");
		return false;
	}
	IStreamPort* port = ptr->second;
	int rc = port->OnClientData(data, len);
	if (rc < 0)
		return false;
	return true;
}

//bool StreamMgr::OnEvent(StreamEvent event)
//{
//	return false;
//}

int StreamMgr::Create(const StreamProcess& StreamServer, const ConnectionInfo & info)
{
	AutoLock lock(m_Lock);
	int i = 0;
	// Search for same enet connection
	for (; i < RC_MAX_CONNECTION; ++i) {
		if (m_StreamServers[i] == NULL)
			continue;
		if (m_StreamServers[i]->Index == StreamServer.Index
			&& 0 == strcmp(m_StreamServers[i]->StreamIP, StreamServer.StreamIP)
			&& m_StreamServers[i]->Port == StreamServer.Port)
		{
			break;
		}
	}
	// Search for free enet peer
	if (i >= RC_MAX_CONNECTION) {
		i = 0;
		for (; i < RC_MAX_CONNECTION; ++i) {
			if (m_Peers[i] == NULL)
				break;
		}
	}
	if (i >= RC_MAX_CONNECTION) {
		fprintf(stderr, "Rearch max connection!!!.\n");
		return -1;
	}
	ENetPeer* peer = NULL;
	if (m_StreamServers[i] == NULL) // New Connection
	{
		ENetAddress address;

		enet_address_set_host(&address, StreamServer.StreamIP);
		address.port = StreamServer.Port;
		int nData = ((MAPPER_TYPE & 0xFFFF) << 16) + (StreamServer.Index & 0xFFFF);
		peer = enet_host_connect(m_ClientHost, &address, RC_MAX_CONNECTION + 1, nData);
		if (peer == NULL)
		{
			fprintf(stderr,
				"No available peers for initiating an ENet connection.\n");
			return -1;
		}
		long& count = *(long*)&peer->data;
		count = 1;
		m_StreamServers[i] = new StreamProcess(StreamServer);
		m_Peers[i] = peer;
	}
	else
	{
		// The connection is built and used by other channel.
		peer = m_Peers[i];
		long& count = *(long*)&peer->data;
		count++;
	}

	IStreamPort* pPort = NULL;
	StreamPortInfo spi;
	spi.Mgr = this;
	spi.ConnInfo = info;
	spi.Peer = peer;
	spi.Mask = StreamServer.Mask;
	switch (info.Type) {
	case CT_SERIAL:
		pPort = new SerialStream(spi);
		printf("Serial Port\n");
		break;
	case CT_TCPC:
	case CT_TCPS:
	case CT_UDP:
		pPort = new NetStream(m_PortMgr, spi);
		printf("UDP Port\n");
		break;
	case CT_PLUGIN:
		pPort = new PluginStream(g_PluginLoader, spi);
		printf("Plugin Port\n");
		break;
	default:
		pPort = new TestStream(spi);
		printf("Test Port\n");
		break;
	}
	if (!pPort) {
		return -1;
	}
	if (peer->state != ENET_PEER_STATE_CONNECTED) {
		printf("Add Pending Port\n");
		m_PendingPorts[peer].push_back(pPort);
	}
	else {
		printf("Open Port\n");
		pPort->Open();
	}

	m_PortInfo[pPort] = info;
	m_PeerChannel2Port[std::make_pair(peer, info.Channel)] = pPort;
	return 0;
}

int StreamMgr::Destroy(const StreamProcess& StreamServer, int channel)
{
	AutoLock lock(m_Lock);
	for (int i = 0; i < RC_MAX_CONNECTION; ++i) {
		if (m_StreamServers[i] == NULL)
			continue;
		if (m_StreamServers[i]->Index == StreamServer.Index
			&& 0 == strcmp(m_StreamServers[i]->StreamIP, StreamServer.StreamIP)
			&& m_StreamServers[i]->Port == StreamServer.Port)
		{
			IStreamPort* pPort = m_PeerChannel2Port[std::make_pair(m_Peers[i], channel)];
			if (pPort) {
				m_PendingClosePorts.push_back(pPort);
				return 0;
			}
			break;
		}
	}
	return -1;
}
int StreamMgr::CloseStream(IStreamPort * port)
{
	AutoLock lock(m_Lock);
	m_PendingClosePorts.push_back(port);
	return 0;
}

int StreamMgr::_CloseStream(IStreamPort * port)
{
	port->Close();
	PeerChannel2PortMap::iterator ptr = m_PeerChannel2Port.begin();
	for (; ptr != m_PeerChannel2Port.end(); ++ptr) {
		if (ptr->second == port) {
			break;
		}
	}
	if (ptr != m_PeerChannel2Port.end()) {
		int index = 0;
		for (; index < RC_MAX_CONNECTION; ++index) {
			if (m_Peers[index] == ptr->first.first) {
				long& count = *(long*)&m_Peers[index]->data;
				count--;
				if (count <= 0) {
					m_PendingClose.push_back(index);
				}
			}
		}
		m_PeerChannel2Port.erase(ptr);
		m_PortInfo.erase(port);
	}
	m_PendingDelete.push_back(port);
	return 0;
}

int StreamMgr::ProcessPending()
{
	AutoLock lock(m_Lock);
	std::list<int>::iterator ptr = m_PendingClose.begin();
	for (; ptr != m_PendingClose.end(); ++ptr) {
		int i = *ptr;
		if (!m_Peers[i]) {
			printf("What happed!!! my GOD!!!!\n");
			if (m_StreamServers[i]) {
				printf("StreamServer[i] is not empty\n");
			}
			continue;
		}
		enet_peer_disconnect_later(m_Peers[i], 0);

		delete m_StreamServers[i];
		m_StreamServers[i] = NULL;
		m_Peers[i] = NULL;
	}
	m_PendingClose.clear();
	std::list<IStreamPort*>::iterator pp = m_PendingDelete.begin();
	for (; pp != m_PendingDelete.end(); ++pp) {
		delete *pp;
	}
	m_PendingDelete.clear();
	for (auto & pPort : m_PendingClosePorts) {
		_CloseStream(pPort);
	}
	m_PendingClosePorts.clear();
	return 0;
}
