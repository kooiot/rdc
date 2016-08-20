#pragma once
#include "Handlers.h"
#include <PluginDefs.h>

class CPortHandler : public IPortHandler
{
public:
	CPortHandler(PluginSendCB send,
		PluginCloseCB close,
		void* ptr);
	~CPortHandler();

	virtual int OnLog(RC_CHANNEL channel, const char* type, const char* content);
	virtual int OnRecv(RC_CHANNEL channel, void* buf, size_t len);
	virtual void OnClose(RC_CHANNEL channel);
	virtual void OnOpen(RC_CHANNEL channel, bool open);

private:
	PluginSendCB m_Send;
	PluginCloseCB m_Close;
	void* m_DataPtr;
};

