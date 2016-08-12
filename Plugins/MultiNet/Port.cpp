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
	m_Mgr.Init();
	json list = json::parse(config);
	int i = 0;
	for (auto & j : list) {
		ConnectionInfo info;
		if (KOO_PARSE_JSON(info, j)) {
			CPortHandler* handler = new CPortHandler(send, close, ptr);
			IPort* port = m_Mgr.Create(i++, handler, info);
			m_Ports.push_back(port);
		}
	}
	
}


CPort::~CPort()
{
}

bool CPort::Open()
{
}

void CPort::Close()
{
}

int CPort::Write(const char * buf, size_t len)
{
	printf("Write %d\n", len);
	return len;
}
