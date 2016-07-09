#include "stdafx.h"
#include "StreamHandler.h"


CStreamHandler::CStreamHandler() : m_data(NULL), m_evt(NULL)
{
}


CStreamHandler::~CStreamHandler()
{
}

bool CStreamHandler::OnData(int channel, void * data, size_t len)
{
	if (m_data) {
		m_data(channel, data, len, m_prv);
		return true;
	}
	return false;
}

bool CStreamHandler::OnEvent(int channel, StreamEvent event)
{
	if (m_evt) {
		m_evt(channel, event, m_prv);
		return true;
	}
	return false;
}

void CStreamHandler::SetCallbacks(stream_data_callback data, stream_event_callback evt, void* prv)
{
	m_data = data;
	m_evt = evt;
	m_prv = prv;
}
