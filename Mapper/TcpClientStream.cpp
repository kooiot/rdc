#include "TcpClientStream.h"
#include <cassert>


static void echo_alloc(uv_handle_t* handle,
	size_t suggested_size,
	uv_buf_t* buf) {
	buf->base = (char*)malloc(suggested_size);
	buf->len = suggested_size;
}

TcpClientStream::TcpClientStream(uv_loop_t* uv_loop, ENetPeer* peer, const ConnectionInfo& info, int mask)
	: StreamPortBase(peer, info, mask)
	, m_uv_loop(uv_loop)
{
	printf("Create TCPClient Stream  C:%s:%d L:%s:%d\n", 
		info.TCPClient.remote.sip,
		info.TCPClient.remote.port,
		info.TCPClient.bind.sip,
		info.TCPClient.bind.port);
}


TcpClientStream::~TcpClientStream()
{
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
	if (0 != uv_read_start(req->handle, echo_alloc, ReadCB)) {

	}
	m_bConnected = true;

	int rc = FireEvent(SE_CHANNEL_OPENED);
	printf("Fire Opened %d\n", rc);
}

void TcpClientStream::ReadCB(uv_stream_t * stream, ssize_t nread, const uv_buf_t * buf)
{
	if (nread < 0) {
		fprintf(stderr, "read_cb error: %s\n", uv_err_name(nread));
		assert(nread == UV_ECONNRESET || nread == UV_EOF);
		uv_close((uv_handle_t*)stream, NULL);
		// TODO: Close?
	}
	else {
		TcpClientStream* pThis = (TcpClientStream*)stream->data;
		pThis->SendData((void*)buf, nread);
	}
}

void TcpClientStream::WriteCB(uv_write_t * req, int status)
{
}

void TcpClientStream::Start()
{
}

bool TcpClientStream::Open()
{
	printf("Open TCP Stream...\n");
	m_bConnected = false;
	struct sockaddr_in addr;

	int rc = uv_ip4_addr(m_Info.TCPClient.remote.sip, m_Info.TCPClient.remote.port, &addr);
	if (0 != rc) {
		FireEvent(SE_CHANNEL_OPEN_FAILED, "Incorrect TCP server address %d\n", rc);
		return false;
	}
	
	rc = uv_tcp_init(m_uv_loop, &m_tcp_handle);
	if (0 != rc) {
		FireEvent(SE_CHANNEL_OPEN_FAILED, "Cannot Init TCP handle %d\n", rc);
		return false;
	}
	m_tcp_handle.data = this;
	m_connect_req.data = this;

	if (m_Info.TCPClient.bind.port != 0) {
		struct sockaddr_in bind_addr;
		rc = uv_ip4_addr(m_Info.TCPClient.bind.sip, m_Info.TCPClient.bind.port, &bind_addr);
		if (0 != rc) {
			FireEvent(SE_CHANNEL_OPEN_FAILED, "Incorrect TCP bind address %d\n", rc);
			return false;
		}
		rc = uv_tcp_bind(&m_tcp_handle, (const struct sockaddr*)&bind_addr, 0);
		if (0 != rc) {
			FireEvent(SE_CHANNEL_OPEN_FAILED, "Cannot Bind TCP to bind ip %d\n", rc);
			return false;
		}
	}

	rc = uv_tcp_connect(&m_connect_req, &m_tcp_handle, (const struct sockaddr*) &addr, ConnectCB);
	if (0 != rc) {
		FireEvent(SE_CHANNEL_OPEN_FAILED, "Cannot Connect TCP to server %d\n", rc);
		return false;
	}

	return false;
}

void TcpClientStream::Close()
{
	uv_read_stop((uv_stream_t*)&m_tcp_handle);
	uv_close((uv_handle_t*)&m_tcp_handle, NULL);
	FireEvent(SE_CHANNEL_CLOSED);
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
