#pragma once

class IStreamPort {
public:
	virtual ~IStreamPort() {};
	
	virtual bool Open() = 0;
	virtual void Close() = 0;

	virtual int OnClientData(void* data, size_t len) = 0;
};