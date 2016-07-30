#include "ServicesApi.h"
#include <sstream>
#include <zmq.h>
#include <cassert>
#include <koo_zmq_helpers.h>
#include <koo_string_util.h>
#include <json.hpp>

#define GET_NODE_STRING(NODE, NAME, BUF, BUF_LEN) \
	strncpy(BUF, NODE[NAME].get<std::string>().c_str(), BUF_LEN)

CServicesApi::CServicesApi(void* ctx) : m_CTX(ctx), m_Socket(NULL)
{
}

CServicesApi::~CServicesApi()
{
}

bool CServicesApi::Connect()
{
	return _Connect();
}

bool CServicesApi::_Connect() 
{
	if (m_Socket != NULL) {
		zmq_close(m_Socket);
		m_Socket = NULL;
	}

	m_Socket = zmq_socket(m_CTX, ZMQ_REQ);
	assert(m_Socket);
	int timeo = 5 * 1000;
	int rc = zmq_setsockopt(m_Socket, ZMQ_RCVTIMEO, &timeo, sizeof(int));
	timeo = 0;
	rc = zmq_setsockopt(m_Socket, ZMQ_LINGER, &timeo, sizeof(int));
	rc = zmq_connect(m_Socket, "tcp://127.0.0.1:40010");
	assert(rc != -1);
	return rc == 0;
}

int CServicesApi::SendRequest(KZPacket& packet, std::function< int(KZPacket&)> cb)
{
	int rc = 0;
	{
		rc = koo_zmq_send(m_Socket, packet);
		if (rc >= 0) {
			KZPacket data;
			rc = koo_zmq_recv(m_Socket, data);
			if (rc == 0) {
				if (packet.cmd() == data.cmd())
					if (cb != nullptr)
					       return cb(data);
					else
					       return data.get("result");
				else if (data.cmd() == "TIMEOUT")
					return -10000;
				else
					return -20000;
			}
		}
	}
	if (rc != 0) {
		int err = errno;
		if (EAGAIN == errno && packet.cmd() != "LOGIN") {
			// Recreate the socket.
			bool br = _Connect();
			assert(br);
		}
	}
	return rc;
}

int CServicesApi::Disconnect()
{
	zmq_close(m_Socket);
	m_Socket = NULL;
	return 0;
}

int CServicesApi::Add(const ServiceNode * node)
{
	assert(node->Index == -1);
	nlohmann::json doc;
	doc["Index"] = node->Index;
	doc["Name"] = node->Name;
	doc["Desc"] = node->Desc;
	doc["Exec"] = node->Exec;
	doc["WorkDir"] = node->WorkDir;
	doc["Args"] = node->Args;
	doc["Mode"] = (int)node->Mode;

	KZPacket packet("KOOIOT", "ADD");
	packet.set("node", doc);

	int rc = SendRequest(packet);

	return rc;
}

int CServicesApi::Modify(const ServiceNode * node)
{
	nlohmann::json doc;
	doc["Index"] = node->Index;
	doc["Name"] = node->Name;
	doc["Desc"] = node->Desc;
	doc["Exec"] = node->Exec;
	doc["WorkDir"] = node->WorkDir;
	doc["Args"] = node->Args;
	doc["Mode"] = (int)node->Mode;

	KZPacket packet("KOOIOT", "UPDATE");
	packet.set("node", doc);

	int rc = SendRequest(packet);

	return rc;
}

int CServicesApi::Delete(int index)
{
	KZPacket packet("KOOIOT", "DELETE");
	packet.set("index", index);

	int rc = SendRequest(packet);

	return rc;
}

int CServicesApi::List(ServiceNode* list, int list_len, bool only_online)
{
	KZPacket packet("KOOIOT", "LIST");
	packet.set("type", only_online ? "ONLINE" : "ALL");
	int rc = SendRequest(packet, [list](KZPacket& data) {
		int i = 0;
		const json& doc = data.get("result");

		for (auto &node : doc) {
			ServiceNode* pNode = list + i;
			pNode->Index = node["Index"];
			GET_NODE_STRING(node, "Name", pNode->Name, RC_MAX_NAME_LEN);
			GET_NODE_STRING(node, "Desc", pNode->Desc, RC_MAX_DESC_LEN);
			GET_NODE_STRING(node, "Exec", pNode->Exec, RC_MAX_PATH);
			GET_NODE_STRING(node, "WorkDir", pNode->WorkDir, RC_MAX_PATH);
			GET_NODE_STRING(node, "Args", pNode->Args, RC_MAX_PATH);
			pNode->Mode = (ServiceMode)(int)node["Mode"];
			i++;
		}
		return i;
	});

	return rc;
}

int CServicesApi::Start(const char * name)
{
	KZPacket packet("KOOIOT", "START");
	packet.set("name", name);

	int rc = SendRequest(packet);

	return rc;
}

int CServicesApi::Stop(const char * name)
{
	KZPacket packet("KOOIOT", "STOP");
	packet.set("name", name);

	int rc = SendRequest(packet);

	return rc;
}
