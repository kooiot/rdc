#include "TcpClient.h"
#include <cassert>

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
	delete (uv_tcp_t*)handle;
}

TcpClient::TcpClient(uv_loop_t* uv_loop,
			RC_CHANNEL channel,
	       	IPortHandler* handler,
		const TCPClientInfo& info)
	: IPort(channel, handler)
	, m_bConnected(false)
	, m_uv_loop(uv_loop)
	, m_tcp_handle(NULL)
	, m_Info(info)
{
	RLOG("Create TCPClient   C:%s:%d L:%s:%d\n",
		info.remote.sip,
		info.remote.port,
		info.bind.sip,
		info.bind.port);
}


TcpClient::~TcpClient()
{
}

void TcpClient::ConnectCB(uv_connect_t* req, int status) {
	TcpClient* pThis = (TcpClient*)req->data;
	pThis->_ConnectCB(req, status);
}

void TcpClient::_ConnectCB(uv_connect_t * req, int status)
{
	uv_stream_t *handle = req->handle;
	delete req;

	if (0 != status) {
		RLOG("Connect to TCP server failed, status %d\n", status);
		m_pHandler->OnOpen(m_nChannel, false);
		OnClose();
		return;
	}
	if (0 != uv_read_start(handle, echo_alloc, ReadCB)) {
		RLOG("Connect Start Read on socket\n");
		m_pHandler->OnOpen(m_nChannel, false);
		OnClose();
		return;
	}
	RLOG("Connect to TCP server sucessed\n");
	m_bConnected = true;
	m_pHandler->OnOpen(m_nChannel, true);
}

void TcpClient::ReadCB(uv_stream_t * stream, ssize_t nread, const uv_buf_t * buf)
{
	RLOG("%s Got len %lld\n", __FUNCTION__, nread);
	TcpClient* pThis = (TcpClient*)stream->data;
	if (nread < 0) {
		RLOG("read_cb error: %s\n", uv_err_name((int)nread));
		assert(nread == UV_ECONNRESET || nread == UV_EOF);
		pThis->OnClose();
	}
	else {
		pThis->m_pHandler->OnRecv(pThis->m_nChannel, buf->base, nread);
	}
}

void TcpClient::WriteCB(uv_write_t * req, int status)
{
	RLOG("%s Got status %d\n", __FUNCTION__, status);
	delete req;
}

void TcpClient::Start()
{
}

bool TcpClient::Open()
{
	RLOG("Open TCP Client ...\n");
	m_bConnected = false;
	struct sockaddr_in addr;

	int rc = uv_ip4_addr(m_Info.remote.sip, m_Info.remote.port, &addr);
	if (0 != rc) {
		RLOG("Incorrect TCP server address %d\n", rc);
		m_pHandler->OnOpen(m_nChannel, false);
		return false;
	}
	
	m_tcp_handle = new uv_tcp_t();
	rc = uv_tcp_init(m_uv_loop, m_tcp_handle);
	if (0 != rc) {
		delete m_tcp_handle;
		m_tcp_handle = NULL;
		RLOG("Cannot Init TCP handle %d\n", rc);
		m_pHandler->OnOpen(m_nChannel, false);
		return false;
	}
	m_tcp_handle->data = this;

	if (m_Info.bind.port != 0) {
		struct sockaddr_in bind_addr;
		rc = uv_ip4_addr(m_Info.bind.sip, m_Info.bind.port, &bind_addr);
		if (0 != rc) {
			uv_close((uv_handle_t*)m_tcp_handle, close_cb);
			RLOG("Incorrect TCP bind address %d\n", rc);
			m_pHandler->OnOpen(m_nChannel, false);
			return false;
		}
		rc = uv_tcp_bind(m_tcp_handle, (const struct sockaddr*)&bind_addr, 0);
		if (0 != rc) {
			uv_close((uv_handle_t*)m_tcp_handle, close_cb);
			RLOG("Cannot Bind TCP to bind ip %d\n", rc);
			m_pHandler->OnOpen(m_nChannel, false);
			return false;
		}
	}

	uv_connect_t *connect_req = new uv_connect_t();
	connect_req->data = this;
	rc = uv_tcp_connect(connect_req, m_tcp_handle, (const struct sockaddr*) &addr, ConnectCB);
	if (0 != rc) {
		uv_close((uv_handle_t*)m_tcp_handle, close_cb);
		RLOG("Cannot Connect TCP to server %d\n", rc);
		m_pHandler->OnOpen(m_nChannel, false);
		return false;
	}
	uv_tcp_nodelay(m_tcp_handle, 1);

	return true;
}

void TcpClient::Close()
{
	if (m_tcp_handle) {
		uv_read_stop((uv_stream_t*)m_tcp_handle);
		uv_close((uv_handle_t*)m_tcp_handle, close_cb);
	}
}

int TcpClient::Write(void * data, size_t len)
{
	uv_write_t* write_req = new uv_write_t();
	uv_buf_t buf = uv_buf_init((char*)data, (unsigned int)len);
	int rc = uv_write(write_req,
		(uv_stream_t*)m_tcp_handle,
		&buf,
		1,
		WriteCB);
	RLOG("%s Send len %llu returns %d\n", __FUNCTION__, len, rc);
	return rc;
}
