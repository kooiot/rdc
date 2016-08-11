#pragma once

#include "Handlers.h"
#include <string>
#include <thread>
#include <uv.h>

class UdpPort;
class TcpPort;
class CPortMgr
{
public:
	CPortMgr();
	~CPortMgr();

	int Init();
	void Close();

	UVUdpPort* CreateUdpPort(int channel, IPortHandler& handler, const std::string& name);
	void FreeUdpPort(UVUdpPort* port);
	UVTcpPort* CreateTcpPort(int channel, IPortHandler& handler, const std::string& name);
	void FreeTcpPort(UVTcpPort* port);

private:
	uv_loop_t * m_uv_loop;
	std::thread* m_thread;
	bool m_abort;
};

