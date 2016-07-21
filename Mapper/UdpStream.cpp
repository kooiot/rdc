#include "UdpStream.h"
#include <cassert>

static void echo_alloc(uv_handle_t* handle,
	size_t suggested_size,
	uv_buf_t* buf) {
	buf->base = (char*)malloc(suggested_size);
	buf->len = suggested_size;
}

static void close_cb(uv_handle_t* handle) {
}

UdpStream::UdpStream(uv_loop_t* uv_loop, StreamPortInfo& info)
	: StreamPortBase(info)
	, m_uv_loop(uv_loop)
{
	printf("Create UDP Stream  C:%s:%d L:%s:%d\n",
		info.ConnInfo.UDP.remote.sip,
		info.ConnInfo.UDP.remote.port,
		info.ConnInfo.UDP.bind.sip,
		info.ConnInfo.UDP.bind.port);
}


UdpStream::~UdpStream()
{
}

void UdpStream::UdpRecvCB(uv_udp_t * handle, ssize_t nread, const uv_buf_t * buf, const sockaddr * addr, unsigned flags)
{
	printf("%s Got len %d", __FUNCTION__, nread);
	UdpStream* pThis = (UdpStream*)handle->data;
	if (nread < 0) {
		fprintf(stderr, "read_cb error: %s\n", uv_err_name(nread));
		assert(nread == UV_ECONNRESET || nread == UV_EOF);
		pThis->OnStreamClose();
	}
	else {
		pThis->SendData((void*)buf, nread);
	}
}

void UdpStream::SendCB(uv_udp_send_t * req, int status)
{
	printf("%s Got status %d", __FUNCTION__, status);
	// FIXME:
}

void UdpStream::Start()
{
}


bool UdpStream::Open()
{
	printf("Open TCP Stream...\n");
	m_bConnected = false;
	struct sockaddr_in addr;

	int rc = uv_ip4_addr(m_Info.ConnInfo.UDP.remote.sip, m_Info.ConnInfo.UDP.remote.port, &m_peer_addr);
	if (0 != rc) {
		FireEvent(SE_CHANNEL_OPEN_FAILED,"Incorrect UDP server address %d\n", rc);
		return false;
	}

	rc = uv_udp_init(m_uv_loop, &m_udp_handle);
	if (0 != rc) {
		FireEvent(SE_CHANNEL_OPEN_FAILED,"Cannot init UDP handle %d\n", rc);
		return false;
	}

	m_udp_handle.data = this;

	if (m_Info.ConnInfo.UDP.bind.port != 0) {
		rc = uv_ip4_addr(m_Info.ConnInfo.UDP.bind.sip, m_Info.ConnInfo.UDP.bind.port, &addr);
		if (0 != rc) {
			uv_close((uv_handle_t*)&m_udp_handle, close_cb);
			FireEvent(SE_CHANNEL_OPEN_FAILED,"Incorrect UDP bind address %d\n", rc);
			return false;
		}

		rc = uv_udp_bind(&m_udp_handle, (const struct sockaddr*)&addr, UV_UDP_REUSEADDR);
		if (0 != rc) {
			uv_close((uv_handle_t*)&m_udp_handle, close_cb);
			FireEvent(SE_CHANNEL_OPEN_FAILED,"Cannot bind UDP bind address %d\n", rc);
			return false;
		}
	}

	rc = uv_udp_recv_start(&m_udp_handle, echo_alloc, UdpRecvCB);
	if (0 != rc) {
		uv_close((uv_handle_t*)&m_udp_handle, close_cb);
		FireEvent(SE_CHANNEL_OPEN_FAILED,"Cannot start UDP recv callback %d\n", rc);
		return false;
	}

	rc = FireEvent(SE_CHANNEL_OPENED);
	printf("%s Fire Opened %d\n", __FUNCTION__, rc);
	return rc == 0;
}

void UdpStream::Close()
{
	uv_udp_recv_stop(&m_udp_handle);
	uv_close((uv_handle_t*)&m_udp_handle, close_cb);
	FireEvent(SE_CHANNEL_CLOSED);
}

int UdpStream::OnWrite(void * data, size_t len)
{
	printf("%s Send len %d", __FUNCTION__, len);

	uv_buf_t buf = uv_buf_init((char*)data, len);
	int rc = uv_udp_send(&m_udp_send_req, &m_udp_handle, &buf, len, (const struct sockaddr*)&m_peer_addr, SendCB);
	return rc;
}
