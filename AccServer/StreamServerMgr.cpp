#include "StreamServerMgr.h"
#include "ClientMgr.h"
#include <zmq.h>
#include <sstream>
#include <cstring>
#include <DataDefs.h>
#include <DataJson.h>

CStreamServerMgr::CStreamServerMgr()
{
	for (int i = 0; i < RC_MAX_STREAM_SERVER_COUNT; ++i) {
		StreamProcessVector& v = m_Streams[i];
		v.Online = false;
		v.Counter = 0;
		memset(v.Used, false, RC_MAX_CONNECTION_PER_SERVER * sizeof(bool));
		for (int j = 0; j < RC_MAX_CONNECTION_PER_SERVER; ++j) {
			v.List[j].__inner = &v;
			v.List[j].Index = j;
		}
	}
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


void CStreamServerMgr::HandleKZPacket(const KZPacket& cmd, void* rep)
{
	bool bSuccess = false;
	if (cmd.cmd() == "ADD") {
		printf("Add event %s\n", cmd.id().c_str());
		JSON_FROM_PACKET(cmd, IPInfo, info);
		AddStream(atoi(cmd.id().c_str()), &info);
		bSuccess = true;
	}
	else if (cmd.cmd() == "REMOVE") {
		printf("Remove event %s\n", cmd.id().c_str());
		bSuccess = true;
		RemoveStream(atoi(cmd.id().c_str()));
	}
	
	koo_zmq_send_result(rep, cmd, bSuccess);
}

void CStreamServerMgr::OnRecv()
{
	KZPacket cmd;
	int rc = koo_zmq_recv(m_pSocket, cmd);
	if (rc == 0) {
		HandleKZPacket(cmd, m_pSocket);
	}
}
StreamProcess * CStreamServerMgr::Alloc()
{
	StreamProcessVector* pV = nullptr;
	StreamProcess* pData = nullptr;

	int nMin = RC_MAX_CONNECTION_PER_SERVER;
	
	for (int i = 0; i < RC_MAX_STREAM_SERVER_COUNT; ++i) {
		StreamProcessVector& v = m_Streams[i];
		if (!v.Online)
			continue;

		if (v.Counter < nMin) {
			nMin = v.Counter;
			pV = &v;

			// Make it faster
			if (nMin == 0)
				break;
		}
	}
	if (pV) {
		for (int i = 0; i < RC_MAX_CONNECTION_PER_SERVER; ++i) {
			if (!pV->Used[i])
			{
				pV->Counter++;
				pV->Used[i] = true;
				pData = &pV->List[i];
				pData->Mask = (long)time(NULL);
				break;
			}
		}
	}

	return pData;
}

void CStreamServerMgr::Release(StreamProcess * pData)
{
	StreamProcessVector* pV = (StreamProcessVector*)pData->__inner;
	pV->Counter--;
	pV->Used[pData->Index] = false;
}

bool CStreamServerMgr::AddStream(int id, IPInfo * Process)
{
	id = id - RC_STREAM_SERVER_ID_BASE;
	StreamProcessVector& sp = m_Streams[id];
	if (sp.Online) {
		return false;
	}

	sp.Online = true;
	for (int j = 0; j < RC_MAX_CONNECTION_PER_SERVER; ++j) {
		memcpy(sp.List[j].StreamIP, Process->sip, RC_MAX_IP_LEN);
		sp.List[j].Port = Process->port;
	}
	return true;
}

bool CStreamServerMgr::RemoveStream(int id)
{
	id = id - RC_STREAM_SERVER_ID_BASE;
	StreamProcessVector& sp = m_Streams[id];
	sp.Online = false;
	return true;
}
