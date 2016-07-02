#pragma once

#include "../api/DataDefs.h"
#include <map>
#include <set>
#include <thread>
#include <enet/enet.h>

class IStreamPort;
class StreamMgr
{
public:
	StreamMgr();
	~StreamMgr();

	bool Init();
	bool Close();

	bool OnData(ENetPeer* peer, int channel, void * data, size_t len);
	//bool OnEvent(StreamEvent event);

	int Create(const StreamProcess& StreamServer, const ConnectionInfo& info);
	int Destroy(const StreamProcess& StreamServer, int channel);
private:
	ENetHost* m_ClientHost;
	std::thread* m_pThread;
	bool m_bAbort;

	StreamProcess* m_StreamServers[RC_MAX_CONNECTION];
	ENetPeer* m_Peers[RC_MAX_CONNECTION]; // Max Connection

	std::map<IStreamPort*, ConnectionInfo> m_PortInfo;
	typedef std::map<std::pair<ENetPeer*, int>, IStreamPort*> PeerChannel2PortMap;
	PeerChannel2PortMap m_PeerChannel2Port;
};

