#include "stdafx.h"
#include "PortMgr.h"


CPortMgr::CPortMgr()
	: m_uv_loop(NULL)
	, m_thread(NULL)
	, m_abort(false)
{
}


CPortMgr::~CPortMgr()
{
}

int CPortMgr::Init()
{
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

UVUdpPort * CPortMgr::CreateUdpPort(int channel, IPortHandler & handler, const std::string & name)
{
	return nullptr;
}

void CPortMgr::FreeUdpPort(UVUdpPort * port)
{
}

UVTcpPort * CPortMgr::CreateTcpPort(int channel, IPortHandler & handler, const std::string & name)
{
	return nullptr;
}

void CPortMgr::FreeTcpPort(UVTcpPort * port)
{
}
