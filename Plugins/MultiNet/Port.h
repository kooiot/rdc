#pragma once

#include <vector>
#include <thread>
#include <PluginDefs.h>
#include <PortMgr.h>

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
	CPortMgr m_Mgr;
	std::vector<IPort*> m_Ports;
};

