#pragma once

#include "DataDefs.h"
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

	IPort* Create(RC_CHANNEL channel, IPortHandler* handler, const ConnectionInfo& info);
	void Destory(IPort* port);

private:
	uv_loop_t * m_uv_loop;
	std::thread* m_thread;
	bool m_abort;
};

