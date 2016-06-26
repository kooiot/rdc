// StreamServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <enet\enet.h>
#include <zmq.h>
#include <koo_zmq_helpers.h>
#include <DataDefs.h>

#define MAX_CONNECTION_PER_SERVER 128
#define MAX_CHANNEL_PER_SERVER 32

#define MAPPER_TYPE 1
#define CLIENT_TYPE 2

ENetPeer* MapperPeers[MAX_CONNECTION_PER_SERVER];
ENetPeer* ClientPeers[MAX_CONNECTION_PER_SERVER];

bool send_add_stream(int id, void* socket, const StreamProcess& sp) {
	KZPacket add;
	std::stringstream ss;
	ss << id;
	add.id = ss.str();
	add.cmd = "ADD";
	add.data = std::string((char*)&sp, sizeof(StreamProcess));
	int rc = koo_zmq_send_cmd(socket, add);
	if (rc != 0)
		return false;
	KZPacket result;
	rc = koo_zmq_recv_cmd(socket, result);
	if (rc != 0)
		return false;

	return result.data == S_SUCCESS;
}
bool send_remove_stream(int id, void* socket) {
	KZPacket add;
	std::stringstream ss;
	ss << id;
	add.id = ss.str();
	add.cmd = "REMOVE";
	int rc = koo_zmq_send_cmd(socket, add);
	if (rc != 0)
		return false;
	KZPacket result;
	rc = koo_zmq_recv_cmd(socket, result);
	if (rc != 0)
		return false;

	return result.data == S_SUCCESS;
}
int main(int argc, char* argv[])
{
	int id = 10000;
	char* sip = "127.0.0.1";
	int sport = 6602;
	char* bip = "127.0.0.1";
	int port = 6800;

	if (argc >= 2)
		id = atoi(argv[1]);
	if (argc >= 3)
		sip = argv[2];
	if (argc >= 4)
		sport = atoi(argv[3]);
	if (argc >= 5)
		bip = argv[4];
	if (argc >= 6)
		port = atoi(argv[5]);

	std::cout << "StreamServer ID: \t" << id << std::endl;
	std::cout << "Server IP: \t" << sip << std::endl;
	std::cout << "Server Port: \t" << sport << std::endl;
	std::cout << "Bind IP: \t" << bip << std::endl;
	std::cout << "Bind Port: \t" << port << std::endl;

	enet_initialize();

	ENetAddress address;
	ENetHost * server;
	if (strcmp(bip, "*") == 0) {
		address.host = ENET_HOST_ANY;
	}
	else {
		enet_address_set_host(&address, bip);
	}
	address.port = port;

	server = enet_host_create(&address /* the address to bind the server host to */,
		MAX_CONNECTION_PER_SERVER      /* allow up to 32 clients and/or outgoing connections */,
		MAX_CHANNEL_PER_SERVER      /* allow up to 2 channels to be used, 0 and 1 */,
		0      /* assume any amount of incoming bandwidth */,
		0      /* assume any amount of outgoing bandwidth */);
	if (server == NULL)
	{
		fprintf(stderr,
			"An error occurred while trying to create an ENet server host.\n");
		exit(EXIT_FAILURE);
	}

	void* ctx = zmq_ctx_new();
	void* req = zmq_socket(ctx, ZMQ_REQ);
	std::stringstream ss;
	ss << "tcp://" << sip << ":" << sport;
	int rc = zmq_connect(req, ss.str().c_str());
	if (rc != 0)
		goto CLOSE;

	StreamProcess sp;
	sp.Counter = 0;
	sp.Port = port;
	sprintf(sp.StreamIP, "%s", bip);
	if (!send_add_stream(id, req, sp))
		goto CLOSE;

	ENetEvent event;
	/* Wait up to 1000 milliseconds for an event. */
	while (enet_host_service(server, &event, 1000) >= 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
		{
			event.peer->data = (void*)event.data;

			int nType = (event.data & 0xFFFF0000 >> 8);
			int nIndex = (event.data & 0xFFFF);
			printf("A new client %d - %d connected from %x:%u.\n", 
				nType,
				nIndex,
				event.peer->address.host,
				event.peer->address.port);
			if (nType == MAPPER_TYPE) {
				// This is the mapper
				if (MapperPeers[nIndex] != NULL) {
					printf("New mapper for index %u, disconnect old one\n", nIndex);
					enet_peer_disconnect(MapperPeers[nIndex], -2);
				}
				MapperPeers[nIndex] = event.peer;
			}
			else if (nType == CLIENT_TYPE) {
				// This is the client
				if (ClientPeers[nIndex] != NULL) {
					printf("New client for index %u, disconnect old one\n", nIndex);
					enet_peer_disconnect(ClientPeers[nIndex], -2);
				}
				ClientPeers[nIndex] = event.peer;
			}
			else {
				printf("Incorrect client type %u\n", nType);
			}
			/* Store any relevant client information here. */
		}break;
		case ENET_EVENT_TYPE_RECEIVE:
		{
			int data = (int)event.peer->data;
			int nType = (data & 0xFFFF0000 >> 8);
			int nIndex = (data & 0xFFFF);

			printf("A packet of length %u containing %s was received from %d-%d on channel %u.\n",
				event.packet->dataLength,
				event.packet->data,
				nType,
				nIndex,
				event.channelID);
			if (nType == MAPPER_TYPE) {
				ENetPeer* client = ClientPeers[nIndex];
				if (client) {
					enet_peer_send(client, event.channelID, event.packet);
				}
				else {
					printf("No Client for this Mapper Connectionn %d\n", nIndex);
				}
			}
			else if (nType == CLIENT_TYPE) {
				ENetPeer* mapper = MapperPeers[nIndex];
				if (mapper) {
					enet_peer_send(mapper, event.channelID, event.packet);
				}
				else {
					printf("No Mapper for this Client Connectionn %d\n", nIndex);
				}
			}
			else {
				printf("Incorrect Connections\n");
			}
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(event.packet);

		} break;
		case ENET_EVENT_TYPE_DISCONNECT:
			int data = (int)event.peer->data;
			int nType = (data & 0xFFFF0000 >> 8);
			int nIndex = (data & 0xFFFF);
			printf("%d - %d disconnected.\n", nType, nIndex);

			if (nType == MAPPER_TYPE) {
				ENetPeer* client = ClientPeers[nIndex];
				if (client) {
					enet_peer_disconnect(client, -3);
				}
			}
			else if (nType == CLIENT_TYPE) {
				ENetPeer* mapper = MapperPeers[nIndex];
				if (mapper) {
					enet_peer_disconnect(mapper, -3);
				}
			}
			/* Reset the peer's client information. */
			event.peer->data = NULL;
		}
	}

CLOSE:
	send_remove_stream(id, req);

	zmq_close(req);
	zmq_ctx_term(ctx);

	enet_host_destroy(server);
	enet_deinitialize();
    return 0;
}

