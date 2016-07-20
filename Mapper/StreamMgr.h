#pragma once

#include "../api/DataDefs.h"
#include <map>
#include <list>
#include <thread>
#include <mutex>
#include <enet/enet.h>
#include <uv.h>

class IStreamPort;
class StreamMgr
{
public:
	StreamMgr(uv_loop_t * uv_loop);
	~StreamMgr();

	bool Init();
	bool Close();

	bool OnConnected(ENetPeer* peer);
	void OnDisconnected(ENetPeer* peer);
	bool OnData(ENetPeer* peer, int channel, void * data, size_t len);
	//bool OnEvent(StreamEvent event);

	int Create(const StreamProcess& StreamServer, const ConnectionInfo& info);
	int Destroy(const StreamProcess& StreamServer, int channel);
	int CloseStream(IStreamPort* port);

private:
	int ProcessPending();
private:
	uv_loop_t * m_UVLoop;
	ENetHost* m_ClientHost;
	std::thread* m_pThread;
	std::thread* m_pWorkerThread;
	bool m_bAbort;

	std::mutex m_Lock;
	StreamProcess* m_StreamServers[RC_MAX_CONNECTION];
	ENetPeer* m_Peers[RC_MAX_CONNECTION]; // Max Connection

	std::map<IStreamPort*, ConnectionInfo> m_PortInfo;
	typedef std::map<std::pair<ENetPeer*, int>, IStreamPort*> PeerChannel2PortMap;
	PeerChannel2PortMap m_PeerChannel2Port;

	std::map<ENetPeer*, std::list<IStreamPort*> > m_PendingPorts;
	std::list<int> m_PendingClose;
};

