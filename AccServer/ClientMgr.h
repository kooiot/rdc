#pragma once

#include <map>
#include <string>
#include "StreamServerMgr.h"
#include "AccDatabase.h"
#include <koo_zmq_helpers.h>

#define MAX_CLIENT_CONNECTION 32
#define CLIENT_HEARTBEAT_TIME 30
#define MAPPER_HEARTBEAT_TIME 120

struct ClientData;
typedef std::map<std::string, ClientData*> ResourceMap;
struct MapperData {
	std::string ID;
	int Heartbeat;
	// Resource Key, Client Pointer
	ResourceMap Resources;
};

struct ConnectionData {
	int Channel;
	MapperData* Mapper;
	std::string Resource;
};

struct ClientData {
	std::string ID;
	int Heartbeat;
	StreamProcess* StreamServer;
	ConnectionData Connections[MAX_CLIENT_CONNECTION];
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
	void HandleKZPacket(const KZPacket& cmd, void* rep, void* pub);

private:
	int AddMapper(const std::string& id);
	int RemoveMapper(const std::string& id);
	int AddClient(const std::string& id);
	int RemoveClient(const std::string& id);
	int UpdateMapperHearbeat(const std::string& id);
	int UpdateClientHearbeat(const std::string& id);

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

