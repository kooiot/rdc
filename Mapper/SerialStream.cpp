#include "SerialStream.h"

using namespace serial;


SerialStream::SerialStream(ENetPeer* peer, const ConnectionInfo& info)
	: m_Peer(peer), m_Serial(NULL)
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

	return false;
}

void SerialStream::Close()
{
	m_Serial->close();
}

int SerialStream::OnClientData(void * data, size_t len)
{
	int n = m_Serial->write((uint8_t*)data, len);
	printf("Write To Serial %d - %d", len, n);
	return n;
}
