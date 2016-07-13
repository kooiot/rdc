#pragma once
#include "Handlers.h"
#include <uv.h>

class UVTcpServer : public IStreamHandler
{
public:
	UVTcpServer(uv_loop_t* uv_loop,
		RC_CHANNEL channel,
		IPortHandler& Handler,
		const TCPClientInfo& Info);
	~UVTcpServer();

public:
	virtual void Start();

	virtual bool Open();
	virtual void Close();

	virtual int OnWrite(void* data, size_t len);

private:
	static void ConnectionCB(uv_stream_t* server, int status);
	void _ConnectionCB(uv_stream_t* server, int status);
	static void ReadCB(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
	static void WriteCB(uv_write_t* req, int status);

private:
	RC_CHANNEL m_nChannel;
	IPortHandler& m_Handler;
	TCPClientInfo m_Info;

	uv_loop_t * m_uv_loop;
	uv_tcp_t m_tcp_server;
	uv_tcp_t* m_tcp_client;
	uv_connect_t m_connect_req;
	uv_write_t m_write_req;
};

