#include "TcpClientStream.h"



TcpClientStream::TcpClientStream(uv_loop_t* uv_loop, ENetPeer* peer, const ConnectionInfo& info, int mask)
	: StreamPortBase(peer, info, mask)
	, m_uv_loop(uv_loop)
{
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
	if (0 != uv_read_start(req->handle, NULL, ReadCB)) {

	}
	m_bConnected = true;

	int rc = FireEvent(SE_CHANNEL_OPENED);
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

	int rc = uv_ip4_addr(m_Info.TCPClient.server.sip, m_Info.TCPClient.server.port, &addr);
	if (0 != rc) {
		printf("Incorrect TCP server address %d\n", rc);
		return false;
	}
	
	rc = uv_tcp_init(m_uv_loop, &m_tcp_handle);
	if (0 != rc) {
		printf("Cannot Init TCP handle %d\n", rc);
		return false;
	}
	m_tcp_handle.data = this;
	m_connect_req.data = this;

	if (m_Info.TCPClient.local.port != 0) {
		struct sockaddr_in bind_addr;
		rc = uv_ip4_addr(m_Info.TCPClient.local.sip, m_Info.TCPClient.local.port, &bind_addr);
		if (0 != rc) {
			printf("Incorrect TCP local address %d\n", rc);
			return false;
		}
		rc = uv_tcp_bind(&m_tcp_handle, (const struct sockaddr*)&bind_addr, 0);
		if (0 != rc) {
			printf("Cannot Bind TCP to local ip %d\n", rc);
			return false;
		}
	}

	rc = uv_tcp_connect(&m_connect_req, &m_tcp_handle, (const struct sockaddr*) &addr, ConnectCB);
	if (0 != rc) {
		printf("Cannot Connect TCP to server %d\n", rc);
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
