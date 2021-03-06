#pragma once

#include <StreamApi.h>
#include "RemoteConnectorApi.h"

class CStreamHandler : public IStreamHandler
{
public:
	CStreamHandler();
	~CStreamHandler();

	virtual bool OnData(int channel, void * data, size_t len);
	virtual bool OnEvent(int channel, StreamEvent event, const char* msg);

	void SetCallbacks(stream_data_callback data, stream_event_callback evt, void* prv);

private:
	stream_data_callback m_data;
	stream_event_callback m_evt;
	void* m_prv;
};

