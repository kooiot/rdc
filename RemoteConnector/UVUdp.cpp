#include "stdafx.h"
#include "UVUdp.h"

UVUdp::UVUdp(uv_loop_t* uv_loop,
			RC_CHANNEL channel,
			IPortHandler& Handler,
			const UDPInfo& Info)
	: m_uv_loop(uv_loop), m_nChannel(channel), m_Handler(Handler), m_Info(Info)
{
}


UVUdp::~UVUdp()
{
}

void UVUdp::Start()
{
}

bool UVUdp::Open()
{
	struct sockaddr_in addr;

	int rc = uv_ip4_addr(m_Info.local.sip, m_Info.local.port, &addr);
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

	rc = uv_udp_bind(&m_udp_handle, (const struct sockaddr*)&addr, UV_UDP_REUSEADDR);
	if (0 != rc) {
		uv_close((uv_handle_t*)&m_udp_handle, NULL);
		printf("Cannot bind UDP local address %d\n", rc);
		return false;
	}

	rc = uv_udp_recv_start(&m_udp_handle, NULL, UdpRecvCB);
	if (0 != rc) {
		uv_close((uv_handle_t*)&m_udp_handle, NULL);
		printf("Cannot start UDP recv callback %d\n", rc);
		return false;
	}

	return true;
}

void UVUdp::Close()
{
	uv_udp_recv_stop(&m_udp_handle);
	uv_close((uv_handle_t*)&m_udp_handle, NULL);
}

int UVUdp::OnData(void * buf, size_t len)
{
	uv_buf_t uvbuf = uv_buf_init((char*)buf, len);
	int rc = uv_udp_send(&m_udp_send_req, &m_udp_handle, &uvbuf, len, (const struct sockaddr*)&m_peer_addr, SendCB);
	return rc;
}

int UVUdp::OnEvent(StreamEvent evt)
{
	return 0;
}

void UVUdp::UdpRecvCB(uv_udp_t * handle, ssize_t nread, const uv_buf_t * buf, const sockaddr * addr, unsigned flags)
{
	UVUdp* pThis = (UVUdp*)handle->data;
	pThis->_UdpRecvCB(handle, nread, buf, addr, flags);
}
void UVUdp::_UdpRecvCB(uv_udp_t * handle, ssize_t nread, const uv_buf_t * buf, const sockaddr * addr, unsigned flags)
{
	if (m_Info.server.port != 0) {
		struct sockaddr_in peer_addr;
		struct sockaddr_in* in_addr = (struct sockaddr_in*)addr;
		int rc = uv_ip4_addr(m_Info.local.sip, m_Info.local.port, &peer_addr);
		if (peer_addr.sin_port != in_addr->sin_port)
			return;
		if (peer_addr.sin_addr.S_un.S_addr != in_addr->sin_addr.S_un.S_addr)
			return;
	}
	m_Handler.Send(m_nChannel, buf->base, buf->len);
}


void UVUdp::SendCB(uv_udp_send_t * req, int status)
{
	// FIXME:
}
