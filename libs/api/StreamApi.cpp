#include "StreamApi.h"
#include <thread>
#include <cassert>
#include <cstring>
#include <enet/enet.h>
#include "DataDefs.h"
#include "DataJson.h"

CStreamApi::CStreamApi(IStreamHandler & Handler, int nType, int nIndex, int nStreamMask)
	: m_Handler(Handler),
	m_nStreamMask(nStreamMask),
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
	client = enet_host_create(NULL, 2, RC_MAX_CONNECTION + 1, 0, 0);
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
	peer = enet_host_connect(client, &address, RC_MAX_CONNECTION + 1, m_nData);
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
					//event.peer->data = "Client information";
					m_Handler.OnEvent(-1, SE_CONNECT, "");
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					printf("A packet of length %u containing %s was received from %s on channel %u.\n",
						event.packet->dataLength,
						event.packet->data,
						(char*)event.peer->data,
						event.channelID);
					OnData(event.channelID, event.packet->data, event.packet->dataLength);
					/* Clean up the packet now that we're done using it. */
					enet_packet_destroy(event.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					printf("%s disconnected.\n", (char*)event.peer->data);
					/* Reset the peer's client information. */
					//event.peer->data = NULL;
					m_Handler.OnEvent(-1, SE_DISCONNECT, "");
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

	if (m_pThread && m_pThread->joinable())
		m_pThread->join();
}

int CStreamApi::SendData(int channel, void * buf, size_t len)
{
	if (!m_Peer || m_bAbort)
		return -1;
	//ENetPacket* packet = enet_packet_create(buf, len, ENET_PACKET_FLAG_RELIABLE);
	ENetPacket* packet = enet_packet_create(&m_nStreamMask, sizeof(int), ENET_PACKET_FLAG_RELIABLE);
	enet_packet_resize(packet, sizeof(int) + len);
	memcpy(packet->data + sizeof(int), buf, len);
	int rc = enet_peer_send((ENetPeer*)m_Peer, channel, packet);

	return rc;
}

int CStreamApi::OnData(int channel, void * data, size_t len)
{
	if (channel == RC_MAX_CONNECTION) {
		// Event
		StreamEventPacket sep;
		std::string str((char*)data, len);
		json j = json::parse(str);
		if (KOO_PARSE_JSON(sep, j)) {
			m_Handler.OnEvent(sep.channel, sep.event, sep.msg);
		}
		return 0;
	}
	// Data
	int Mask = *(long*)data;
	if (Mask != m_nStreamMask) {
		fprintf(stderr, "Incorrect Mask Packet %ld - %ld", Mask, m_nStreamMask);
		return -1;
	}
	char* pbuf = (char*)data;
	pbuf += sizeof(int);
	m_Handler.OnData(channel, pbuf, len - sizeof(int));

	return 0;
}
