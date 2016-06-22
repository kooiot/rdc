#include "StreamServerMgr.h"
#include "ClientMgr.h"
#include <zmq.h>
#include <sstream>

#define MAX_CONNECTION_PER_SERVER 128

CStreamServerMgr::CStreamServerMgr()
{
}

CStreamServerMgr::~CStreamServerMgr()
{
}

void * CStreamServerMgr::Init(void * ctx, const char* bip, int port)
{
	void* s = zmq_socket(ctx, ZMQ_REP);

	std::stringstream ss;

	ss << "tcp://" << bip << ":" << port;
	int rc = zmq_bind(s, ss.str().c_str());
	if (rc != 0) {
		//std::cout << "Reply Socket Cannot bind to " << ss.str() << std::endl;
		zmq_close(s);
		return NULL;
	}
	m_pSocket = s;
	return s;
}

void CStreamServerMgr::Close()
{
	zmq_close(m_pSocket);
}

void CStreamServerMgr::OnRecv()
{
}

StreamProcess * CStreamServerMgr::Alloc()
{
	StreamProcess* pData = nullptr;

	int nMin = MAX_CONNECTION_PER_SERVER;
	StreamProcessMap::iterator ptr = m_Streams.begin();
	for (; ptr != m_Streams.end(); ++ptr) {
		if (ptr->second->Counter < nMin) {
			nMin = ptr->second->Counter;
			pData = ptr->second;

			// Make it faster
			if (nMin == 0)
				break;
		}
	}

	if (pData) {
		pData->Counter++;
	}

	return pData;
}

void CStreamServerMgr::Release(StreamProcess * pData)
{
	if (pData->Counter > 0)
		pData->Counter--;
}

bool CStreamServerMgr::AddStream(int id, StreamProcess* Process)
{
	if (m_Streams[id] == NULL) {
		m_Streams[id] = Process;
		return true;
	}
	return false;
}

StreamProcess* CStreamServerMgr::RemoveStream(int id)
{
	StreamProcess* pProcess = m_Streams[id];
	m_Streams[id] = NULL;
	return pProcess;
}
