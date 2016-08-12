#include "TcpClient.h"
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

TcpClient::TcpClient(uv_loop_t* uv_loop, PortInfo& info)
	: PortBase(info)
	, m_uv_loop(uv_loop)
{
	printf("Create TCPClient   C:%s:%d L:%s:%d\n", 
		info.ConnInfo.TCPClient.remote.sip,
		info.ConnInfo.TCPClient.remote.port,
		info.ConnInfo.TCPClient.bind.sip,
		info.ConnInfo.TCPClient.bind.port);
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
	uv_handle_t *handle = req->handle;
	delete req;

	if (0 != status) {
		FireEvent(SE_CHANNEL_OPEN_FAILED, "Connect to TCP server failed, status %d", status);
		OnClose();
		return;
	}
	if (0 != uv_read_start(handle, echo_alloc, ReadCB)) {
		FireEvent(SE_CHANNEL_OPEN_FAILED, "Connect Start Read on socket");
		OnClose();
		return;
	}
	m_bConnected = true;

	int rc = FireEvent(SE_CHANNEL_OPENED);
	printf("Fire Opened %d\n", rc);
}

void TcpClient::ReadCB(uv_stream_t * stream, ssize_t nread, const uv_buf_t * buf)
{
	printf("%s Got len %d\n", __FUNCTION__, nread);
	TcpClient* pThis = (TcpClient*)stream->data;
	if (nread < 0) {
		fprintf(stderr, "read_cb error: %s\n", uv_err_name(nread));
		assert(nread == UV_ECONNRESET || nread == UV_EOF);
		pThis->OnClose();
	}
	else {
		pThis->SendData(buf->base, nread);
	}
}

void TcpClient::WriteCB(uv_write_t * req, int status)
{
	printf("%s Got status %d\n", __FUNCTION__, status);
	delete req;
}

void TcpClient::Start()
{
}

bool TcpClient::Open()
{
	printf("Open TCP ...\n");
	m_bConnected = false;
	struct sockaddr_in addr;

	int rc = uv_ip4_addr(m_Info.ConnInfo.TCPClient.remote.sip, m_Info.ConnInfo.TCPClient.remote.port, &addr);
	if (0 != rc) {
		FireEvent(SE_CHANNEL_OPEN_FAILED, "Incorrect TCP server address %d\n", rc);
		return false;
	}
	
	m_tcp_handle = new uv_tcp_t();
	rc = uv_tcp_init(m_uv_loop, m_tcp_handle);
	if (0 != rc) {
		delete m_tcp_handle;
		m_tcp_handle = NULL;
		FireEvent(SE_CHANNEL_OPEN_FAILED, "Cannot Init TCP handle %d\n", rc);
		return false;
	}
	m_tcp_handle->data = this;

	if (m_Info.ConnInfo.TCPClient.bind.port != 0) {
		struct sockaddr_in bind_addr;
		rc = uv_ip4_addr(m_Info.ConnInfo.TCPClient.bind.sip, m_Info.ConnInfo.TCPClient.bind.port, &bind_addr);
		if (0 != rc) {
			uv_close((uv_handle_t*)m_tcp_handle, close_cb);
			FireEvent(SE_CHANNEL_OPEN_FAILED, "Incorrect TCP bind address %d\n", rc);
			return false;
		}
		rc = uv_tcp_bind(m_tcp_handle, (const struct sockaddr*)&bind_addr, 0);
		if (0 != rc) {
			uv_close((uv_handle_t*)m_tcp_handle, close_cb);
			FireEvent(SE_CHANNEL_OPEN_FAILED, "Cannot Bind TCP to bind ip %d\n", rc);
			return false;
		}
	}

	uv_connect_t *connect_req = new uv_connect_t();
	connect_req->data = this;
	rc = uv_tcp_connect(connect_req, m_tcp_handle, (const struct sockaddr*) &addr, ConnectCB);
	if (0 != rc) {
		uv_close((uv_handle_t*)m_tcp_handle, close_cb);
		FireEvent(SE_CHANNEL_OPEN_FAILED, "Cannot Connect TCP to server %d\n", rc);
		return false;
	}
	uv_tcp_nodelay(m_tcp_handle, 1);

	return false;
}

void TcpClient::Close()
{
	uv_read_stop((uv_stream_t*)m_tcp_handle);
	uv_close((uv_handle_t*)m_tcp_handle, close_cb);
	FireEvent(SE_CHANNEL_CLOSED);
}

int TcpClient::Write(void * data, size_t len)
{
	printf("%s Send len %d\n", __FUNCTION__, len);
	uv_write_t* write_req = new uv_write_t();
	uv_buf_t buf = uv_buf_init((char*)data, len);
	int rc = uv_write(write_req,
		(uv_stream_t*)m_tcp_handle,
		&buf,
		1,
		WriteCB);
	return rc;
}
