#include "ClientMgr.h"
#include <DataDefs.h>
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

	m_Database.Init();

	return rep;
}

void CClientMgr::Close()
{
	m_Database.Close();
	zmq_close(m_pReply);
	zmq_close(m_pPublish);
}

void CClientMgr::HandleKZPacket(const KZPacket& cmd, void* rep, void* pub)
{
	std::cout << __FUNCTION__ << ": " << cmd.id << "\t" << cmd.cmd << std::endl;

	int nReturn = -1;
	if (cmd.cmd == "LOGIN") {
		std::cout << "LOGIN: " << cmd.id << "\t" << cmd.GetStr() << std::endl;
		if (cmd.id == "@DEVICE@") {
			std::string mapper_id = cmd.GetStr();
			// Check for valid device sn
			if (m_Database.IsValidMapper(mapper_id) == 0) {
				AddMapper(mapper_id);
				nReturn = 0;
			}
		}
		else if (m_Database.Login(cmd.id, cmd.GetStr()) == 0) {
			AddClient(cmd.id);
			nReturn = 0;
		}
	}
	else if (cmd.cmd == "HEARTBEAT") {
		nReturn = 0;
		if (cmd.id == "@DEVICE@")
			UpdateMapperHearbeat(cmd.GetStr());
		else
			UpdateClientHearbeat(cmd.id);
	}
	else if (cmd.cmd == "LOGOUT") {
		nReturn = 0;
		if (cmd.id == "@DEVICE@")
			RemoveMapper(cmd.GetStr());
		else
			RemoveClient(cmd.id);
	}
	else if (cmd.cmd == "STREAM") {
		IPInfo info;
		memset(&info, 0, sizeof(IPInfo));
		ClientData* pData = m_Clients[cmd.id];
		if (pData && pData->StreamServer) {
			memcpy(info.sip, pData->StreamServer->StreamIP, RC_MAX_IP_LEN);
			info.port = pData->StreamServer->Port;
		}
		int rc = koo_zmq_send_reply(rep, cmd, &info, sizeof(IPInfo));
		assert(rc == 0);
		return;
	}
	else if (cmd.cmd == "CREATE") {
		ConnectionInfo info;
		int index = -1;
		memcpy(&info, cmd.GetData(), sizeof(ConnectionInfo));
		if (m_Database.Access(cmd.id, info.DevSN)) {
			// FIXME:
			index = 0;
		}
		std::stringstream ss;
		ss << index;
		int rc = koo_zmq_send_reply(rep, cmd, ss.str());
		assert(rc == 0);
		return;
	}
	else if (cmd.cmd == "DESTROY") {
		int index = atoi(cmd.GetStr().c_str());
		if (index > 0 && index < RC_MAX_CONNECTION) {
			// FIXME:
			nReturn = 0;
		}
	}
	else if (cmd.cmd == "ADD_DEV") {
		DeviceInfo* pInfo = (DeviceInfo*)cmd.GetData();
		DbDeviceInfo dbInfo;
		dbInfo.FromDeviceInfo(*pInfo);
		nReturn = m_Database.AddDevice(dbInfo);
	}
	else if (cmd.cmd == "MOD_DEV") {
		DeviceInfo* pInfo = (DeviceInfo*)cmd.GetData();
		DbDeviceInfo dbInfo;
		dbInfo.FromDeviceInfo(*pInfo);
		nReturn = m_Database.UpdateDevice(dbInfo);
	}
	else if (cmd.cmd == "DEL_DEV") {
		nReturn = m_Database.DeleteDevice(cmd.GetStr());
	}
	else if (cmd.cmd == "LIST_DEV") {
		std::string type = cmd.GetStr();
		std::list<std::string> list;
		if (type != "ALL") {
			MapperMap::iterator ptr = m_Mappers.begin();
			for (; ptr != m_Mappers.end(); ++ptr) {
				list.push_back(ptr->first);
			}
		}
		else {
			m_Database.ListDevices(list);
		}

		std::string data;
		std::list<std::string>::iterator ptr = list.begin();
		for (; ptr != list.end(); ++ptr, data += "|") {
			data += *ptr;
		}
		int rc = koo_zmq_send_reply(rep, cmd, data);
		assert(rc == 0);
		return;
	}
	else if (cmd.cmd == "DEV_INFO") {
		std::string sn = cmd.GetStr();
		DbDeviceInfo dbInfo;
		int rc = m_Database.GetDevice(sn, dbInfo);
		if (rc != 0) {
			std::cerr << "Invalid Device ID " << std::endl;

		}
		DeviceInfo info;
		memset(&info, 0, sizeof(DeviceInfo));
		dbInfo.ToDeviceInfo(info);
		koo_zmq_send_reply(rep, cmd, &info, sizeof(DeviceInfo));
		return;
	}
	else if (cmd.cmd == "ADD_CLIENT") {
		UserInfo* pInfo = (UserInfo*)cmd.GetData();
		DbUserInfo dbInfo;
		dbInfo.FromUserInfo(*pInfo);
		nReturn = m_Database.AddUser(dbInfo, pInfo->Passwd);
	}
	else if (cmd.cmd == "MOD_CLIENT") {
		UserInfo* pInfo = (UserInfo*)cmd.GetData();
		DbUserInfo dbInfo;
		dbInfo.FromUserInfo(*pInfo);
		nReturn = m_Database.UpdateUser(dbInfo, pInfo->Passwd);
	}
	else if (cmd.cmd == "DEL_CLIENT") {
		nReturn = m_Database.DeleteUser(cmd.GetStr());
	}
	else if (cmd.cmd == "LIST_CLIENT") {
		std::string type = cmd.GetStr();
		std::list<std::string> list;
		if (type != "ALL") {
			ClientMap::iterator ptr = m_Clients.begin();
			for (; ptr != m_Clients.end(); ++ptr) {
				list.push_back(ptr->first);
			}
		}
		else {
			m_Database.ListDevices(list);
		}

		std::string data;
		std::list<std::string>::iterator ptr = list.begin();
		for (; ptr != list.end(); ++ptr, data += "|") {
			data += *ptr;
		}
		int rc = koo_zmq_send_reply(rep, cmd, data);
		assert(rc == 0);
		return;
	}
	else if (cmd.cmd == "CLIENT_INFO") {
		std::string id = cmd.GetStr();
		DbUserInfo dbInfo;
		int rc = m_Database.GetUser(id, dbInfo);
		if (rc != 0) {
			std::cerr << "Invalid User ID " << std::endl;
		}
		UserInfo info;
		memset(&info, 0, sizeof(UserInfo));
		dbInfo.ToUserInfo(info);
		koo_zmq_send_reply(rep, cmd, &info, sizeof(UserInfo));
		return;
	}
	std::cout << "SEND REPLY: " << cmd.id << "\t" << cmd.cmd << "\t" << nReturn << std::endl;
	std::stringstream ss;
	ss << nReturn;
	koo_zmq_send_reply(rep, cmd, ss.str());
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
	m_Mappers[id] = pData;

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
	m_Clients[id] = pData;
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
