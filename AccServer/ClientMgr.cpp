#include "ClientMgr.h"
#include <DataDefs.h>
#include <DataJson.h>
#include <zmq.h>

#include <list>
#include <sstream>
#include <cassert>
#include <iostream>
#include <cstring>

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

	m_Database.Init();

	return rep;
}

void CClientMgr::Close()
{
	m_Database.Close();
	zmq_close(m_pReply);
	zmq_close(m_pPublish);
}

void CClientMgr::HandleKZPacket(const KZPacket& cmd)
{
	std::cout << __FUNCTION__ << ": " << cmd.id() << "\t" << cmd.cmd() << std::endl;
	if (cmd.cmd() != "LOGIN" && cmd.id() != "@DEVICE@") {
		if (m_Clients.find(cmd.id()) == m_Clients.end()) {
			std::cerr << "User does not login or heartbeat timeout" << cmd.id() << std::endl;
			koo_zmq_send_result(m_pReply, cmd, -10000);
			return;
		}
	}

	int nReturn = -1;
	if (cmd.cmd() == "LOGIN") {
		if (cmd.id() == "@DEVICE@") {
			std::string dev_sn = cmd.get("sn");
			std::cout << "Device LOGIN: " << dev_sn << std::endl;
			// Check for valid device sn
			if (m_Database.IsValidDevice(dev_sn) == 0) {
				AddMapper(dev_sn);
				nReturn = 0;
			}
		       	else {
				nReturn = -99; // Not valid device
			}
		}
		else {
			std::cout << "Client LOGIN: " << cmd.id() << "\t" << cmd.get("pass") << std::endl;
		       	if (m_Database.Login(cmd.id(), cmd.get("pass")) == 0) {
				nReturn = AddClient(cmd.id());
			}
		}
	}
	else if (cmd.cmd() == "HEARTBEAT") {
		nReturn = 0;
		if (cmd.id() == "@DEVICE@")
			UpdateMapperHearbeat(cmd.get("sn"));
		else
			UpdateClientHearbeat(cmd.id());
	}
	else if (cmd.cmd() == "LOGOUT") {
		nReturn = 0;
		if (cmd.id() == "@DEVICE@")
			RemoveMapper(cmd.get("sn"));
		else
			RemoveClient(cmd.id());
	}
	else if (cmd.cmd() == "STREAM") {
		StreamProcess info;
		memset(&info, 0, sizeof(StreamProcess));
		ClientData* pData = m_Clients[cmd.id()];
		if (pData && pData->StreamServer) {
			memcpy(&info, pData->StreamServer, sizeof(StreamProcess));
		}
		int rc = koo_zmq_send_result(m_pReply, cmd, KOO_GEN_JSON(info));
		assert(rc >= 0);
		return;
	}
	else if (cmd.cmd() == "CREATE") {
		JSON_FROM_PACKET(cmd, ConnectionInfo, info);
		int channel = -99;
		if (FindMapper(info.DevSN) && 0 == m_Database.Access(cmd.id(), info.DevSN)) {
			ClientData* pClient = FindClient(cmd.id());
			assert(pClient);
			channel = AllocStream(cmd.id(), info.DevSN);
			if (channel >= 0) {
				info.Channel = channel;
				std::cout << "Create Stream Channel " << channel << std::endl;
				json val;
				val["sp"] = KOO_GEN_JSON(*pClient->StreamServer);
				val["info"] = KOO_GEN_JSON(info);
				SendToMapper(info.DevSN, "CREATE", val);
			}
		}
		int rc = koo_zmq_send_result(m_pReply, cmd, channel);
		assert(rc >= 0);
		return;
	}
	else if (cmd.cmd() == "DESTROY") {
		int channel = cmd.get("channel");
		if (channel >= 0 && channel < RC_MAX_CONNECTION) {
			nReturn = FreeStream(cmd.id(), channel);
		}
	}
	else if (cmd.cmd() == "ADD_DEV") {
		JSON_FROM_PACKET(cmd, DeviceInfo, info);
		DbDeviceInfo dbInfo;
		dbInfo.FromDeviceInfo(info);
		nReturn = m_Database.AddDevice(dbInfo);
	}
	else if (cmd.cmd() == "MOD_DEV") {
		JSON_FROM_PACKET(cmd, DeviceInfo, info);
		DbDeviceInfo dbInfo;
		dbInfo.FromDeviceInfo(info);
		nReturn = m_Database.UpdateDevice(dbInfo);
	}
	else if (cmd.cmd() == "DEL_DEV") {
		nReturn = m_Database.DeleteDevice(cmd.get("sn"));
	}
	else if (cmd.cmd() == "LIST_DEV") {
		std::string type = cmd.get("type");
		std::list<std::string> list;
		if (type != "ALL") {
			MapperMap::iterator ptr = m_Mappers.begin();
			for (; ptr != m_Mappers.end(); ++ptr) {
				if (0 == m_Database.Access(cmd.id(), ptr->first)) {
					list.push_back(ptr->first);
				}
			}
		}
		else {
			std::list<std::string> all;
			m_Database.ListDevices(all);
			for (std::list<std::string>::iterator ptr = all.begin();
				ptr != all.end(); ++ptr) {
				if (0 == m_Database.Access(cmd.id(), *ptr)) {
					list.push_back(*ptr);
				}
			}
		}

		std::string data;
		std::list<std::string>::iterator ptr = list.begin();
		for (; ptr != list.end(); ++ptr, data += "|") {
			data += *ptr;
		}
		int rc = koo_zmq_send_result(m_pReply, cmd, data);
		assert(rc >= 0);
		return;
	}
	else if (cmd.cmd() == "DEV_INFO") {
		std::string sn = cmd.get("sn");
		DbDeviceInfo dbInfo;
		int rc = m_Database.GetDevice(sn, dbInfo);
		if (rc != 0) {
			std::cerr << "Invalid Device ID " << std::endl;

		}
		DeviceInfo info;
		memset(&info, 0, sizeof(DeviceInfo));
		if (0 == m_Database.Access(cmd.id(), dbInfo.SN)) {
			dbInfo.ToDeviceInfo(info);
		}
		koo_zmq_send_result(m_pReply, cmd, KOO_GEN_JSON(info));
		return;
	}
	else if (cmd.cmd() == "ADD_CLIENT") {
		JSON_FROM_PACKET(cmd, UserInfo, info);
		DbUserInfo dbInfo;
		dbInfo.FromUserInfo(info);
		nReturn = m_Database.AddUser(dbInfo, info.Passwd);
	}
	else if (cmd.cmd() == "MOD_CLIENT") {
		JSON_FROM_PACKET(cmd, UserInfo, info);
		DbUserInfo dbInfo;
		dbInfo.FromUserInfo(info);
		nReturn = m_Database.UpdateUser(dbInfo, info.Passwd);
	}
	else if (cmd.cmd() == "DEL_CLIENT") {
		nReturn = m_Database.DeleteUser(cmd.get("client"));
	}
	else if (cmd.cmd() == "LIST_CLIENT") {
		std::string type = cmd.get("type");
		std::list<std::string> list;
		if (type != "ALL") {
			ClientMap::iterator ptr = m_Clients.begin();
			for (; ptr != m_Clients.end(); ++ptr) {
				list.push_back(ptr->first);
			}
		}
		else {
			m_Database.ListUsers(list);
		}

		std::string data;
		std::list<std::string>::iterator ptr = list.begin();
		for (; ptr != list.end(); ++ptr, data += "|") {
			data += *ptr;
		}
		int rc = koo_zmq_send_result(m_pReply, cmd, data);
		assert(rc >= 0);
		return;
	}
	else if (cmd.cmd() == "CLIENT_INFO") {
		std::string id = cmd.get("client");
		DbUserInfo dbInfo;
		int rc = m_Database.GetUser(id, dbInfo);
		if (rc != 0) {
			std::cerr << "Invalid User ID " << std::endl;
		}
		UserInfo info;
		memset(&info, 0, sizeof(UserInfo));
		dbInfo.ToUserInfo(info);
		koo_zmq_send_result(m_pReply, cmd, KOO_GEN_JSON(info));
		return;
	}
	else {
		std::cerr << "Unknown CMD... " << std::endl;
	}

	std::cout << "SEND REPLY: " << cmd.id() << "\t" << cmd.cmd() << "\t" << nReturn << std::endl;
	koo_zmq_send_result(m_pReply, cmd, nReturn);
}

int CClientMgr::SendToMapper(const std::string & id, const std::string & cmd, const json& val)
{
	std::string filter = id + " " + cmd;
	std::cout << "Publish to Mapper " << filter << std::endl;
	int rc = zmq_send(m_pPublish, filter.c_str(), filter.length(), ZMQ_SNDMORE);
	if (rc == -1) {
		std::cerr << "PUBLISH Failed filter: " << filter << std::endl;
		return rc;
	}
	std::stringstream ss;
	val >> ss;
	rc = zmq_send(m_pPublish, ss.str().c_str(), ss.str().length(), 0);
	if (rc == -1) {
		std::cerr << "PUBLISH Failed Data: " << std::endl;
	}
	return rc;
}

void CClientMgr::OnRecv()
{
	KZPacket cmd;
	int rc = koo_zmq_recv(m_pReply, cmd);
	if (rc == 0) {
		HandleKZPacket(cmd);
	}
}

int CClientMgr::AddMapper(const std::string & id)
{
	if (m_Mappers.find(id) != m_Mappers.end()) {
		RemoveMapper(id);
	}
	MapperData* pData = new MapperData();

	pData->ID = id;
	pData->Heartbeat = time(NULL);
	m_Mappers[id] = pData;

	return 0;
}

int CClientMgr::RemoveMapper(const std::string & id)
{
	std::cout << "Remove Client " << id << std::endl;

	MapperMap::iterator ptr = m_Mappers.find(id);
	if (ptr == m_Mappers.end())
		return -1;
	MapperData* pMapper = ptr->second;

	ClientMap::iterator cptr = m_Clients.begin();
	for (; cptr != m_Clients.end(); ++cptr) {
		ClientData* pClient = cptr->second;
		if (pClient != NULL) {
			for (int i = 0; i < RC_MAX_CONNECTION; ++i) {
				ConnectionData* Data = pClient->Connections[i];
				if (Data && Data->Mapper == ptr->second) {
					std::cout << "Mapper Destroy Stream Channel " << i << std::endl;
					delete Data;
					pClient->Connections[i] = NULL;
				}
			}
		}
	}

	delete pMapper;
	m_Mappers.erase(ptr);
	return 0;
}

MapperData * CClientMgr::FindMapper(const std::string & id)
{
	MapperMap::iterator ptr = m_Mappers.find(id);
	if (ptr == m_Mappers.end())
		return NULL;
	return ptr->second;;
}

int CClientMgr::AddClient(const std::string & id)
{
	if (m_Clients.find(id) != m_Clients.end()) {
		RemoveClient(id);
	}
	StreamProcess* pStreamServer = m_StreamMgr.Alloc();
	if (!pStreamServer)
		return -100;

	ClientData* pData = new ClientData();

	pData->ID = id;
	pData->Heartbeat = time(NULL);
	pData->StreamServer = pStreamServer;
	memset(pData->Connections, 0, sizeof(ConnectionData*) * RC_MAX_CONNECTION);
	m_Clients[id] = pData;
	return 0;
}

int CClientMgr::RemoveClient(const std::string & id)
{
	std::cout << "Remove Client " << id << std::endl;

	ClientMap::iterator ptr = m_Clients.find(id);
	if (ptr == m_Clients.end())
		return -1;

	for (int i = 0; i < RC_MAX_CONNECTION; ++i){
		ConnectionData* pConn = ptr->second->Connections[i];
		if (pConn) {
			FreeStream(id, i);
		}
	}

	StreamProcess* StreamServer = ptr->second->StreamServer;
	m_StreamMgr.Release(StreamServer);

	delete ptr->second;
	m_Clients.erase(ptr);
	return 0;
}

ClientData * CClientMgr::FindClient(const std::string & id)
{
	ClientMap::iterator ptr = m_Clients.find(id);
	if (ptr == m_Clients.end())
		return NULL;
	return ptr->second;
}

int CClientMgr::UpdateMapperHearbeat(const std::string & id)
{
	MapperMap::iterator ptr = m_Mappers.find(id);
	if (ptr == m_Mappers.end())
		return -1;
	ptr->second->Heartbeat = time(NULL);
	return 0;
}

int CClientMgr::UpdateClientHearbeat(const std::string & id)
{
	ClientMap::iterator ptr = m_Clients.find(id);
	if (ptr == m_Clients.end())
		return -1;
	ptr->second->Heartbeat = time(NULL);
	return 0;
}

int CClientMgr::AllocStream(const std::string & id, const std::string & mapper_id)
{
	ClientMap::iterator ptr = m_Clients.find(id);
	if (ptr == m_Clients.end())
		return -1;
	MapperMap::iterator mptr = m_Mappers.find(mapper_id);
	if (mptr == m_Mappers.end())
		return -1;

	ClientData* pClient = ptr->second;
	MapperData* pMapper = mptr->second;

	for (int i = 0; i < RC_MAX_CONNECTION; ++i) {
		if (!pClient->Connections[i]) {
			pClient->Connections[i] = new ConnectionData();
			pClient->Connections[i]->Channel = i;
			pClient->Connections[i]->Mapper = pMapper;
			return i;
		}
	}
	return -1;
}

MapperData * CClientMgr::FindStream(const std::string & id, int channel)
{
	ClientMap::iterator ptr = m_Clients.find(id);
	if (ptr == m_Clients.end())
		return NULL;

	ClientData* pClient = ptr->second;
	MapperData* pMapper = NULL;
	if (pClient->Connections[channel]) {
		pMapper = pClient->Connections[channel]->Mapper;
	}

	return pMapper;
}

int CClientMgr::FreeStream(const std::string& id, int channel)
{
	ClientMap::iterator ptr = m_Clients.find(id);
	if (ptr == m_Clients.end())
		return -1;
	
	ClientData* pClient = ptr->second;

	if (pClient->Connections[channel]) {
		ConnectionData * pData = pClient->Connections[channel];

		std::cout << "Client " << id << " Destroy Stream Channel " << channel << std::endl;
		json val;
		val["sp"] = KOO_GEN_JSON(*pClient->StreamServer);
		val["channel"] = channel;
		SendToMapper(pData->Mapper->ID, "DESTROY", val);

		delete pData;
		pClient->Connections[channel] = NULL;
		return 0;
	}
	return -1;
}

void CClientMgr::OnTimer(int nTime)
{
	std::list<std::string> rlist;
	// Check Clients
	ClientMap::iterator ptr = m_Clients.begin();
	for (; ptr != m_Clients.end(); ++ptr) {
		if (nTime - ptr->second->Heartbeat > CLIENT_HEARTBEAT_TIME * 100) {
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
		if (nTime - mptr->second->Heartbeat > MAPPER_HEARTBEAT_TIME * 100) {
			// Logger
			rlist.push_back(mptr->first);
		}
	}
	lptr = rlist.begin();
	for (; lptr != rlist.end(); ++lptr) {
		RemoveMapper(*lptr);
	}
}
