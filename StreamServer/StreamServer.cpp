// StreamServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <cstring>
#include <map>
#include <vector>
#include <enet/enet.h>
#include <zmq.h>
#include <koo_zmq_helpers.h>
#include <DataDefs.h>
#include <DataJson.h>

//ENetPeer* MapperPeers[RC_MAX_CONNECTION_PER_SERVER];

struct MapperData {
	ENetPeer* Peer;
	bool Channels[RC_MAX_CONNECTION];

	MapperData() {
		for (int i = 0; i < RC_MAX_CONNECTION; ++i) {
			Channels[i] = false;
		}
	}
};
std::map<int, std::vector<MapperData*> > ClientMapperMap;
ENetPeer* ClientPeers[RC_MAX_CONNECTION_PER_SERVER];

bool send_add_stream(int id, void* socket, const IPInfo& info) {
	std::stringstream ss;
	ss << id;
	KZPacket add(ss.str(), "ADD");
	add.set("info", KOO_GEN_JSON(info));
	int rc = koo_zmq_send(socket, add);
	if (rc != 0)
		return false;
	KZPacket result;
	rc = koo_zmq_recv(socket, result);
	if (rc != 0)
		return false;

	return result.get("result");
}
bool send_remove_stream(int id, void* socket) {
	std::stringstream ss;
	ss << id;
	KZPacket add(ss.str(), "REMOVE");
	int rc = koo_zmq_send(socket, add);
	if (rc != 0)
		return false;
	KZPacket result;
	rc = koo_zmq_recv(socket, result);
	if (rc != 0)
		return false;

	return result.get("result");
}
int main(int argc, char* argv[])
{
	int id = RC_STREAM_SERVER_ID_BASE;
	const char* sip = "127.0.0.1";
	int sport = 6602;
	const char* bip = "127.0.0.1";
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

	if (id < RC_STREAM_SERVER_ID_BASE || id > RC_STREAM_SERVER_ID_BASE + RC_MAX_STREAM_SERVER_COUNT)
	{
		std::cerr << "Invalid Stream Server id: " << id << std::endl;
		std::cerr << "Valid From " << RC_STREAM_SERVER_ID_BASE;
		std::cerr << " to " << RC_STREAM_SERVER_ID_BASE + RC_MAX_STREAM_SERVER_COUNT << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "StreamServer ID: \t" << id << std::endl;
	std::cout << "Server IP: \t" << sip << std::endl;
	std::cout << "Server Port: \t" << sport << std::endl;
	std::cout << "Bind IP: \t" << bip << std::endl;
	std::cout << "Bind Port: \t" << port << std::endl;

	enet_initialize();

	ENetAddress address;
	ENetHost * remote;
	if (strcmp(bip, "*") == 0) {
		address.host = ENET_HOST_ANY;
	}
	else {
		enet_address_set_host(&address, bip);
	}
	address.port = port;

	remote = enet_host_create(&address /* the address to bind the server host to */,
		RC_MAX_CONNECTION_PER_SERVER      /* allow up to 32 clients and/or outgoing connections */,
		RC_MAX_CONNECTION + 1      /* allow up to 2 channels to be used, 0 and 1 */,
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
	if (rc != 0)
		goto CLOSE;

	IPInfo info;
	info.port = port;
	sprintf(info.sip, "%s", bip);
	if (!send_add_stream(id, req, info))
		goto CLOSE;

	printf("%s\n", "Initialized!");
	ENetEvent event;
	/* Wait up to 5000 milliseconds for an event. */
	while (true)
	{
		int nRet = enet_host_service(remote, &event, 5000);
		if (nRet < 0) {
			int nr = errno;
			printf("errno %d\n", errno);
			break;
		}
#ifdef _DEBUG
		putc('.', stdout);
#endif
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
		{
			event.peer->data = (void*)(long)event.data;

			int nType = ((event.data & 0xFFFF0000) >> 16);
			int nIndex = (event.data & 0xFFFF);
			printf("A new connection %d - %d connected from %x:%u.\n",
				nType,
				nIndex,
				event.peer->address.host,
				event.peer->address.port);
			if (nType == MAPPER_TYPE) {
				MapperData* data = new MapperData();
				data->Peer = event.peer;
				ClientMapperMap[nIndex].push_back(data);
			}
			else if (nType == CLIENT_TYPE) {
				// This is the client
				ENetPeer* pOldPeer = ClientPeers[nIndex];
				// This is the mapper
				if (pOldPeer != NULL && pOldPeer != event.peer) {
					printf("New client for index %u, disconnect old one\n", nIndex);
					enet_peer_disconnect(pOldPeer, -2);
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
			int data = (long)event.peer->data;
			int nType = ((data & 0xFFFF0000) >> 16);
			int nIndex = (data & 0xFFFF);

#ifdef _DEBUG
			printf("A packet of length %u was received from %d-%d on channel %u.\n",
				event.packet->dataLength,
				nType,
				nIndex,
				event.channelID);
#endif
			if (RC_MAX_CONNECTION != event.channelID && nType == MAPPER_TYPE) {
				ENetPeer* client = ClientPeers[nIndex];
				if (client) {
					ENetPacket *cp = enet_packet_create(event.packet->data, event.packet->dataLength, ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(client, event.channelID, cp);
				}
				else {
					std::stringstream ss;
					ss << "ERROR: No Client for this Mapper Connectionn. Index:" << nIndex << "\n";
					ENetPacket* perr = enet_packet_create(ss.str().c_str(), ss.str().length(), ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(event.peer, event.channelID, perr);
				}
			}
			else if (RC_MAX_CONNECTION != event.channelID && nType == CLIENT_TYPE) {
				//ENetPeer* mapper = MapperPeers[nIndex];
				ENetPeer* mapper = NULL;
				std::vector<MapperData*>::iterator ptr = ClientMapperMap[nIndex].begin();
				for (; ptr != ClientMapperMap[nIndex].end(); ++ptr) {
					if ((*ptr)->Channels[event.channelID] == true) {
						mapper = (*ptr)->Peer;
					}
				}
				if (mapper) {
					ENetPacket *cp = enet_packet_create(event.packet->data, event.packet->dataLength, ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(mapper, event.channelID, cp);
				}
				else {
					std::stringstream ss;
					ss << "ERROR: No Mapper for this Client Connectionn. Index:" << nIndex << "\n";
					ENetPacket* perr = enet_packet_create(ss.str().c_str(), ss.str().length(), ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(event.peer, event.channelID, perr);
				}
			}
			else {
				if (RC_MAX_CONNECTION == event.channelID) {
					StreamEventPacket* sep = (StreamEventPacket*)event.packet->data;
					if (nType == MAPPER_TYPE) {
						if (sep->event == SE_CHANNEL_OPENED || sep->event == SE_CHANNEL_CLOSED) {
							MapperData* pData = NULL;
							std::vector<MapperData*>::iterator ptr = ClientMapperMap[nIndex].begin();
							for (; ptr != ClientMapperMap[nIndex].end(); ++ptr) {
								if (event.peer == (*ptr)->Peer) {
									(*ptr)->Channels[sep->channel] = sep->event == SE_CHANNEL_OPENED ? true : false;
									pData = *ptr;
									break;
								}
							}
							if (!pData) {
								printf("There should be client Connections\n");
							}
						}
						// Forward event to client
						ENetPeer* client = ClientPeers[nIndex];
						if (client) {
							ENetPacket *cp = enet_packet_create(event.packet->data, event.packet->dataLength, ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(client, RC_MAX_CONNECTION, cp);
						}
						else {
							printf("There should be mapper Connections\n");
						}
					}
				}
				else {
					printf("Incorrect Connections\n");
				}
			}
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(event.packet);

		} break;
		case ENET_EVENT_TYPE_DISCONNECT:
			int data = (long)event.peer->data;
			int nType = ((data & 0xFFFF0000) >> 16);
			int nIndex = (data & 0xFFFF);
			printf("connection %d - %d disconnected.\n", nType, nIndex);

			if (nType == MAPPER_TYPE) {
				std::vector<MapperData*>::iterator ptr = ClientMapperMap[nIndex].begin();
				for (; ptr != ClientMapperMap[nIndex].end(); ++ptr) {
					if (event.peer == (*ptr)->Peer) {
						ClientMapperMap[nIndex].erase(ptr);
						break;
					}
				}
			}
			else if (nType == CLIENT_TYPE) {
				std::vector<MapperData*>::iterator ptr = ClientMapperMap[nIndex].begin();
				for (; ptr != ClientMapperMap[nIndex].end(); ++ptr) {
					enet_peer_disconnect_later((*ptr)->Peer, -3);
				}
				ClientMapperMap.erase(nIndex);
			}
			/* Reset the peer's client information. */
			event.peer->data = NULL;
		}
	}

CLOSE:
	send_remove_stream(id, req);

	zmq_close(req);
	zmq_ctx_term(ctx);

	enet_host_destroy(remote);
	enet_deinitialize();
    return 0;
}

