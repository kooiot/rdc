#pragma once

#include "Handlers.h"
#include <string>
#include <thread>
#include <uv.h>

class UVUdpPort;
class UVTcpPort;
class CUVMgr
{
public:
	CUVMgr();
	~CUVMgr();

	int Init();
	void Close();

	UVUdpPort* CreateUdpPort(RC_CHANNEL channel, IPortHandler& handler, const std::string& name);
	void FreeUdpPort(UVUdpPort* port);
	UVTcpPort* CreateTcpPort(RC_CHANNEL channel, IPortHandler& handler, const std::string& name);
	void FreeTcpPort(UVTcpPort* port);

private:
	uv_loop_t * m_uv_loop;
	std::thread* m_thread;
	bool m_abort;
};

