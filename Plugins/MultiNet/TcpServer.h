#pragma once
#include "Handlers.h"
#include <uv.h>

class TcpServer : public IStreamHandler
{
public:
	TcpServer(uv_loop_t* uv_loop,
		int channel,
		IPortHandler& Handler,
		const TCPServerInfo& Info);
	~TcpServer();

public:
	virtual bool Open();
	virtual void Close();

	virtual int OnData(void* buf, size_t len);
	virtual int OnEvent(StreamEvent evt);

private:
	static void ConnectionCB(uv_stream_t* remote, int status);
	void _ConnectionCB(uv_stream_t* remote, int status);
	static void ReadCB(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
	static void WriteCB(uv_write_t* req, int status);

private:
	int m_nChannel;
	IPortHandler& m_Handler;
	TCPServerInfo m_Info;

	uv_loop_t * m_uv_loop;
	uv_tcp_t* m_tcp_server;
	uv_tcp_t* m_tcp_client;
};

