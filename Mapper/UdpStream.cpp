#include "UdpStream.h"



UdpStream::UdpStream(uv_loop_t* uv_loop, ENetPeer* peer, const ConnectionInfo& info, int mask)
	: StreamPortBase(peer, info, mask)
	, m_uv_loop(uv_loop)
{
}


UdpStream::~UdpStream()
{
}

void UdpStream::UdpRecvCB(uv_udp_t * handle, ssize_t nread, const uv_buf_t * buf, const sockaddr * addr, unsigned flags)
{
	UdpStream* pThis = (UdpStream*)handle->data;
	pThis->SendData((void*)buf, nread);
}

void UdpStream::SendCB(uv_udp_send_t * req, int status)
{
	// FIXME:
}

void UdpStream::Start()
{
}

bool UdpStream::Open()
{
	m_bConnected = false;
	struct sockaddr_in addr;

	int rc = uv_ip4_addr(m_Info.UDP.server.sip, m_Info.UDP.server.port, &m_peer_addr);
	if (0 != rc) {
		printf("Incorrect UDP server address %d\n", rc);
		return false;
	}

	rc = uv_udp_init(m_uv_loop, &m_udp_handle);
	if (0 != rc) {
		printf("Cannot init UDP handle %d\n", rc);
		return false;
	}

	m_udp_handle.data = this;

	if (m_Info.UDP.local.port != 0) {
		rc = uv_ip4_addr(m_Info.UDP.local.sip, m_Info.UDP.local.port, &addr);
		if (0 != rc) {
			uv_close((uv_handle_t*)&m_udp_handle, NULL);
			printf("Incorrect UDP local address %d\n", rc);
			return false;
		}

		rc = uv_udp_bind(&m_udp_handle, (const struct sockaddr*)&addr, UV_UDP_REUSEADDR);
		if (0 != rc) {
			uv_close((uv_handle_t*)&m_udp_handle, NULL);
			printf("Cannot bind UDP local address %d\n", rc);
			return false;
		}
	}

	rc = uv_udp_recv_start(&m_udp_handle, NULL, UdpRecvCB);
	if (0 != rc) {
		uv_close((uv_handle_t*)&m_udp_handle, NULL);
		printf("Cannot start UDP recv callback %d\n", rc);
		return false;
	}

	rc = FireEvent(SE_CHANNEL_OPENED);
	return rc == 0;
}

void UdpStream::Close()
{
	uv_udp_recv_stop(&m_udp_handle);
	uv_close((uv_handle_t*)&m_udp_handle, NULL);
	FireEvent(SE_CHANNEL_CLOSED);
}

int UdpStream::OnWrite(void * data, size_t len)
{
	uv_buf_t buf = uv_buf_init((char*)data, len);
	int rc = uv_udp_send(&m_udp_send_req, &m_udp_handle, &buf, len, (const struct sockaddr*)&m_peer_addr, SendCB);
	return rc;
}
