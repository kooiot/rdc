#include "TestStream.h"

TestStream::TestStream(ENetPeer* peer, const ConnectionInfo& info, int mask)
	: StreamPortBase(peer, info, mask)
{
}


TestStream::~TestStream()
{
}

bool TestStream::Open()
{
	if (m_Info.Type != CT_TEST) {
		StreamEventPacket sp;
		sp.event = SE_CHANNEL_NOT_SUPPORT;
		sp.channel = m_Info.Channel;
		ENetPacket* packet = enet_packet_create(&sp, sizeof(StreamEventPacket), ENET_PACKET_FLAG_RELIABLE);
		int rc = enet_peer_send(m_Peer, RC_MAX_CONNECTION, packet);
		printf("Send Not Support returns %d\n", rc);
		return rc >= 0;
	}
	return __super::Open();
}

void TestStream::Close()
{
	__super::Close();
}

void TestStream::Run()
{
	Sleep(1000);
	if (m_Info.Type != CT_TEST)
		return;

	time_t now = time(NULL);
	char buf[80];
	strftime(buf, 80, "%F %T", localtime(&now));

	this->SendData(buf, strlen(buf));
}

int TestStream::OnWrite(void * data, size_t len)
{
	std::string ch = std::string((char*)data, len);
	printf("Received %s\n", ch.c_str());
	return 0;
}
