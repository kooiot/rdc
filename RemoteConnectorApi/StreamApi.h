#pragma once

enum StreamEvent {
	SE_CLOSE,
	SE_TIMEOUT,
};
class IStreamHandler
{
public:
	virtual bool OnRecv(const char* buf, size_t len) = 0;
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

private:
	IStreamHandler& m_Handler;
	bool m_bAbort;
};

