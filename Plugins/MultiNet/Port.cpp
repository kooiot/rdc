#include "stdafx.h"
#include <DataJson.h>
#include <PluginDefs.h>
#include "Port.h"
#include "PortHandler.h"
#include <cstring>

CPort::CPort(char *config,
	PluginSendCB send,
	PluginCloseCB close,
	void* ptr)
{
	CPortHandler* m_PortHandler = new CPortHandler(send, close, ptr);
	m_Mgr.Init();
	json list = json::parse(config);
	int i = 0;
	for (auto & j : list) {
		ConnectionInfo info;
		if (KOO_PARSE_JSON(info, j)) {
			IPort* port = m_Mgr.Create(i++, m_PortHandler, info);
			m_Ports.push_back(port);
		}
	}
	
}


CPort::~CPort()
{
	m_Mgr.Close();
	delete m_PortHandler;
}

bool CPort::Open()
{
	for (auto & port : m_Ports) {
		if (!port->Open()) {
			return false;
		}
	}
}

void CPort::Close()
{
	for (auto & port : m_Ports) {
		port->Close();
	}
}

int CPort::Write(const char * buf, size_t len)
{
	int index = *buf;
	if (index < 0 || index > m_Ports.size())
		return -1;

	const char* p = buf + 1;
	return m_Ports[index]->Write((void*)p, len - 1);
}
