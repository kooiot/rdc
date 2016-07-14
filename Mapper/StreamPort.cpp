#include "StreamPort.h"

StreamPortBase::StreamPortBase(ENetPeer* peer, const ConnectionInfo& info, int mask)
	: m_Peer(peer), m_Mask(mask), m_Info(info)
{

}


StreamPortBase::~StreamPortBase()
{
}

int StreamPortBase::OnClientData(void * data, size_t len)
{
	int Mask = *(int*)data;
	if (m_Mask != Mask) {
		printf("Incorrect packet mask %d - %d", Mask, m_Mask);
		return -1;
	}
	char* pbuf = (char*)data;
	pbuf += sizeof(int);

	int n = this->OnWrite((uint8_t*)pbuf, len - sizeof(int));
	printf("Write To Port %d - %d", len - sizeof(int), n);
	return n;
}

int StreamPortBase::SendData(void * data, size_t len)
{
	//ENetPacket* packet = enet_packet_create(data, len, ENET_PACKET_FLAG_RELIABLE);
	ENetPacket* packet = enet_packet_create(&m_Mask, sizeof(int), ENET_PACKET_FLAG_RELIABLE);
	enet_packet_resize(packet, sizeof(int) + len);
	memcpy(packet->data + sizeof(int), data, len);
	enet_peer_send(m_Peer, m_Info.Channel, packet);
	return 0;
}

int StreamPortBase::OnOpened()
{
	StreamEventPacket sp;
	sp.event = SE_CHANNEL_CONNECT;
	sp.channel = m_Info.Channel;
	ENetPacket* packet = enet_packet_create(&sp, sizeof(StreamEventPacket), ENET_PACKET_FLAG_RELIABLE);
	int rc = enet_peer_send(m_Peer, RC_MAX_CONNECTION, packet);
	printf("Send SE_CHANNEL_CONNECT returns %d\n", rc);
	return rc;
}

int StreamPortBase::OnClosed()
{
	StreamEventPacket sp;
	sp.event = SE_CHANNEL_DISCONNECT;
	sp.channel = m_Info.Channel;
	ENetPacket* packet = enet_packet_create(&sp, sizeof(StreamEventPacket), ENET_PACKET_FLAG_RELIABLE);
	int rc = enet_peer_send(m_Peer, RC_MAX_CONNECTION, packet);
	printf("Send SE_CHANNEL_DISCONNECT returns %d\n", rc);

	printf("StreamPort Closed Channel %d", m_Info.Channel);
	return rc;
}