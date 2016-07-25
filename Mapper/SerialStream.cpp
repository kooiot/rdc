#include "SerialStream.h"

using namespace serial;


SerialStream::SerialStream(StreamPortInfo& info)
	: StreamPortBase(info), m_Serial(NULL), m_pThread(NULL), m_bAbort(false)
{
	printf("Create Serial Stream  DEV:%s Baudrate:%d ByteSize:%d Stopbits:%d FlowControl %d, Parity:%d\n",
		info.ConnInfo.Serial.dev,
		info.ConnInfo.Serial.baudrate,
		info.ConnInfo.Serial.bytesize,
		info.ConnInfo.Serial.stopbits,
		info.ConnInfo.Serial.flowcontrol,
		info.ConnInfo.Serial.parity);
}


SerialStream::~SerialStream()
{
	delete m_Serial;
}

bool SerialStream::Open()
{
	printf("Open Serial On PORT %s", m_Info.ConnInfo.Serial.dev);
	try {
		m_Serial = new Serial(m_Info.ConnInfo.Serial.dev,
			m_Info.ConnInfo.Serial.baudrate,
			Timeout::simpleTimeout(1000),
			(bytesize_t)m_Info.ConnInfo.Serial.bytesize,
			(parity_t)m_Info.ConnInfo.Serial.parity,
			(stopbits_t)m_Info.ConnInfo.Serial.stopbits,
			(flowcontrol_t)m_Info.ConnInfo.Serial.flowcontrol);
	}
	catch (const IOException& ex) {
		printf("%s\n", ex.what());
		FireEvent(SE_CHANNEL_OPEN_FAILED, "Exception %s", ex.what());
		return false;
	}
	catch (const std::exception ex) {
		printf("%s\n", ex.what());
		FireEvent(SE_CHANNEL_OPEN_FAILED, "Exception %s", ex.what());
		return false;
	}
	catch (...) {
		// 
		FireEvent(SE_CHANNEL_OPEN_FAILED, "Unknown Exception");
		m_Serial = NULL;
		return false;
	}
	if (!m_Serial)
		return false;

	if (!m_Serial->isOpen())
		m_Serial->open();
	
	if (m_Serial->isOpen()) {
		int rc = FireEvent(SE_CHANNEL_OPENED);

		m_pThread = new std::thread([this]() {
			while (!m_bAbort) {
				this->Run();
			}
		});

		return true;
	}
	FireEvent(SE_CHANNEL_OPEN_FAILED, "Failed to open serial port %s", m_Info.ConnInfo.Serial.dev);
	return false;
}

void SerialStream::Close()
{
	m_bAbort = true;
	if (m_pThread && m_pThread->joinable())
		m_pThread->join();
	delete m_pThread;
	m_pThread = NULL;

	m_Serial->close();

	FireEvent(SE_CHANNEL_CLOSED);
}

void SerialStream::Run()
{
	uint8_t buf[1024];
	if (!m_Serial->isOpen())
	{
#ifdef RDC_LINUX_SYS
		usleep(500 * 1000);
#else
		Sleep(500);
#endif
		return;
	}
	int len = m_Serial->read(buf, 1024);
	if (len == 0) {
//#ifdef RDC_LINUX_SYS
//		usleep(50 * 1000);
//#else
//		Sleep(50);
//#endif
		return;
	}
	SendData(buf, len);
}

int SerialStream::OnWrite(void * data, size_t len)
{
	int n = m_Serial->write((uint8_t*)data, len);
	return n;
}


