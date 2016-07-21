#include "stdafx.h"
#include "UVUdp.h"

static void echo_alloc(uv_handle_t* handle,
	size_t suggested_size,
	uv_buf_t* buf) {
	buf->base = (char*)malloc(suggested_size);
	buf->len = suggested_size;
}

static void close_cb(uv_handle_t* handle) {
	delete (uv_udp_t*)handle;
}

UVUdp::UVUdp(uv_loop_t* uv_loop,
			RC_CHANNEL channel,
			IPortHandler& Handler,
			const UDPInfo& Info)
	: m_uv_loop(uv_loop), m_nChannel(channel), m_Handler(Handler), m_Info(Info)
{
	m_udp_handle = new uv_udp_t();
}


UVUdp::~UVUdp()
{
}

bool UVUdp::Open()
{
	if (m_Info.remote.port != 0) {
		int rc = uv_ip4_addr(m_Info.remote.sip, m_Info.remote.port, &m_peer_addr);
		if (0 != rc) {
			printf("Incorrect UDP server address %d\n", rc);
			return false;
		}
	}
	struct sockaddr_in addr;

	int rc = uv_ip4_addr(m_Info.bind.sip, m_Info.bind.port, &addr);
	if (0 != rc) {
		printf("Incorrect UDP server address %d\n", rc);
		return false;
	}

	rc = uv_udp_init(m_uv_loop, m_udp_handle);
	if (0 != rc) {
		printf("Cannot init UDP handle %d\n", rc);
		return false;
	}

	m_udp_handle->data = this;

	rc = uv_udp_bind(m_udp_handle, (const struct sockaddr*)&addr, UV_UDP_REUSEADDR);
	if (0 != rc) {
		uv_close((uv_handle_t*)m_udp_handle, close_cb);
		printf("Cannot bind UDP bind address %d\n", rc);
		return false;
	}

	rc = uv_udp_recv_start(m_udp_handle, echo_alloc, UdpRecvCB);
	if (0 != rc) {
		uv_close((uv_handle_t*)m_udp_handle, close_cb);
		printf("Cannot start UDP recv callback %d\n", rc);
		return false;
	}

	return true;
}

void UVUdp::Close()
{
	uv_udp_recv_stop(m_udp_handle);
	uv_close((uv_handle_t*)m_udp_handle, close_cb);
}

int UVUdp::OnData(void * buf, size_t len)
{
	uv_udp_send_t* send_req = new uv_udp_send_t();
	uv_buf_t uvbuf = uv_buf_init((char*)buf, len);
	int rc = uv_udp_send(send_req, m_udp_handle, &uvbuf, 1, (const struct sockaddr*)&m_peer_addr, SendCB);
	return rc;
}

int UVUdp::OnEvent(StreamEvent evt)
{
	return 0;
}

void UVUdp::UdpRecvCB(uv_udp_t * handle, ssize_t nread, const uv_buf_t * buf, const sockaddr * addr, unsigned flags)
{
	if (nread < 0) {
		fprintf(stderr, "recv_cb error: %s\n", uv_err_name(nread));
		assert(nread == UV_ECONNRESET || nread == UV_EOF);
		uv_close((uv_handle_t*)handle, close_cb);
	}
	else {
		UVUdp* pThis = (UVUdp*)handle->data;
		pThis->_UdpRecvCB(handle, nread, buf, addr, flags);
	}
}
void UVUdp::_UdpRecvCB(uv_udp_t * handle, ssize_t nread, const uv_buf_t * buf, const sockaddr * addr, unsigned flags)
{
	if (m_Info.remote.port != 0) {
		struct sockaddr_in* in_addr = (struct sockaddr_in*)addr;
		if (m_peer_addr.sin_port != in_addr->sin_port)
			return;
		if (m_peer_addr.sin_addr.S_un.S_addr != in_addr->sin_addr.S_un.S_addr)
			return;
	}
	else {
		memcpy(&m_peer_addr, (struct sockaddr_in*)addr, sizeof(sockaddr_in));
	}
	m_Handler.Send(m_nChannel, buf->base, nread);
}


void UVUdp::SendCB(uv_udp_send_t * req, int status)
{
	delete req;
	// FIXME:
}
