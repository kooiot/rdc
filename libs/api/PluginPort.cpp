#include "PluginPort.h"


PluginPort::PluginPort(RC_CHANNEL channel, IPortHandler* Handler, const PluginInfo& info, PluginApi* api)
	: IPort(channel, Handler)
	, m_Info(info)
	, m_Api(api)
	, m_ApiHandle(0)
{
}


PluginPort::~PluginPort()
{
}

bool PluginPort::Open()
{
	m_ApiHandle = m_Api->CreateHandle(m_Info.Data, SendCB, CloseCB, this);
	int rc = m_Api->Open(m_ApiHandle);
	if (rc == 0)
		return true;
	m_Api->Destory(m_ApiHandle);
	m_ApiHandle = 0;
	m_Api = NULL;
	return true;
}

void PluginPort::Close()
{
	if (m_ApiHandle) {
		m_Api->Close(m_ApiHandle);
		m_Api->Destory(m_ApiHandle);
		m_ApiHandle = 0;
		m_Api = NULL;
	}
}

int PluginPort::Write(void * buf, size_t len)
{
	if (m_ApiHandle == 0 || NULL == m_Api)
		return -1;

	int rc = m_Api->Write(m_ApiHandle, (char*)buf, len);
	if (rc > 0)
		return 0;
	return -1;
}

int PluginPort::Event(StreamEvent evt)
{
	return 0;
}

int PluginPort::SendCB(const char * buf, size_t len, void * ptr)
{
	PluginPort* pThis = (PluginPort*)ptr;

	return pThis->m_pHandler->OnRecv(pThis->m_nChannel, (void*)buf, len);
}

int PluginPort::CloseCB(void * ptr)
{
	PluginPort* pThis = (PluginPort*)ptr;
	pThis->OnClose();
	return 0;
}
