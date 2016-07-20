#include "StreamPort.h"
#include "StreamMgr.h"
#include <cstring>
#include <cstdarg>

const std::string StreamEventNames[] = {
	"SE_CONNECT",
	"SE_DISCONNECT",
	"SE_CHANNEL_OPENED",
	"SE_CHANNEL_CLOSED",
	"SE_CHANNEL_NOT_SUPPORT",
	"SE_CHANNEL_OPEN_FAILED",
	"SE_CHANNEL_READ_ERROR",
	"SE_CLOSE",
	"SE_TIMEOUT",
};

StreamPortBase::StreamPortBase(StreamPortInfo& info)
	: m_Info(info)
{

}


StreamPortBase::~StreamPortBase()
{
}

int StreamPortBase::OnClientData(void * data, size_t len)
{
	int Mask = *(int*)data;
	if (m_Info.Mask != Mask) {
		printf("Incorrect packet mask %d - %d", Mask, m_Info.Mask);
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
	ENetPacket* packet = enet_packet_create(&m_Info.Mask, sizeof(int), ENET_PACKET_FLAG_RELIABLE);
	enet_packet_resize(packet, sizeof(int) + len);
	memcpy(packet->data + sizeof(int), data, len);
	enet_peer_send(m_Info.Peer, m_Info.ConnInfo.Channel, packet);
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

int StreamPortBase::OnStreamClose()
{
	printf("OnStreamClose\n");
	m_Info.Mgr->CloseStream(this);
	return 0;
}

int StreamPortBase::_FireEvent(StreamEvent se, const char* msg)
{
	printf("Channel %d Send StreamEvent %s:%s\n", m_Info.ConnInfo.Channel, StreamEventNames[se].c_str(), msg);

	StreamEventPacket sp;
	sp.event = se;
	sp.channel = m_Info.ConnInfo.Channel;
	sprintf(sp.msg, "%s", msg);
	ENetPacket* packet = enet_packet_create(&sp, sizeof(StreamEventPacket), ENET_PACKET_FLAG_RELIABLE);
	int rc = enet_peer_send(m_Info.Peer, RC_MAX_CONNECTION, packet);
	printf("Send StreamEvent returns %d\n", rc);

	return rc;
}


