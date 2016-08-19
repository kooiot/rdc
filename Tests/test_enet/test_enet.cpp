// test_enet.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <enet\enet.h>
#include <string>
#include <sstream>
#include <zmq.h>
#include <koo_zmq_helpers.h>
#include <DataJson.h>
#include <thread>

bool send_add_stream(int id, void* socket, const IPInfo& info) {
	std::stringstream ss;
	ss << id;
	KZPacket add(ss.str(), "ADD");
	add.set("info", KOO_GEN_JSON(info));
	int rc = koo_zmq_send(socket, add);
	if (rc < 0)
		return false;
	KZPacket result;
	rc = koo_zmq_recv(socket, result);
	if (rc != 0)
		return false;

	return result.get("result");
}

bool send_heartbeat_stream(int id, void* socket, const IPInfo& info) {
	std::cerr << "Fire HEARTBEAT" << std::endl;
	std::stringstream ss;
	ss << id;
	KZPacket add(ss.str(), "HEARTBEAT");
	add.set("info", KOO_GEN_JSON(info));
	int rc = koo_zmq_send(socket, add);
	if (rc < 0)
		return false;
	KZPacket result;
	rc = koo_zmq_recv(socket, result);
	if (rc != 0)
		return false;

	return result.get("result");
}

int main()
{
	int id = 10000;
	std::string sip = "127.0.0.1";
	int sport = 6602;

	ENetPeer* Peers[2];
	Peers[0] = NULL;
	Peers[1] = NULL;
	enet_initialize();

	ENetAddress address;
	ENetHost * remote;
	address.host = ENET_HOST_ANY;
	address.port = 6680;

	remote = enet_host_create(&address /* the address to bind the server host to */,
		128 * 16     /* allow up to 32 clients and/or outgoing connections */,
		17     /* allow up to 2 channels to be used, 0 and 1 */,
		0      /* assume any amount of incoming bandwidth */,
		0      /* assume any amount of outgoing bandwidth */);
	if (remote == NULL)
	{
		fprintf(stderr,
			"An error occurred while trying to create an ENet server host.\n");
		exit(EXIT_FAILURE);
	}


	void* ctx = zmq_ctx_new();
	void* req = zmq_socket(ctx, ZMQ_REQ);
	int timeo = 0;
	int rc = zmq_setsockopt(req, ZMQ_LINGER, &timeo, sizeof(int));
	std::stringstream ss;
	ss << "tcp://" << sip << ":" << sport;
	rc = zmq_connect(req, ss.str().c_str());

	IPInfo info;
	info.port = 6680;
	sprintf(info.sip, "%s", sip.c_str());
	if (send_add_stream(id, req, info)) {
		std::thread* hb = new std::thread([id, req, info] {
			while (true) {
#ifndef RDC_LINUX_SYS
				Sleep(5000);
#else
				sleep(5);
#endif
				if (!send_heartbeat_stream(id, req, info))
				{
					exit(EXIT_FAILURE);
				}
			}
		});
		printf("%s\n", "Initialized!");
	}
	ENetEvent event;
	/* Wait up to 1000 milliseconds for an event. */
	while (enet_host_service(remote, &event, 1000) >= 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			printf("A new client connected from %x:%u.\n",
				event.peer->address.host,
				event.peer->address.port);
			/* Store any relevant client information here. */
			{
				int index = 0;
				if (Peers[0] != NULL)
					index = 1;
				event.peer->data = &Peers[(index + 1) % 2];
				Peers[index] = event.peer;
			}
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			//printf("A packet of length %u was received from %s on channel %u.\n",
			//	event.packet->dataLength,
			//	event.peer->data,
			//	event.channelID);
			{
				ENetPeer* peer = *(ENetPeer**)event.peer->data;
				if (peer) {
					putc('.', stdout);
					auto p = enet_packet_create(event.packet->data, event.packet->dataLength, ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(peer, event.channelID, p);
				}
			}

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

