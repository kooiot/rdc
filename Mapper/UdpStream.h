#pragma once
#include "StreamPort.h"
#include <uv.h>

class UdpStream : public StreamPortBase
{
public:
	UdpStream(uv_loop_t* uv_loop, ENetPeer* peer, const ConnectionInfo& info, int mask);
	~UdpStream();

public:
	virtual void Start();

	virtual bool Open();
	virtual void Close();

	virtual int OnWrite(void* data, size_t len);

private:
	static void UdpRecvCB(uv_udp_t* handle,
		ssize_t nread,
		const uv_buf_t* buf,
		const struct sockaddr* addr,
		unsigned flags);
	static void SendCB(uv_udp_send_t* req, int status);

private:
	bool m_bConnected;
	uv_loop_t * m_uv_loop;
	uv_udp_t m_udp_handle;
	uv_udp_send_t m_udp_send_req;
	struct sockaddr_in m_peer_addr;
};

