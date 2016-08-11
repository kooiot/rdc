#pragma once

#include <cstdlib>

class IStreamHandler {
public:
	virtual ~IStreamHandler() {}
	virtual int OnData(void* buf, size_t len) = 0;

	virtual bool Open() = 0;
	virtual void Close() = 0;
};

class IPortHandler {
public:
	virtual ~IPortHandler() {}
	virtual int OnLog(int channel, const char* type, const char* content) = 0;
	virtual int Send(int channel, void* buf, size_t len) = 0;
};
