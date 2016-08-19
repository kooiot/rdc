// test_enet_client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <enet\enet.h>
#include <thread>
#include <mutex>

int main()
{
	enet_initialize();

	ENetHost * client = enet_host_create(NULL /* create a client host */,
		1 /* only allow 1 outgoing connection */,
		2 /* allow up 2 channels to be used, 0 and 1 */,
		57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
		14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);
	if (client == NULL)
	{
		fprintf(stderr,
			"An error occurred while trying to create an ENet client host.\n");
		exit(EXIT_FAILURE);
	}

	ENetAddress address;
	ENetPeer *peer;

	enet_address_set_host(&address, "127.0.0.1");
	address.port = 6680;
	peer = enet_host_connect(client, &address, 17, 1);
	if (peer == NULL)
	{
		fprintf(stderr,
			"No available peers for initiating an ENet connection.\n");
		return false;
	}

	std::mutex* lock = new std::mutex();
	ENetEvent event;
	/* Wait up to 1000 milliseconds for an event. */
	while (enet_host_service(client, &event, 1000) >= 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			printf("A new client connected from %x:%u.\n",
				event.peer->address.host,
				event.peer->address.port);
			/* Store any relevant client information here. */
			event.peer->data = "Client information";
			{
				std::thread* p = new std::thread([lock, peer] {
					do {
						char temp[80];
						for (int i = 0; i < 80; ++i) {
							temp[i] = 'a' + i;
						}
						{
							auto p = enet_packet_create(temp, 80, ENET_PACKET_FLAG_RELIABLE);
							lock->lock();
							enet_peer_send(peer, 0, p);
							lock->unlock();
						}
						Sleep(10);
					} while (true);
				});
				std::thread* p2 = new std::thread([lock, peer] {
					do {
						char temp[80];
						for (int i = 0; i < 80; ++i) {
							temp[i] = 'a' + i;
						}
						{
							auto p = enet_packet_create(temp, 80, ENET_PACKET_FLAG_RELIABLE);
							lock->lock();
							enet_peer_send(peer, 1, p);
							lock->unlock();
						}
						Sleep(10);
					} while (true);
				});
			}
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			//printf("A packet of length %u was received from %s on channel %u.\n",
			//	event.packet->dataLength,
			//	event.peer->data,
			//	event.channelID);
			putc('.', stderr);

			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(event.packet);

			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			printf("%s disconnected.\n", event.peer->data);
			/* Reset the peer's client information. */
			event.peer->data = NULL;
			break;
		}
	}

    return 0;
}

