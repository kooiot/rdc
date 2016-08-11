#include "stdafx.h"
#include "PortMgr.h"
#include <DataJson.h>
#include "Udp.h"
//#include "TcpClient.h"
#include "TcpServer.h"

CPortMgr::CPortMgr()
	: m_uv_loop(NULL)
	, m_thread(NULL)
	, m_abort(false)
{
}


CPortMgr::~CPortMgr()
{
}

CPortMgr & CPortMgr::Instance()
{
	static CPortMgr obj;
	return obj;
}

int CPortMgr::Init()
{
	if (m_thread)
		return 0;

	m_uv_loop = uv_default_loop();
	m_thread = new std::thread([this](void) {
		while (!m_abort) {
			uv_run(m_uv_loop, UV_RUN_ONCE);
		}
	});
	return 0;
}

void CPortMgr::Close()
{
	m_abort = true;
	uv_loop_close(m_uv_loop);
	if (m_thread && m_thread->joinable()) {
		m_thread->join();
	}
}
IPort * CPortMgr::Create(int channel, IPortHandler* handler, const ConnectionInfo& info)
{
	IPort* port = nullptr;
	switch (info.Type) {
	case CT_UDP:
		port = new Udp(m_uv_loop, channel, handler, info.UDP);
		break;
	case CT_TCPC:
		break;
	case CT_TCPS:
		port = new TcpServer(m_uv_loop, channel, handler, info.TCPServer);
		break;
	default:
		break;
	}
	return port;
}
void CPortMgr::Destory(IPort * port)
{
	delete port;
}
