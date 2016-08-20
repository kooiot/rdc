#pragma once

#include <vector>
#include <thread>
#include <PluginDefs.h>
#include <PortMgr.h>
#include "PortHandler.h"

class CPort
{
public:
	CPort(char *config,
		PluginSendCB send,
		PluginCloseCB close,
		void* ptr);
	~CPort();

	bool Open();
	void Close();
	int Write(const char* buf, size_t len);

private:
	CPortHandler* m_PortHandler;
	CPortMgr m_Mgr;
	std::vector<IPort*> m_Ports;
};

