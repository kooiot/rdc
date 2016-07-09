#pragma once
#include <thread>
#include "DataDefs.h"

class IStreamHandler
{
public:
	virtual bool OnData(int channel, void * data, size_t len) = 0;
	// Channel is not -1 when channel event happens
	virtual bool OnEvent(int channel, StreamEvent event) = 0;
};

#define MAPPER_TYPE 1
#define CLIENT_TYPE 2

class CStreamApi
{
public:
	CStreamApi(IStreamHandler& Handler, int nType, int nIndex, int nStreamMask);
	~CStreamApi();

public:
	bool Connect(const char* ip, int port);
	void Disconnect();

	int SendData(int channel, void * buf, size_t len);

private:
	int OnData(int channel, void * data, size_t len);
private:
	IStreamHandler& m_Handler;
	int m_nStreamMask;
	bool m_bAbort;
	void* m_Peer;
	std::thread* m_pThread;
	int m_nData;
};

