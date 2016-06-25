#include "ClientMgr.h"
#include "../RemoteConnectorApi/DataDefs.h"
#include <zmq.h>

#include <list>
#include <sstream>
#include <cassert>
#include <iostream>

#ifdef WIN32
#include <Windows.h>
#endif


CClientMgr::CClientMgr(CStreamServerMgr& StreamMgr) : m_StreamMgr(StreamMgr)
{
}


CClientMgr::~CClientMgr()
{
}

void * CClientMgr::Init(void * ctx, const char * bip, int port_rep, int port_pub)
{
	void* rep = zmq_socket(ctx, ZMQ_REP);

	std::stringstream ss;

	ss << "tcp://" << bip << ":" << port_rep;
	int rc = zmq_bind(rep, ss.str().c_str());
	if (rc != 0) {
		//std::cout << "Reply Socket Cannot bind to " << ss.str() << std::endl;
		zmq_close(rep);
		return NULL;
	}
	void* pub = zmq_socket(ctx, ZMQ_PUB);

	std::stringstream ss2;
	ss2 << "tcp://" << bip << ":" << port_pub;
	rc = zmq_bind(pub, ss2.str().c_str());
	if (rc != 0) {
		//std::cout << "Publish Socket Cannot bind to " << ss.str() << std::endl;
		zmq_close(rep);
		zmq_close(pub);
		return NULL;
	}

	m_pReply = rep;
	m_pPublish = pub;

	m_Auth.Init();

	return rep;
}

void CClientMgr::Close()
{
	m_Auth.Close();
	zmq_close(m_pReply);
	zmq_close(m_pPublish);
}

void CClientMgr::HandleKZPacket(const KZPacket& cmd, void* rep, void* pub)
{
	std::cout << __FUNCTION__ << ": " << cmd.id << "\t" << cmd.cmd << std::endl;

	bool bSuccess = false;
	if (cmd.cmd == "LOGIN") {
		std::cout << "LOGIN: " << cmd.id << "\t" << cmd.data << std::endl;
		if (cmd.id == "@DEVICE@") {
			// Check for valid device sn
			if (m_Auth.IsValidMapper(cmd.data) == 0) {
				AddMapper(cmd.data);
				bSuccess = true;
			}
		}
		else if (m_Auth.Login(cmd.id, cmd.data) == 0) {
			AddClient(cmd.id);
			bSuccess = true;
		}
	}
	else if (cmd.cmd == "HEARTBEAT") {
		bSuccess = true;
		if (cmd.id == "@DEVICE@")
			UpdateMapperHearbeat(cmd.data);
		else
			UpdateClientHearbeat(cmd.id);
	}
	else if (cmd.cmd == "LOGOUT") {
		bSuccess = true;
		if (cmd.id == "@DEVICE@")
			RemoveMapper(cmd.data);
		else
			RemoveClient(cmd.id);
	}
	else if (cmd.cmd == "CREATE") {
		std::string dest_id = cmd.data.substr(0, cmd.data.find(" "));
		if (m_Auth.Access(cmd.id, dest_id) == 0) {
			koo_zmq_send_msg(pub, cmd.data, false);
			bSuccess = true;
		}
	}
	else if (cmd.cmd == "DESTROY") {
		std::string dest_id = cmd.data.substr(0, cmd.data.find(" "));
		if (m_Auth.Access(cmd.id, dest_id) == 0) {
			koo_zmq_send_msg(pub, cmd.data, false);
			bSuccess = true;
		}
	}
	else if (cmd.cmd == "LIST_DEV") {
		std::string data;
		MapperMap::iterator ptr = m_Mappers.begin();
		for (; ptr != m_Mappers.end(); ++ptr, data += "|") {
			data += ptr->first;
		}
		int rc = koo_zmq_send_reply(rep, cmd, data);
		assert(rc == 0);
		return;
	}
	else if (cmd.cmd == "DEV_INFO") {
		std::string dev_id = cmd.data;
		MapperMap::iterator ptr = m_Mappers.find(dev_id);
		if (ptr != m_Mappers.end()) {
			DeviceInfo info;
			memset(&info, 0, sizeof(DeviceInfo));
			snprintf(info.ID, RC_MAX_ID_LEN, "%s", dev_id.c_str());
			snprintf(info.Name, RC_MAX_NAME_LEN, "%s", ptr->second->ID.c_str());
			snprintf(info.Desc, RC_MAX_NAME_LEN, "%s", "Mapper Device");
			info.IsOnline = true;
			info.LastUpdate = ptr->second->Heartbeat;
			koo_zmq_send_reply(rep, cmd, &info, sizeof(DeviceInfo));
			return;
		}
		else {
			// TODO: Find in database;
		}
	}
	else if (cmd.cmd == "LIST_CLIENT") {
		std::string data;
		ClientMap::iterator ptr = m_Clients.begin();
		for (; ptr != m_Clients.end(); ++ptr, data += "|") {
			data += ptr->first;
		}
		koo_zmq_send_reply(rep, cmd, data);
		return;
	}
	else if (cmd.cmd == "CLIENT_INFO") {
		std::string id = cmd.data;
		ClientMap::iterator ptr = m_Clients.find(id);
		if (ptr != m_Clients.end()) {
			UserInfo info;
			memset(&info, 0, sizeof(UserInfo));
			snprintf(info.ID, RC_MAX_ID_LEN, "%s", id.c_str());
			snprintf(info.Name, RC_MAX_NAME_LEN, "%s", ptr->second->ID.c_str());
			snprintf(info.Desc, RC_MAX_NAME_LEN, "%s", "Mapper Device");
			snprintf(info.Desc, RC_MAX_NAME_LEN, "%s", "Unknown");
			info.IsOnline = true;
			koo_zmq_send_reply(rep, cmd, &info, sizeof(UserInfo));
			return;
		}
		else {
			// TODO: Find in database;
		}
	}
	std::cout << "SEND REPLY: " << cmd.id << "\t" << cmd.cmd << "\t" << (bSuccess ? S_SUCCESS : S_FAILED) << std::endl;
	koo_zmq_send_reply(rep, cmd, bSuccess ? S_SUCCESS : S_FAILED);
}

void CClientMgr::OnRecv()
{
	KZPacket cmd;
	int rc = koo_zmq_recv_cmd(m_pReply, cmd);
	if (rc == 0) {
		HandleKZPacket(cmd, m_pReply, m_pPublish);
	}
}

int CClientMgr::AddMapper(const std::string & id)
{
	if (m_Mappers.find(id) != m_Mappers.end()) {
		RemoveMapper(id);
	}
	MapperData* pData = new MapperData();

	pData->ID = id;
	pData->Heartbeat = ::GetCurrentTime();

	return 0;
}

int CClientMgr::RemoveMapper(const std::string & id)
{
	MapperMap::iterator ptr = m_Mappers.find(id);
	if (ptr == m_Mappers.end())
		return -1;

	ResourceMap& Resources = ptr->second->Resources;
	ResourceMap::iterator cptr = Resources.begin();
	for (; cptr != Resources.end(); ++cptr) {
		ClientData* pClient = cptr->second;
		if (pClient != NULL) {
			for (int i = 0; i < MAX_CLIENT_CONNECTION; ++i) {
				ConnectionData& Data = pClient->Connections[i];
				if (Data.Mapper == ptr->second) {
					Data.Mapper = NULL;
					Data.Resource = "";
				}
			}
		}
	}

	delete ptr->second;
	return 0;
}

int CClientMgr::AddClient(const std::string & id)
{
	if (m_Clients.find(id) != m_Clients.end()) {
		RemoveMapper(id);
	}
	ClientData* pData = new ClientData();

	pData->ID = id;
	pData->Heartbeat = ::GetCurrentTime();
	pData->StreamServer = m_StreamMgr.Alloc();
	return 0;
}

int CClientMgr::RemoveClient(const std::string & id)
{
	ClientMap::iterator ptr = m_Clients.find(id);
	if (ptr == m_Clients.end())
		return -1;

	for (int i = 0; i < MAX_CLIENT_CONNECTION; ++i){
		ConnectionData& Conn = ptr->second->Connections[i];
		if (Conn.Mapper != NULL) {
			Conn.Mapper->Resources[Conn.Resource] = NULL;
		}
	}

	StreamProcess* StreamServer = ptr->second->StreamServer;
	m_StreamMgr.Release(StreamServer);

	delete ptr->second;
	return 0;
}

int CClientMgr::UpdateMapperHearbeat(const std::string & id)
{
	MapperMap::iterator ptr = m_Mappers.find(id);
	if (ptr == m_Mappers.end())
		return -1;
	ptr->second->Heartbeat = ::GetCurrentTime();
	return 0;
}

int CClientMgr::UpdateClientHearbeat(const std::string & id)
{
	ClientMap::iterator ptr = m_Clients.find(id);
	if (ptr == m_Clients.end())
		return -1;
	ptr->second->Heartbeat = ::GetCurrentTime();
	return 0;
}

void CClientMgr::OnTimer(int nTime)
{
	std::list<std::string> rlist;
	// Check Clients
	ClientMap::iterator ptr = m_Clients.begin();
	for (; ptr != m_Clients.end(); ++ptr) {
		if (nTime - ptr->second->Heartbeat > CLIENT_HEARTBEAT_TIME) {
			// Logger
			rlist.push_back(ptr->first);
		}
	}
	std::list<std::string>::iterator lptr = rlist.begin();
	for (; lptr != rlist.end(); ++lptr) {
		RemoveClient(*lptr);
	}

	rlist.clear();
	// Check Clients
	MapperMap::iterator mptr = m_Mappers.begin();
	for (; mptr != m_Mappers.end(); ++mptr) {
		if (nTime - mptr->second->Heartbeat > MAPPER_HEARTBEAT_TIME) {
			// Logger
			rlist.push_back(mptr->first);
		}
	}
	lptr = rlist.begin();
	for (; lptr != rlist.end(); ++lptr) {
		RemoveMapper(*lptr);
	}
}
