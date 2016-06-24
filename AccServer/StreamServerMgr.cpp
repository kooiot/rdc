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


void CStreamServerMgr::HandleCMD(const CMD& cmd, void* rep)
{
	bool bSuccess = false;
	if (cmd.cmd == "ADD") {
		printf("Add event %s\n", cmd.id.c_str());
		StreamProcess* sp = new StreamProcess();
		memcpy(sp, cmd.data.c_str(), sizeof(StreamProcess));
		AddStream(atoi(cmd.id.c_str()), sp);
		bSuccess = true;
	}
	else if (cmd.cmd == "REMOVE") {
		printf("Remove event %s\n", cmd.id.c_str());
		bSuccess = true;
		RemoveStream(atoi(cmd.id.c_str()));
	}
	
	send_reply(rep, cmd, bSuccess ? "SUCCESS" : "FAILURE");
}

void CStreamServerMgr::OnRecv()
{
	CMD cmd;
	int rc = recv_cmd(m_pSocket, cmd);
	if (rc == 0) {
		HandleCMD(cmd, m_pSocket);
	}
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
