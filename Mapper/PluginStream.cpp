#include "PluginStream.h"

PluginStream::PluginStream(CPluginLoader& loader, StreamPortInfo& info)
	: StreamPortBase(info), m_Loader(loader), m_Plugin(NULL), m_Instance(0)
{
}


PluginStream::~PluginStream()
{
}

bool PluginStream::Open()
{
	int rc = 0;
	PluginInfo& info = m_Info.ConnInfo.Plugin;
	printf("Open Plugin %s", info.Name);
	try {
		m_Plugin = m_Loader.Find(info.Name);
		if (m_Plugin)
			m_Instance = m_Plugin->CreateHandle(info.Data, SendCB, CloseCB, this);
		else
			FireEvent(SE_CHANNEL_OPEN_FAILED, "No plugin for name %s\n", info.Name);
	}
	catch (const std::exception& ex) {
		printf("%s\n", ex.what());
		FireEvent(SE_CHANNEL_OPEN_FAILED, "Exception %s", ex.what());
		m_Plugin = NULL;
		m_Instance = 0;
		return false;
	}
	catch (...) {
		printf("Unknown Exception\n");
		FireEvent(SE_CHANNEL_OPEN_FAILED, "Unknown Exception");
		m_Plugin = NULL;
		m_Instance = 0;
		return false;
	}
	if (m_Instance == 0) {
		FireEvent(SE_CHANNEL_OPEN_FAILED, "Failed create plugin instance");
		m_Plugin = NULL;
		return false;
	}

	if (0 != m_Plugin->Open(m_Instance)) {
		FireEvent(SE_CHANNEL_OPEN_FAILED, "Cannot start UDP recv callback %d\n", rc);
		return false;
	}
	rc = FireEvent(SE_CHANNEL_OPENED);
	return true;
}

void PluginStream::Close()
{
	if (m_Plugin && m_Instance != 0) {
		m_Plugin->Close(m_Instance);
		m_Plugin->Destory(m_Instance);
	}
	FireEvent(SE_CHANNEL_CLOSED);
}

int PluginStream::OnWrite(void * data, size_t len)
{
	if (m_Plugin && m_Instance != 0)
		return m_Plugin->Write(m_Instance, (char*)data, len);
	return -1;
}

int PluginStream::SendCB(const char * buf, size_t len, void * ptr)
{
	PluginStream* pThis = (PluginStream*)ptr;
	return pThis->SendData((void*)buf, len);
}

int PluginStream::CloseCB(void * ptr)
{
	PluginStream* pThis = (PluginStream*)ptr;
	return pThis->OnStreamClose();
}


