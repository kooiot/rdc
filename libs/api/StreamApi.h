#pragma once

enum StreamEvent {
	SE_CONNECT,
	SE_DISCONNECT,
	SE_CLOSE,
	SE_TIMEOUT,
};
class IStreamHandler
{
public:
	virtual bool OnData(int index, const unsigned char* data, size_t len) = 0;
	virtual bool OnEvent(StreamEvent event) = 0;
};

class CStreamApi
{
public:
	CStreamApi(IStreamHandler& Handler);
	~CStreamApi();

public:
	bool Connect(const char* ip, int port);
	bool Disconnect();

	bool SendData(int index, const unsigned char* buf, size_t len);
private:
	IStreamHandler& m_Handler;
	bool m_bAbort;
	void* m_Peer;
};

