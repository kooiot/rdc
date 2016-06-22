#include "ClientMgr.h"
#include <zmq.h>

#include <list>
#include <sstream>
#include <cassert>

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
	return rep;
}

void CClientMgr::Close()
{
	zmq_close(m_pReply);
	zmq_close(m_pPublish);
}


bool recv_string(void* skt, std::string& str, bool more) {
	zmq_msg_t part;
	int rc = zmq_msg_init(&part);
	assert(rc == 0);
	rc = zmq_msg_recv(&part, skt, 0);
	assert(rc != -1);
	if (more) {
		int more;
		size_t more_size = sizeof(more);
		rc = zmq_getsockopt(skt, ZMQ_RCVMORE, &more, &more_size);
		assert(rc == 0);

		if (!more) {
			zmq_msg_close(&part);
			return false;
		}
	}
	str = std::string((char*)zmq_msg_data(&part), zmq_msg_size(&part));

	zmq_msg_close(&part);
	return true;
}
#define RECV_STRING(skt, x, more) \
	if (!recv_string(skt, x, more)) \
		return -1;

int recv_cmd(void* skt, CMD& cmd) {
	RECV_STRING(skt, cmd.id, true);
	RECV_STRING(skt, cmd.cmd, true);
	RECV_STRING(skt, cmd.data, false);
	return 0;
}

int send_msg(void* skt, const std::string& data, bool more) {
	zmq_msg_t msg;
	zmq_msg_init_data(&msg, (void*)data.c_str(), data.length(), NULL, NULL);
	int rc = zmq_msg_send(&msg, skt, more ? ZMQ_MORE : 0);
	zmq_msg_close(&msg);
	return rc;
}

#define SEND_MSG(skt, data, more) \
	if (!send_msg(skt, data, more)) \
		return -1;

int send_reply(void* skt, const CMD& cmd, const std::string& rep) {
	SEND_MSG(skt, cmd.id, true);
	SEND_MSG(skt, cmd.cmd, true);
	SEND_MSG(skt, rep, false);
	return 0;
}

void CClientMgr::HandleCMD(const CMD& cmd, void* rep, void* pub)
{
	if (cmd.cmd == "LOGIN") {
		if (cmd.id == "TEST" && cmd.data == "PASSWORD") {
			send_reply(rep, cmd, "SUCCESS"); // FIXME: WITH ID
		}
		else {
			send_reply(rep, cmd, "FAILURE"); // FIXME: WITH ID
		}
	}
	if (cmd.cmd == "HEARTBEAT") {
		send_reply(rep, cmd, "SUCCESS");
	}
	if (cmd.cmd == "LOGOUT") {
		send_reply(rep, cmd, "SUCCESS");
		// FIXME:
	}
	if (cmd.cmd == "CREATE") {
		// Check for permissions
		std::string dest_id = cmd.data.substr(0, cmd.data.find(" "));
		// FIXME:Check for dest online or not
		// Forwarding request
		send_msg(pub, cmd.data, false);

		send_reply(rep, cmd, "SUCCESS");
	}
	if (cmd.cmd == "DESTROY") {
		// Check for permissions
		std::string dest_id = cmd.data.substr(0, cmd.data.find(" "));
		// FIXME:Check for dest online or not
		// Forwarding request
		send_msg(pub, cmd.data, false);

		send_reply(rep, cmd, "SUCCESS");
	}
}

void CClientMgr::OnRecv()
{
	CMD cmd;
	int rc = recv_cmd(m_pReply, cmd);
	if (rc == 0) {
		HandleCMD(cmd, m_pReply, m_pPublish);
	}
}

int CClientMgr::AddMapper(const std::string & id, ResourceMap Resources)
{
	if (m_Mappers.find(id) != m_Mappers.end()) {
		RemoveMapper(id);
	}
	MapperData* pData = new MapperData();

	pData->ID = id;
	pData->Heartbeat = ::GetCurrentTime();
	pData->Resources = Resources;

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
