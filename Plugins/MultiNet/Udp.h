#pragma once
#include "Handlers.h"
#include <uv.h>
#include <string>

class Udp : public IStreamHandler
{
public:
	Udp(uv_loop_t* uv_loop,
		int channel,
		IPortHandler& Handler,
		const UDPInfo& Info);
	~Udp();

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
	int m_nChannel;
	IPortHandler& m_Handler;
	UDPInfo m_Info;

	uv_loop_t * m_uv_loop;
	uv_udp_t* m_udp_handle;
	struct sockaddr_in m_peer_addr;
};

