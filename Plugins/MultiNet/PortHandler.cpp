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

int CPortHandler::OnLog(int channel, const char * type, const char * content)
{
	printf("Channel %d type %s content %s\n", channel, type, content);
	return 0;
}

int CPortHandler::Send(int channel, void * buf, size_t len)
{
	char* temp = new char[len + 1];
	*temp = channel;
	memcpy(temp + 1, buf, len);
	int rc = m_Send(temp, len, m_DataPtr);
	delete[] temp;
	return rc;
}

void CPortHandler::Close()
{
	m_Close(m_DataPtr);
}
