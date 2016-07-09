#pragma once
#include "StreamPort.h"
#include <uv.h>

class TcpClientStream : public StreamPortBase
{
public:
	TcpClientStream(uv_loop_t* uv_loop, ENetPeer* peer, const ConnectionInfo& info, int mask);
	~TcpClientStream();

public:
	virtual void Start();

	virtual bool Open();
	virtual void Close();

	virtual int OnWrite(void* data, size_t len);

private:
	static void ConnectCB(uv_connect_t* req, int status);
	void _ConnectCB(uv_connect_t* req, int status);
	static void ReadCB(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
	static void WriteCB(uv_write_t* req, int status);

private:
	bool m_bConnected;
	uv_loop_t * m_uv_loop;
	uv_tcp_t m_tcp_handle;
	uv_connect_t m_connect_req;
	uv_write_t m_write_req;
};

