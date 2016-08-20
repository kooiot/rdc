#pragma once
#include "DataDefs.h"
#include "Handlers.h"
#include <uv.h>
#include <string>

class Udp : public IPort
{
public:
	Udp(uv_loop_t* uv_loop,
		RC_CHANNEL channel,
		IPortHandler* handler,
		const UDPInfo& info);
	~Udp();

public:
	virtual bool Open();
	virtual void Close();

	virtual int Write(void* buf, size_t len);

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
	UDPInfo m_Info;

	uv_loop_t * m_uv_loop;
	uv_udp_t* m_udp_handle;
	struct sockaddr_in m_peer_addr;
};

