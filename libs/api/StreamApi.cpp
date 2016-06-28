#include "StreamApi.h"
#include <thread>
#include <enet\enet.h>
#include "DataDefs.h"

CStreamApi::CStreamApi(IStreamHandler & Handler, int nIndex, int nType)
	: m_Handler(Handler),
	m_pThread(nullptr),
	m_bAbort(false)
{
	m_nData = ((nType & 0xFFFF) << 16) + (nIndex & 0xFFFF);
}

CStreamApi::~CStreamApi()
{
}

bool CStreamApi::Connect(const char * ip, int port)
{
	ENetHost * client;
	client = enet_host_create(NULL, 2, RC_MAX_CONNECTION, 0, 0);
	if (client == NULL)
	{
		fprintf(stderr,
			"An error occurred while trying to create an ENet client host.\n");
		return false;
	}
	ENetAddress address;
	ENetPeer *peer;

	enet_address_set_host(&address, ip);
	address.port = port;
	peer = enet_host_connect(client, &address, RC_MAX_CONNECTION, m_nData);
	if (peer == NULL)
	{
		fprintf(stderr,
			"No available peers for initiating an ENet connection.\n");
		return false;
	}
	m_Peer = peer;
	
	m_pThread =  new std::thread([this, client, peer](){
		ENetEvent event;

		while (!m_bAbort) {
			if (enet_host_service(client, &event, 500) > 0)
			{
				switch (event.type)
				{
				case ENET_EVENT_TYPE_CONNECT:
					printf("Connection to %d:%d succeeded.\n",
						event.peer->address.host,
						event.peer->address.port);
					/* Store any relevant client information here. */
					event.peer->data = "Client information";
					m_Handler.OnEvent(SE_CONNECT);
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					printf("A packet of length %u containing %s was received from %s on channel %u.\n",
						event.packet->dataLength,
						event.packet->data,
						(char*)event.peer->data,
						event.channelID);
					m_Handler.OnData(event.channelID, event.packet->data, event.packet->dataLength);

					/* Clean up the packet now that we're done using it. */
					enet_packet_destroy(event.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					printf("%s disconnected.\n", (char*)event.peer->data);
					/* Reset the peer's client information. */
					event.peer->data = NULL;
					m_Handler.OnEvent(SE_DISCONNECT);
				}
			}
		}
		enet_peer_disconnect((ENetPeer*)m_Peer, 0);
		enet_host_destroy(client);
	});
	return true;
}

void CStreamApi::Disconnect()
{
	m_bAbort = true;
	m_Peer = NULL;

	if (m_pThread && m_pThread->joinable())
		m_pThread->join();
}

int CStreamApi::SendData(int channel, const unsigned char * buf, size_t len)
{
	if (!m_Peer)
		return -1;
	ENetPacket* packet = enet_packet_create(buf, len, ENET_PACKET_FLAG_RELIABLE);
	int rc = enet_peer_send((ENetPeer*)m_Peer, channel, packet);

	return rc;
}
