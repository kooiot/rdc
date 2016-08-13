#include "Port.h"
#include <cstring>

#ifndef RDC_LINUX_SYS
#include <Windows.h>
#else
#include <unistd.h>
#define Sleep(x) usleep(x * 1000)
#endif

CPort::CPort(char *config,
	PluginSendCB send,
	PluginCloseCB close,
	void* ptr)
	: m_Send(send)
	, m_Close(close)
	, m_ptr(ptr)
	, m_bAbort(false)
	, m_pThread(nullptr)
{
	memcpy(m_Config, config, RDC_MAX_PLUGIN_CONFIG);
}


CPort::~CPort()
{
}

bool CPort::Open()
{
	if (m_pThread)
		return false;

	m_pThread = new std::thread([this]() {
		int i = 0;
		char temp[80];
		for (; i < 80; ++i) {
			temp[i] = 'a' + i;
		}
		while (!m_bAbort) {
			//if (i++ > 100)
			//	break;

			Sleep(50);

			//m_Send("Hello World", strlen("Hello World"), m_ptr);
			m_Send(temp, 80, m_ptr);
		}

		if (!m_bAbort)
			m_Close(m_ptr);
	});
	return true;
}

void CPort::Close()
{
	m_bAbort = true;
	if (m_pThread && m_pThread->joinable())
		m_pThread->join();
	delete m_pThread;
}

int CPort::Write(const char * buf, size_t len)
{
	printf("Write %d\n", len);
	return len;
}
