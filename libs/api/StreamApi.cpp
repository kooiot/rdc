#include "stdafx.h"
#include "StreamApi.h"
#include <thread>
#include <enet\enet.h>

CStreamApi::CStreamApi(IStreamHandler & Handler) : m_Handler(Handler)
{
}

CStreamApi::~CStreamApi()
{
}

bool CStreamApi::Connect(const char * ip, int port)
{
	ENetHost * client;
	client = enet_host_create(NULL, 1, 32, 0, 0);
	if (client == NULL)
	{
		fprintf(stderr,
			"An error occurred while trying to create an ENet client host.\n");
		exit(EXIT_FAILURE);
	}
	ENetAddress address;
	ENetPeer *peer;

	enet_address_set_host(&address, ip);
	address.port = port;
	peer = enet_host_connect(client, &address, 32, 0);
	if (peer == NULL)
	{
		fprintf(stderr,
			"No available peers for initiating an ENet connection.\n");
		return false;
	}
	
	std::thread([this, client, peer](){
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
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					printf("A packet of length %u containing %s was received from %s on channel %u.\n",
						event.packet->dataLength,
						event.packet->data,
						(char*)event.peer->data,
						event.channelID);
					/* Clean up the packet now that we're done using it. */
					enet_packet_destroy(event.packet);

					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					printf("%s disconnected.\n", (char*)event.peer->data);
					/* Reset the peer's client information. */
					event.peer->data = NULL;
				}
			}
		}
	});
	return false;
}

bool CStreamApi::Disconnect()
{
	m_bAbort = true;

	return true;
}
