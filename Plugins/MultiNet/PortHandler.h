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

	virtual int OnLog(int channel, const char* type, const char* content);
	virtual int Send(int channel, void* buf, size_t len);
	virtual void Close();

private:
	PluginSendCB m_Send;
	PluginCloseCB m_Close;
	void* m_DataPtr;
};

