#include "StreamPort.h"
#include <cstring>
#include <cstdarg>

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

int StreamPortBase::FireEvent(StreamEvent se, const char* msgfmt, ...)
{
	va_list args;
	va_start(args, msgfmt);
	char* temp = new char[1024];
	vsprintf(temp, msgfmt, args);

	printf("%s\n", temp);
	temp[127] = '\0';
	va_end(args);

	return _FireEvent(se, temp);
}

int StreamPortBase::_FireEvent(StreamEvent se, const char* msg)
{
	printf("Channel %d Send StreamEvent %d:%s\n", m_Info.Channel, se, msg);

	StreamEventPacket sp;
	sp.event = se;
	sp.channel = m_Info.Channel;
	sprintf(sp.msg, "%s", msg);
	ENetPacket* packet = enet_packet_create(&sp, sizeof(StreamEventPacket), ENET_PACKET_FLAG_RELIABLE);
	int rc = enet_peer_send(m_Peer, RC_MAX_CONNECTION, packet);
	printf("Send StreamEvent returns %d\n", rc);

	return rc;
}


