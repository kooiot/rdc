#include "StreamMgr.h"
#include <cassert>
#include "SerialStream.h"

StreamMgr::StreamMgr() : m_ClientHost(NULL), m_pThread(NULL), m_bAbort(false)
{
	memset(m_Peers, 0, sizeof(ENetPeer*) * RC_MAX_CONNECTION);
}


StreamMgr::~StreamMgr()
{
}

bool StreamMgr::Init()
{
	m_bAbort = false;
	m_ClientHost = enet_host_create(NULL, RC_MAX_CONNECTION, RC_MAX_CONNECTION, 0, 0);
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
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					printf("A packet of length %u was received from %d on channel %u.\n",
						event.packet->dataLength,
						(int)event.peer->data,
						event.channelID);
					OnData(event.peer, event.channelID, event.packet->data, event.packet->dataLength);

					/* Clean up the packet now that we're done using it. */
					enet_packet_destroy(event.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					printf("ENet peer disconnected, count %d.\n", (long)event.peer->data);
					if ((long)event.peer->data != 0)
					{
						// TODO:
					}
				}
			}
		}
		enet_host_destroy(m_ClientHost);
	});

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

bool StreamMgr::OnData(ENetPeer* peer, int channel, void * data, size_t len)
{
	std::pair<ENetPeer*, int> key = std::make_pair(peer, channel);
	PeerChannel2PortMap::iterator ptr = m_PeerChannel2Port.find(key);
	if (ptr == m_PeerChannel2Port.end()) {
		fprintf(stderr, "No StreamPort for enet peer channel.\n");
		return false;
	}
	IStreamPort* port = ptr->second;
	int rc = port->OnClientData(data, len);
	if (rc < 0)
		return -1;
	return true;
}

//bool StreamMgr::OnEvent(StreamEvent event)
//{
//	return false;
//}

int StreamMgr::Create(const StreamProcess& StreamServer, const ConnectionInfo & info)
{
	int i = 0;
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
		peer = enet_host_connect(m_ClientHost, &address, RC_MAX_CONNECTION, nData);
		if (peer == NULL)
		{
			fprintf(stderr,
				"No available peers for initiating an ENet connection.\n");
			return -1;
		}
		long i = 1;
		peer->data = (void*)i;
		m_StreamServers[i] = new StreamProcess(StreamServer);
		m_Peers[i] = peer;
	}
	else
	{
		// The connection is built and used by other channel.
		peer = m_Peers[i];
	}
	assert(info.Type == CT_SERIAL);

	// Create StreamPort here: FIXME:
	SerialStream* pStream = new SerialStream(peer, info);
	pStream->Open();
	m_PortInfo[pStream] = info;
	m_PeerChannel2Port[std::make_pair(peer, info.Channel)] = pStream;
	return 0;
}

int StreamMgr::Destroy(const StreamProcess& StreamServer, int channel)
{
	for (int i = 0; i < RC_MAX_CONNECTION; ++i) {
		if (m_StreamServers[i] == NULL)
			continue;
		if (m_StreamServers[i]->Index == StreamServer.Index
			&& 0 == strcmp(m_StreamServers[i]->StreamIP, StreamServer.StreamIP)
			&& m_StreamServers[i]->Port == StreamServer.Port)
		{
			long* count = (long*)&m_Peers[i]->data;
			*count--;
			if (*count <= 0) {
				enet_peer_disconnect(m_Peers[i], 0);
			}
			delete m_StreamServers[i];
			m_StreamServers[i] = NULL;
			// TODO: Close stream
			return 0;
		}
	}
	return -1;
}
