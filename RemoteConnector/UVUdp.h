#pragma once
#include "Handlers.h"
#include <uv.h>
#include <string>

class UVUdp : public IStreamHandler
{
public:
	UVUdp(uv_loop_t* uv_loop,
		RC_CHANNEL channel,
		IPortHandler& Handler,
		const UDPInfo& Info);
	~UVUdp();

public:
	virtual bool Open();
	virtual void Close();

	virtual int OnData(void* buf, size_t len);
	virtual int OnEvent(StreamEvent evt);

private:
	static void UdpRecvCB(uv_udp_t* handle,
		ssize_t nread,
		const uv_buf_t* buf,
		const struct sockaddr* addr,
		unsigned flags);
	void _UdpRecvCB(uv_udp_t* handle,
		ssize_t nread,
		const uv_buf_t* buf,
		const struct sockaddr* addr,
		unsigned flags);
	static void SendCB(uv_udp_send_t* req, int status);

private:
	RC_CHANNEL m_nChannel;
	IPortHandler& m_Handler;
	UDPInfo m_Info;

	uv_loop_t * m_uv_loop;
	uv_udp_t m_udp_handle;
	uv_udp_send_t m_udp_send_req;
	struct sockaddr_in m_peer_addr;
};

