#include "SerialStream.h"

using namespace serial;


SerialStream::SerialStream(ENetPeer* peer, const ConnectionInfo& info, int mask)
	: StreamPortBase(peer, info, mask)
{
	printf("Open Serial On PORT %s", info.Serial.dev);
	try {
		m_Serial = new Serial(info.Serial.dev,
			info.Serial.baudrate,
			Timeout::simpleTimeout(1000),
			(bytesize_t)info.Serial.bytesize,
			(parity_t)info.Serial.parity,
			(stopbits_t)info.Serial.stopbits,
			(flowcontrol_t)info.Serial.flowcontrol);
	}
	catch (const IOException& ex) {
		printf("%s\n", ex.what());
	}
	catch (const std::exception ex) {
		printf("%s\n", ex.what());
	}
	catch (...) {
		// 
		m_Serial = NULL;
	}
}


SerialStream::~SerialStream()
{
	delete m_Serial;
}

bool SerialStream::Open()
{
	if (m_Serial->isOpen())
		return true;

	m_Serial->open();
	
	if (m_Serial->isOpen())
		return __super::Open();
	return false;
}

void SerialStream::Close()
{
	__super::Open();
	m_Serial->close();
}

void SerialStream::Run()
{
	uint8_t buf[1024];
	if (!m_Serial->isOpen())
	{
		Sleep(500);
		return;
	}
	int len = m_Serial->read(buf, 1024);
	if (len == 0) {
		Sleep(50);
		return;
	}
	SendData(buf, len);
}

int SerialStream::OnWrite(void * data, size_t len)
{
	int n = m_Serial->write((uint8_t*)data, len);
	return n;
}


