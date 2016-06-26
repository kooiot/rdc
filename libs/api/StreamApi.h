#pragma once
#include <thread>

enum StreamEvent {
	SE_CONNECT,
	SE_DISCONNECT,
	SE_CLOSE,
	SE_TIMEOUT,
};
class IStreamHandler
{
public:
	virtual bool OnData(int channel, const unsigned char* data, size_t len) = 0;
	virtual bool OnEvent(StreamEvent event) = 0;
};

#define MAPPER_TYPE 1
#define CLIENT_TYPE 2

class CStreamApi
{
public:
	CStreamApi(IStreamHandler& Handler, int nIndex, int nType = CLIENT_TYPE);
	~CStreamApi();

public:
	bool Connect(const char* ip, int port);
	bool Disconnect();

	bool SendData(int channel, const unsigned char* buf, size_t len);
private:
	IStreamHandler& m_Handler;
	bool m_bAbort;
	void* m_Peer;
	std::thread* m_pThread;
	int m_nData;
};

