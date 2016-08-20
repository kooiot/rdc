#include "Udp.h"
#include <cassert>
#include <cstring>

#ifndef RLOG
#define RLOG printf
#endif

static void echo_alloc(uv_handle_t* handle,
	size_t suggested_size,
	uv_buf_t* buf) {
	buf->base = (char*)malloc(suggested_size);
	buf->len = (ULONG)suggested_size;
}

static void close_cb(uv_handle_t* handle) {
	delete (uv_udp_t*)handle;
}

Udp::Udp(uv_loop_t* uv_loop,
			RC_CHANNEL channel,
			IPortHandler* handler,
			const UDPInfo& info)
	: IPort(channel, handler)
	, m_uv_loop(uv_loop)
	, m_Info(info)
{
	printf("Create UDP  	R:%s:%d L:%s:%d\n", 
		info.remote.sip,
		info.remote.port,
		info.bind.sip,
		info.bind.port);
}


Udp::~Udp()
{
}

bool Udp::Open()
{
	if (m_Info.remote.port != 0) {
		int rc = uv_ip4_addr(m_Info.remote.sip, m_Info.remote.port, &m_peer_addr);
		if (0 != rc) {
			RLOG("Incorrect UDP server address %d\n", rc);
			m_pHandler->OnOpen(m_nChannel, false);
			return false;
		}
	}
	struct sockaddr_in addr;

	int rc = uv_ip4_addr(m_Info.bind.sip, m_Info.bind.port, &addr);
	if (0 != rc) {
		RLOG("Incorrect UDP server address %d\n", rc);
		m_pHandler->OnOpen(m_nChannel, false);
		return false;
	}

	m_udp_handle = new uv_udp_t();
	rc = uv_udp_init(m_uv_loop, m_udp_handle);
	if (0 != rc) {
		delete m_udp_handle;
		RLOG("Cannot init UDP handle %d\n", rc);
		m_pHandler->OnOpen(m_nChannel, false);
		return false;
	}

	m_udp_handle->data = this;

	rc = uv_udp_bind(m_udp_handle, (const struct sockaddr*)&addr, UV_UDP_REUSEADDR);
	if (0 != rc) {
		uv_close((uv_handle_t*)m_udp_handle, close_cb);
		RLOG("Cannot bind UDP bind address %d\n", rc);
		m_pHandler->OnOpen(m_nChannel, false);
		return false;
	}

	rc = uv_udp_recv_start(m_udp_handle, echo_alloc, UdpRecvCB);
	if (0 != rc) {
		uv_close((uv_handle_t*)m_udp_handle, close_cb);
		RLOG("Cannot start UDP recv callback %d\n", rc);
		m_pHandler->OnOpen(m_nChannel, false);
		return false;
	}
	RLOG("Start UDP Done!\n");

	m_pHandler->OnOpen(m_nChannel, true);
	return true;
}

void Udp::Close()
{
	if (m_udp_handle) {
		uv_udp_recv_stop(m_udp_handle);
		uv_close((uv_handle_t*)m_udp_handle, close_cb);
	}
}

int Udp::Write(void * buf, size_t len)
{
	uv_udp_send_t* send_req = new uv_udp_send_t();
	uv_buf_t uvbuf = uv_buf_init((char*)buf, (unsigned int)len);
	int rc = uv_udp_send(send_req, m_udp_handle, &uvbuf, 1, (const struct sockaddr*)&m_peer_addr, SendCB);
	RLOG("%s Send len %llu returns %d\n", __FUNCTION__, len, rc);
	return rc;
}

void Udp::UdpRecvCB(uv_udp_t * handle, ssize_t nread, const uv_buf_t * buf, const sockaddr * addr, unsigned flags)
{
	if (nread < 0) {
		RLOG("recv_cb error: %s\n", uv_err_name((int)nread));
		assert(nread == UV_ECONNRESET || nread == UV_EOF);
		uv_close((uv_handle_t*)handle, close_cb);
	}
	else {
		Udp* pThis = (Udp*)handle->data;
		pThis->_UdpRecvCB(handle, nread, buf, addr, flags);
	}
}
void Udp::_UdpRecvCB(uv_udp_t * handle, ssize_t nread, const uv_buf_t * buf, const sockaddr * addr, unsigned flags)
{
	if (m_Info.remote.port != 0) {
		struct sockaddr_in* in_addr = (struct sockaddr_in*)addr;
		if (m_peer_addr.sin_port != in_addr->sin_port)
			return;
#ifndef RDC_LINUX_SYS
		if (m_peer_addr.sin_addr.S_un.S_addr != in_addr->sin_addr.S_un.S_addr)
#else
		if (m_peer_addr.sin_addr.s_addr != in_addr->sin_addr.s_addr)
#endif
			return;
	}
	else {
		memcpy(&m_peer_addr, (struct sockaddr_in*)addr, sizeof(sockaddr_in));
	}
	m_pHandler->OnRecv(m_nChannel, buf->base, nread);
}


void Udp::SendCB(uv_udp_send_t * req, int status)
{
	delete req;
	// FIXME:
}
