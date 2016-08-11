#pragma once

#include <map>
#include <string>
#include "StreamServerMgr.h"
#include "AccDatabase.h"
#include <koo_zmq_helpers.h>

#define CLIENT_HEARTBEAT_TIME 30
#define MAPPER_HEARTBEAT_TIME 120

struct ClientData;
struct MapperData {
	std::string ID;
	time_t Heartbeat;
};

struct ConnectionData {
	int Channel;
	MapperData* Mapper;
};

struct ClientData {
	std::string ID;
	time_t Heartbeat;
	StreamProcess* StreamServer;
	ConnectionData* Connections[RC_MAX_CONNECTION];
};

class CClientMgr
{
public:
	CClientMgr(CStreamServerMgr& StreamMgr);
	~CClientMgr();

	void* Init(void* ctx, const char* bip, int port_rep, int port_pub);
	void Close();
	void OnRecv();

	void OnTimer(int nTime);

private:
	void HandleKZPacket(const KZPacket& cmd);
	int SendToMapper(const std::string& id, const std::string& cmd, const json& val);

private:
	int AddMapper(const std::string& id);
	int RemoveMapper(const std::string& id);
	MapperData* FindMapper(const std::string& id);
	int AddClient(const std::string& id);
	int RemoveClient(const std::string& id);
	ClientData* FindClient(const std::string& id);
	int UpdateMapperHearbeat(const std::string& id);
	int UpdateClientHearbeat(const std::string& id);

	// Return channel index
	int AllocStream(const std::string& id, const std::string& mapper_id);
	MapperData* FindStream(const std::string& id, int channel);
	int FreeStream(const std::string& id, int channel);
	int ListStream(const std::string& id, std::vector<int> channels);
private:
	void* m_pReply;
	void* m_pPublish;
	typedef std::map<std::string, ClientData*> ClientMap;
	ClientMap m_Clients;
	typedef std::map<std::string, MapperData*> MapperMap;
	MapperMap m_Mappers;

	CStreamServerMgr& m_StreamMgr;
	CAccDatabase m_Database;
};

