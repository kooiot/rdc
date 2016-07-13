#include "stdafx.h"
#include "UVTcpServer.h"



UVTcpServer::UVTcpServer(uv_loop_t* uv_loop,
	RC_CHANNEL channel,
	IPortHandler& Handler,
	const TCPClientInfo& Info)
	: m_uv_loop(uv_loop), m_nChannel(channel), m_Handler(Handler), m_Info(Info),
	m_tcp_client(NULL)
{
}


UVTcpServer::~UVTcpServer()
{
}

void UVTcpServer::ConnectionCB(uv_stream_t* server, int status) {
	UVTcpServer* pThis = (UVTcpServer*)server->data;
	pThis->_ConnectionCB(server, status);
}

void UVTcpServer::_ConnectionCB(uv_stream_t* server, int status)
{
	if (0 != status) {
		// Failed
		return;
	}
	if (NULL != m_tcp_client) {
		uv_close((uv_handle_t*)m_tcp_client, NULL);
		delete m_tcp_client;
	}

	uv_tcp_t* client = new uv_tcp_t();
	if (0 != uv_tcp_init(m_uv_loop, client)) {
		return;
	}

	int rc = uv_accept(server, (uv_stream_t*)client);
	if (rc != 0) {
		return;
	}

	if (m_Info.server.port != 0) {
		struct sockaddr_in in_addr;
		int rs = sizeof(in_addr);
		uv_tcp_getpeername(client, (sockaddr*)&in_addr, &rs);
	/*	int rc = uv_ip4_addr(m_Info.local.sip, m_Info.local.port, &m_peer);
		if (peer_addr.sin_port != in_addr->sin_port)
			return;
		if (peer_addr.sin_addr.S_un.S_addr != in_addr->sin_addr.S_un.S_addr)
			return;*/
	}

	rc = uv_read_start((uv_stream_t*)client, NULL, ReadCB);
	if (rc != 0) {

	}
	m_tcp_client = client;
}

void UVTcpServer::ReadCB(uv_stream_t * stream, ssize_t nread, const uv_buf_t * buf)
{
	UVTcpServer* pThis = (UVTcpServer*)stream->data;
	pThis->m_Handler.Send(pThis->m_nChannel, buf->base, buf->len);
}

void UVTcpServer::WriteCB(uv_write_t * req, int status)
{
}

void UVTcpServer::Start()
{
}

bool UVTcpServer::Open()
{
	struct sockaddr_in addr;

	int rc = uv_ip4_addr(m_Info.server.sip, m_Info.server.port, &addr);
	if (0 != rc) {
		printf("Incorrect TCP server address %d\n", rc);
		return false;
	}
	
	rc = uv_tcp_init(m_uv_loop, &m_tcp_server);
	if (0 != rc) {
		printf("Cannot Init TCP handle %d\n", rc);
		return false;
	}
	m_tcp_server.data = this;
	m_connect_req.data = this;

	if (m_Info.local.port != 0) {
		struct sockaddr_in bind_addr;
		rc = uv_ip4_addr(m_Info.local.sip, m_Info.local.port, &bind_addr);
		if (0 != rc) {
			printf("Incorrect TCP local address %d\n", rc);
			return false;
		}
		rc = uv_tcp_bind(&m_tcp_server, (const struct sockaddr*)&bind_addr, 0);
		if (0 != rc) {
			printf("Cannot Bind TCP to local ip %d\n", rc);
			return false;
		}
	}

	rc = uv_listen((uv_stream_t*)&m_tcp_server, 16, ConnectionCB);

	if (0 != rc) {
		printf("Cannot Connect TCP to server %d\n", rc);
		return false;
	}

	return false;
}

void UVTcpServer::Close()
{
	if (NULL != m_tcp_client) {
		uv_close((uv_handle_t*)m_tcp_client, NULL);
		delete m_tcp_client;
	}

	uv_read_stop((uv_stream_t*)&m_tcp_server);
	uv_close((uv_handle_t*)&m_tcp_server, NULL);
}

int UVTcpServer::OnWrite(void * data, size_t len)
{
	if (m_tcp_client == NULL) {
		return -1;
	}

	uv_buf_t buf = uv_buf_init((char*)data, len);
	int rc = uv_write(&m_write_req,
		(uv_stream_t*)&m_tcp_client,
		&buf,
		1,
		WriteCB);
	return rc;
}
