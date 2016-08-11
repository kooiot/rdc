#include "stdafx.h"
#include "TcpServer.h"
#include <cassert>

static void echo_alloc(uv_handle_t* handle,
	size_t suggested_size,
	uv_buf_t* buf) {
	buf->base = (char*)malloc(suggested_size);
	buf->len = suggested_size;
}

static void close_cb(uv_handle_t* handle) {
	delete (uv_tcp_t*)handle;
}


TcpServer::TcpServer(uv_loop_t* uv_loop,
	int channel,
	IPortHandler& Handler,
	const TCPServerInfo& Info)
	: m_uv_loop(uv_loop), m_nChannel(channel), m_Handler(Handler), m_Info(Info),
	m_tcp_client(NULL)
{
	m_tcp_server = new uv_tcp_t();
}


TcpServer::~TcpServer()
{
}

void TcpServer::ConnectionCB(uv_stream_t* remote, int status) {
	TcpServer* pThis = (TcpServer*)remote->data;
	pThis->_ConnectionCB(remote, status);
}

void TcpServer::_ConnectionCB(uv_stream_t* remote, int status)
{
	if (0 != status) {
		// Failed
		return;
	}
	if (NULL != m_tcp_client) {
		uv_close((uv_handle_t*)m_tcp_client, close_cb);
		delete m_tcp_client;
	}

	uv_tcp_t* client = new uv_tcp_t();
	if (0 != uv_tcp_init(m_uv_loop, client)) {
		return;
	}

	int rc = uv_accept(remote, (uv_stream_t*)client);
	if (rc != 0) {
		return;
	}

	rc = uv_read_start((uv_stream_t*)client, echo_alloc, ReadCB);
	if (rc != 0) {

	}
	client->data = this;
	m_tcp_client = client;
}

void TcpServer::ReadCB(uv_stream_t * stream, ssize_t nread, const uv_buf_t * buf)
{
	if (nread < 0) {
		RLOG("read_cb error: %s\n", uv_err_name(nread));
		assert(nread == UV_ECONNRESET || nread == UV_EOF);
		uv_close((uv_handle_t*)stream, close_cb);
	}
	else {
		TcpServer* pThis = (TcpServer*)stream->data;
		pThis->m_Handler.Send(pThis->m_nChannel, buf->base, nread);
	}
}

void TcpServer::WriteCB(uv_write_t * req, int status)
{
	delete req;
}

bool TcpServer::Open()
{
	struct sockaddr_in addr;

	RLOG("TCP Server bind on address %s:%d\n", m_Info.bind.sip, m_Info.bind.port);

	int rc = uv_ip4_addr(m_Info.bind.sip, m_Info.bind.port, &addr);
	if (0 != rc) {
		RLOG("Incorrect TCP server address %d\n", rc);
		return false;
	}
	
	rc = uv_tcp_init(m_uv_loop, m_tcp_server);
	if (0 != rc) {
		RLOG("Cannot Init TCP handle %d\n", rc);
		return false;
	}
	m_tcp_server->data = this;

	rc = uv_tcp_bind(m_tcp_server, (const struct sockaddr*)&addr, 0);
	if (0 != rc) {
		RLOG("Cannot Bind TCP to bind ip %d\n", rc);
		return false;
	}

	rc = uv_listen((uv_stream_t*)m_tcp_server, 16, ConnectionCB);

	if (0 != rc) {
		RLOG("Cannot Connect TCP to server %d\n", rc);
		return false;
	}

	return true;
}

void TcpServer::Close()
{
	RLOG("Close called\n");
	if (NULL != m_tcp_client) {
		uv_close((uv_handle_t*)m_tcp_client, close_cb);
	}

	uv_close((uv_handle_t*)m_tcp_server, close_cb);
}

int TcpServer::OnData(void * buf, size_t len)
{
	RLOG("OnData called len %d\n", len);
	if (m_tcp_client == NULL) {
		return -1;
	}

	uv_write_t* write_req = new uv_write_t();
	uv_buf_t uvbuf = uv_buf_init((char*)buf, len);
	int rc = uv_write(write_req,
		(uv_stream_t*)&m_tcp_client,
		&uvbuf,
		1,
		WriteCB);
	return rc;
}

int TcpServer::OnEvent(StreamEvent evt)
{
	RLOG("OnEvent evt %d\n", evt);
	return 0;
}
