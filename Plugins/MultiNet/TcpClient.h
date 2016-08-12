#pragma once
#include "Handlers.h"
#include <uv.h>

class TcpClient : public IPort 
{
public:
	TcpClient(uv_loop_t* uv_loop, int channel, IPortHandler* handler, const TCPClientInfo& info);
	~TcpClient();

public:
	virtual void Start();

	virtual bool Open();
	virtual void Close();

	virtual int Write(void* data, size_t len);

private:
	static void ConnectCB(uv_connect_t* req, int status);
	void _ConnectCB(uv_connect_t* req, int status);
	static void ReadCB(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
	static void WriteCB(uv_write_t* req, int status);

private:
	bool m_bConnected;
	uv_loop_t * m_uv_loop;
	uv_tcp_t* m_tcp_handle;
	TCPClientInfo m_Info;
};

