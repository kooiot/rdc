#include "stdafx.h"
#include "PortHandler.h"
#include <cstdio>
#include <cstring>

CPortHandler::CPortHandler(PluginSendCB send,
	PluginCloseCB close,
	void* ptr)
	: m_Send(send)
	, m_Close(close)
	, m_DataPtr(ptr)
{
}


CPortHandler::~CPortHandler()
{
}

int CPortHandler::OnLog(RC_CHANNEL channel, const char * type, const char * content)
{
	printf("Channel %d type %s content %s\n", channel, type, content);
	return 0;
}

int CPortHandler::OnRecv(RC_CHANNEL channel, void * buf, size_t len)
{
	char* temp = new char[len + 1];
	*temp = channel;
	memcpy(temp + 1, buf, len);
	int rc = m_Send(temp, len + 1, m_DataPtr);
	delete[] temp;
	return rc;
}

void CPortHandler::OnClose(RC_CHANNEL channel)
{
	m_Close(m_DataPtr);
}

void CPortHandler::OnOpen(RC_CHANNEL channel, bool open)
{
	printf("Channel %d OnOpen %s\n", channel, open ? "True" : "False");
}
