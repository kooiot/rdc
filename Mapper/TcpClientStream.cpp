#include "TcpClientStream.h"



TcpClientStream::TcpClientStream(uv_loop_t* uv_loop, ENetPeer* peer, const ConnectionInfo& info, int mask)
	: StreamPortBase(peer, info, mask)
	, m_uv_loop(uv_loop)
{
}


TcpClientStream::~TcpClientStream()
{
}

static void fail_cb(void) {
	printf("%s\n", "fail_cb called");
}

void TcpClientStream::ConnectCB(uv_connect_t* req, int status) {
	TcpClientStream* pThis = (TcpClientStream*)req->data;
	pThis->_ConnectCB(req, status);
}

void TcpClientStream::_ConnectCB(uv_connect_t * req, int status)
{
	if (0 != status) {
		// Failed
		return;
	}
	if (0 != uv_read_start(req->handle, (uv_alloc_cb)fail_cb, (uv_read_cb)ReadCB)) {

	}
	m_bConnected = true;

	int rc = OnOpened();
}

void TcpClientStream::ReadCB(uv_stream_t * stream, ssize_t nread, const uv_buf_t * buf)
{
	TcpClientStream* pThis = (TcpClientStream*)stream->data;
	pThis->SendData((void*)buf, nread);
}

void TcpClientStream::WriteCB(uv_write_t * req, int status)
{
}

void TcpClientStream::Start()
{
}

bool TcpClientStream::Open()
{
	m_bConnected = false;
	struct sockaddr_in addr;

	if (0 != uv_ip4_addr(m_Info.TCPClient.server.sip, m_Info.TCPClient.server.port, &addr)) {
		return false;
	}
	
	if (0 != uv_tcp_init(m_uv_loop, &m_tcp_handle)) {
		return false;
	}
	m_tcp_handle.data = this;
	m_connect_req.data = this;
	int rc = uv_tcp_connect(&m_connect_req, &m_tcp_handle, (const struct sockaddr*) &addr, ConnectCB);
	if (0 != rc) {
		return false;
	}

	return false;
}

void TcpClientStream::Close()
{
	uv_close((uv_handle_t*)&m_tcp_handle, NULL);
	OnClosed();
}

int TcpClientStream::OnWrite(void * data, size_t len)
{
	uv_buf_t buf = uv_buf_init((char*)data, len);
	int rc = uv_write(&m_write_req,
		(uv_stream_t*)&m_tcp_handle,
		&buf,
		1,
		WriteCB);
	return rc;
}
