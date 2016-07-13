#include "stdafx.h"
#include "UVMgr.h"


CUVMgr::CUVMgr()
	: m_uv_loop(NULL)
	, m_thread(NULL)
	, m_abort(false)
{
}


CUVMgr::~CUVMgr()
{
}

int CUVMgr::Init()
{
	m_uv_loop = uv_default_loop();
	m_thread = new std::thread([this](void) {
		while (!m_abort) {
			uv_run(m_uv_loop, UV_RUN_ONCE);
		}
	});
	return 0;
}

void CUVMgr::Close()
{
	m_abort = true;
	uv_loop_close(m_uv_loop);
	if (m_thread && m_thread->joinable()) {
		m_thread->join();
	}
}

UVUdpPort * CUVMgr::CreateUdpPort(RC_CHANNEL channel, IPortHandler & handler, const std::string & name)
{
	return nullptr;
}

void CUVMgr::FreeUdpPort(UVUdpPort * port)
{
}

UVTcpPort * CUVMgr::CreateTcpPort(RC_CHANNEL channel, IPortHandler & handler, const std::string & name)
{
	return nullptr;
}

void CUVMgr::FreeTcpPort(UVTcpPort * port)
{
}
