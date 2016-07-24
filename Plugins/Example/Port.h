#pragma once

#include <thread>
#include <PluginDefs.h>

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
	char * m_Config[RDC_MAX_PLUGIN_CONFIG];
	PluginSendCB m_Send;
	PluginCloseCB m_Close;
	void* m_ptr;
	bool m_bAbort;
	std::thread* m_pThread;
};

