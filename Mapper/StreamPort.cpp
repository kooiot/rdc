#include "StreamPort.h"

StreamPortBase::StreamPortBase(ENetPeer* peer, const ConnectionInfo& info, int mask)
	: m_Peer(peer), m_Mask(mask), m_Info(info), m_pThread(NULL), m_bAbort(false)
{

}


StreamPortBase::~StreamPortBase()
{
}

void StreamPortBase::Start()
{
	if (m_pThread)
		return;

	m_pThread = new std::thread([this]() {
		Sleep(500);
		bool bOpen = false;
		while (!m_bAbort) {
			if (!bOpen) {
				if (m_Peer->state == ENET_PEER_STATE_CONNECTED) {
					bOpen = Open();
					if (bOpen) {
						printf("%s\n", "Open Stream Port OK");
						continue;
					}
					printf("%s\n", "Failed Stream Port");
				}
				Sleep(1000);
			}

			this->Run();
		}
	});
}

bool StreamPortBase::Open()
{
	StreamEventPacket sp;
	sp.event = SE_CHANNEL_CONNECT;
	sp.channel = m_Info.Channel;
	ENetPacket* packet = enet_packet_create(&sp, sizeof(StreamEventPacket), ENET_PACKET_FLAG_RELIABLE);
	int rc = enet_peer_send(m_Peer, RC_MAX_CONNECTION, packet);
	printf("Send SE_CHANNEL_CONNECT returns %d\n", rc);
	return rc >= 0;
}

void StreamPortBase::Close()
{
	StreamEventPacket sp;
	sp.event = SE_CHANNEL_DISCONNECT;
	sp.channel = m_Info.Channel;
	ENetPacket* packet = enet_packet_create(&sp, sizeof(StreamEventPacket), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(m_Peer, RC_MAX_CONNECTION, packet);

	m_bAbort = true;
	if (m_pThread && m_pThread->joinable())
		m_pThread->join();
	delete m_pThread;
	m_pThread = NULL;
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
