#pragma once


class IStreamHandler {
public:
	virtual ~IStreamHandler() {}
	virtual int OnData(void* buf, size_t len) = 0;
	virtual int OnEvent(StreamEvent evt) = 0;
};

class IPortHandler {
public:
	virtual ~IPortHandler() {}
	virtual int OnLog(RC_CHANNEL channel, const char* type, const char* content) = 0;
	virtual int Send(RC_CHANNEL channel, void* buf, size_t len) = 0;
};