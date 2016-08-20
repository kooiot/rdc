#include "AccMgr.h"
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


CAccMgr::CAccMgr(CClientMgr& CliengMgr, CAccDatabase& Database)
	: m_pReply(NULL), m_ClientMgr(CliengMgr), m_Database(Database)
{
}


CAccMgr::~CAccMgr()
{
}

void * CAccMgr::Init(void * ctx, const char * bip, int port)
{
	void* rep = zmq_socket(ctx, ZMQ_REP);

	std::stringstream ss;

	ss << "tcp://" << bip << ":" << port;
	int rc = zmq_bind(rep, ss.str().c_str());
	if (rc != 0) {
		std::cout << "Acc Admin Socket Cannot bind to " << ss.str() << std::endl;
		zmq_close(rep);
		return NULL;
	}
	
	m_pReply = rep;

	return rep;
}

void CAccMgr::Close()
{
	zmq_close(m_pReply);
}

void CAccMgr::HandleKZPacket(const KZPacket& cmd)
{
	std::cout << __FUNCTION__ << ": " << cmd.id() << "\t" << cmd.cmd() << std::endl;

	int nReturn = -1;
	if (cmd.cmd() == "LOGIN") {
		std::cout << "Acc Admin LOGIN: " << cmd.id() << "\t" << cmd.get("pass") << std::endl;
		if (m_Database.Login(cmd.id(), cmd.get("pass")) == 0) {
			DbUserInfo info;
			m_Database.GetUser(cmd.id(), info);
			if (info.Level >= UL_SYS_ADMIN)
				nReturn = 0;
		}
	}
	else if (cmd.cmd() == "LOGOUT") {
		nReturn = 0;
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
		std::list<std::string> all;
		std::list<std::string> list;
		if (type != "ALL") {
			m_ClientMgr.ListMappers(all);
		}
		else {
			m_Database.ListDevices(all);
		}
		for (std::list<std::string>::iterator ptr = all.begin();
			ptr != all.end(); ++ptr) {
			if (0 == m_Database.Access(cmd.id(), *ptr)) {
				list.push_back(*ptr);
			}
		}

		int rc = koo_zmq_send_result(m_pReply, cmd, KOO_GEN_JSON(list));
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
	else if (cmd.cmd() == "ADD_GROUP") {
		JSON_FROM_PACKET(cmd, GroupInfo, info);
		DbGroupInfo dbInfo;
		dbInfo.FromGroupInfo(info);
		nReturn = m_Database.AddGroup(dbInfo);
	}
	else if (cmd.cmd() == "MOD_GROUP") {
		JSON_FROM_PACKET(cmd, GroupInfo, info);
		DbGroupInfo dbInfo;
		dbInfo.FromGroupInfo(info);
		nReturn = m_Database.UpdateGroup(dbInfo);
	}
	else if (cmd.cmd() == "DEL_GROUP") {
		nReturn = m_Database.DeleteGroup(cmd.get("name"));
	}
	else if (cmd.cmd() == "LIST_GROUP") {
		std::list<std::string> all;
		m_Database.ListDevices(all);
		int rc = koo_zmq_send_result(m_pReply, cmd, KOO_GEN_JSON(all));
		assert(rc >= 0);
		return;
	}
	else if (cmd.cmd() == "GROUP_INFO") {
		std::string name = cmd.get("name");
		DbGroupInfo dbInfo;
		int rc = m_Database.GetGroup(name, dbInfo);
		if (rc != 0) {
			std::cerr << "Invalid Group Name " << name << std::endl;

		}
		GroupInfo info;
		memset(&info, 0, sizeof(GroupInfo));
		dbInfo.ToGroupInfo(info);
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
			m_ClientMgr.ListClients(list);
		}
		else {
			m_Database.ListUsers(list);
		}

		int rc = koo_zmq_send_result(m_pReply, cmd, KOO_GEN_JSON(list));
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

void CAccMgr::OnRecv()
{
	KZPacket cmd;
	int rc = koo_zmq_recv(m_pReply, cmd);
	if (rc == 0) {
		HandleKZPacket(cmd);
	}
}
