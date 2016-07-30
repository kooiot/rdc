#include "stdafx.h"
#include <zmq.h>
#include <iostream>
#include <string>
#include <fstream>
#include <json.hpp>
#include <koo_process.h>
#include <koo_zmq_helpers.h>

#include "ServiceMgr.h"

bool g_bServerConsole = false;

ServiceMgr::ServiceMgr() : m_bAbort(false)
{
}


ServiceMgr::~ServiceMgr()
{
}

#define GET_NODE_STRING(NODE, NAME, BUF, BUF_LEN) \
	strncpy(BUF, NODE[NAME].get<std::string>().c_str(), BUF_LEN)

void ServiceMgr::Load(const std::string& conf)
{
	m_Nodes.clear();

	try {
		m_Config = conf;

		std::ifstream input(conf);
		nlohmann::json doc;
		doc << input;

		for (auto &node : doc) {
			ServiceNode Node;
			Node.Index = node["Index"];
			GET_NODE_STRING(node, "Name", Node.Name, RC_MAX_NAME_LEN);
			GET_NODE_STRING(node, "Desc", Node.Desc, RC_MAX_DESC_LEN);
			GET_NODE_STRING(node, "Exec", Node.Exec, RC_MAX_PATH);
			GET_NODE_STRING(node, "WorkDir", Node.WorkDir, RC_MAX_PATH);
			GET_NODE_STRING(node, "Args", Node.Args, RC_MAX_PATH);
			Node.Mode = (ServiceMode)(int)node["Mode"];
		
			AddNode(Node);
		}
		input.close();
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Failed loading ini file " << ex.what() <<  std::endl;
	}
	catch (...)
	{
		std::cerr << "Failed loading ini file" << std::endl;
	}
}


void ServiceMgr::Save()
{
	try {
		nlohmann::json doc;
		int i = 0;
		ServiceNodeMap::iterator ptr = m_Nodes.begin();
		for (; ptr != m_Nodes.end(); ++ptr, ++i) {
			ServiceNodeEx* pNode = ptr->second;
			doc[i]["Index"] = pNode->Index;
			doc[i]["Name"] = pNode->Name;
			doc[i]["Desc"] = pNode->Desc;
			doc[i]["Exec"] = pNode->Exec;
			doc[i]["WorkDir"] = pNode->WorkDir;
			doc[i]["Args"] = pNode->Args;
			doc[i]["Mode"] = (int)pNode->Mode;
		}
		std::ofstream output(m_Config);
		doc >> output;
	}
	catch(...) {

	}
}

void ServiceMgr::Run()
{
	void* ctx = zmq_ctx_new();
	void* rep = zmq_socket(ctx, ZMQ_REP);

	int rc = zmq_bind(rep, "tcp://127.0.0.1:40010");
	if (rc != 0) {
		zmq_close(rep);
#ifndef RDC_LINUX_SYS
		MessageBox(NULL, "Failed to bind on 40010", "Error", MB_ICONERROR);
#else
		printf("Failed to bind on 40010");
#endif
		return;
	}

	while (!m_bAbort) {
		zmq_pollitem_t items[] = {
			{ rep,   0, ZMQ_POLLIN, 0 },
		};
		zmq_poll(items, 1, 1000);
		if (items[0].revents & ZMQ_POLLIN) {
			ProcessReq(rep);
		}
	}

	// Close all services
	//ServiceNodeMap::iterator ptr = m_Nodes.begin();
	for (auto& node : m_Nodes) {
		if (node.second->Process) {
			node.second->Process->stop();
			delete node.second->Process;
			node.second->Process = NULL;
		}
	}

	zmq_close(rep);
	zmq_ctx_term(ctx);
}

void ServiceMgr::Stop()
{
	m_bAbort = true;
}

void ServiceMgr::ProcessReq(void* socket)
{
	KZPacket packet;
	int rc = koo_zmq_recv(socket, packet);
	if (rc != 0 || packet.id() != "KOOIOT")
		return;

	if (packet.cmd() == "LIST") {
		nlohmann::json doc;
		int i = 0;
		ServiceNodeMap::iterator ptr = m_Nodes.begin();
		for (; ptr != m_Nodes.end(); ++ptr, ++i) {
			ServiceNodeEx* pNode = ptr->second;
			doc[i]["Index"] = pNode->Index;
			doc[i]["Name"] = pNode->Name;
			doc[i]["Desc"] = pNode->Desc;
			doc[i]["Exec"] = pNode->Exec;
			doc[i]["WorkDir"] = pNode->WorkDir;
			doc[i]["Args"] = pNode->Args;
			doc[i]["Mode"] = (int)pNode->Mode;
			doc[i]["Status"] = pNode->Process == NULL ? "STOPED" : "RUNNING";
		}
		koo_zmq_send_result(socket, packet, doc);
		return;
	}
	else if (packet.cmd() == "ADD") {
		nlohmann::json doc = packet.get("node");
		ServiceNode node;
		std::string name = doc["Name"];
		node.Index = doc["Index"];
		GET_NODE_STRING(doc, "Name", node.Name, RC_MAX_NAME_LEN);
		GET_NODE_STRING(doc, "Desc", node.Desc, RC_MAX_DESC_LEN);
		GET_NODE_STRING(doc, "Exec", node.Exec, RC_MAX_PATH);
		GET_NODE_STRING(doc, "WorkDir", node.WorkDir, RC_MAX_PATH);
		GET_NODE_STRING(doc, "Args", node.Args, RC_MAX_PATH);
		node.Mode = (ServiceMode)(int)doc["Mode"];
		if (node.Index == -1)
			rc = AddNode(node);
		else
			rc = -1;
	}
	else if (packet.cmd() == "DELETE") {
		int index = packet.get("index");
		rc = DeleteNode(index);
	}
	else if (packet.cmd() == "UPDATE") {
		nlohmann::json doc = packet.get("node");
		ServiceNode node;
		node.Index = doc["Index"];
		GET_NODE_STRING(doc, "Name", node.Name, RC_MAX_NAME_LEN);
		GET_NODE_STRING(doc, "Desc", node.Desc, RC_MAX_DESC_LEN);
		GET_NODE_STRING(doc, "Exec", node.Exec, RC_MAX_PATH);
		GET_NODE_STRING(doc, "WorkDir", node.WorkDir, RC_MAX_PATH);
		GET_NODE_STRING(doc, "Args", node.Args, RC_MAX_PATH);
		node.Mode = (ServiceMode)(int)doc["Mode"];
		rc = UpdateNode(node);
	}
	else if (packet.cmd() == "START") {
		int index = packet.get("index");
		rc = StartNode(index);
	}
	else if (packet.cmd() == "STOP") {
		int index = packet.get("index");
		rc = StopNode(index);
	}
	koo_zmq_send_result(socket, packet, rc);
}

int ServiceMgr::AddNode(const ServiceNode & node)
{
	ServiceNodeEx* pNode = new ServiceNodeEx();
	memcpy(pNode, &node, sizeof(ServiceNode));

	if (node.Index == -1) {
		int index = 0;
		while (index < 64) {
			auto ptr = m_Nodes.begin();
			for (; ptr != m_Nodes.end(); ++ptr) {
				if (ptr->first == index)
					break;
			}
			if (ptr == m_Nodes.end())
				break;
			index++;
		}
		if (index >= 64)
			return -1;

		pNode->Index = index;
	}

	m_Nodes[pNode->Index] = pNode;

	if (pNode->Mode != SM_DISABLE) {
		pNode->Process = new koo_process(node.Name, node.WorkDir, node.Exec, node.Args, g_bServerConsole);
	}
	else {
		pNode->Process = NULL;
	}
	
	if (pNode->Mode == SM_ONCE)
		pNode->Process->start_once();
	if (pNode->Mode == SM_AUTO)
		pNode->Process->start();

	Save();
	return pNode->Index;
}

int ServiceMgr::UpdateNode(const ServiceNode & node)
{
	ServiceNodeMap::iterator ptr = m_Nodes.find(node.Index);
	if (ptr == m_Nodes.end()) {
		return -1;
	}
	ServiceNodeEx* pNode = ptr->second;
	memcpy(pNode, &node, sizeof(ServiceNode));

	if (pNode->Process) {
		pNode->Process->stop();
		delete pNode->Process;
		pNode->Process = NULL;
	}

	if (pNode->Mode != SM_DISABLE) {
		if (pNode->Process == NULL) {
			pNode->Process = new koo_process(pNode->Name, pNode->WorkDir, pNode->Exec, pNode->Args, g_bServerConsole);
		}

		if (pNode->Mode == SM_ONCE)
			pNode->Process->start_once();
		if (pNode->Mode == SM_AUTO)
			pNode->Process->start();
	}
	Save();
	return 0;
}

int ServiceMgr::DeleteNode(int index)
{
	ServiceNodeMap::iterator ptr = m_Nodes.find(index);
	if (ptr == m_Nodes.end()) {
		return -1;
	}
	ServiceNodeEx* pNode = ptr->second;

	if (pNode->Process) {
		pNode->Process->stop();
		delete pNode->Process;
		pNode->Process = NULL;
	}
	m_Nodes.erase(ptr);
	Save();
	return 0;
}

int ServiceMgr::StartNode(int index)
{
	ServiceNodeMap::iterator ptr = m_Nodes.find(index);
	if (ptr == m_Nodes.end()) {
		return -1;
	}
	ServiceNodeEx* pNode = ptr->second;

	if (pNode->Process) {
		pNode->Process->stop();
		delete pNode->Process;
		pNode->Process = NULL;
	}
	pNode->Process = new koo_process(pNode->Name, pNode->WorkDir, pNode->Exec, pNode->Args, false);

	if (pNode->Mode != SM_AUTO)
		pNode->Process->start_once();
	else if (pNode->Mode == SM_ONCE)
		pNode->Process->start();

	return 0;
}

int ServiceMgr::StopNode(int index)
{
	ServiceNodeMap::iterator ptr = m_Nodes.find(index);
	if (ptr == m_Nodes.end()) {
		return -1;
	}
	ServiceNodeEx* pNode = ptr->second;

	if (pNode->Process) {
		pNode->Process->stop();
		delete pNode->Process;
		pNode->Process = NULL;
	}
	return 0;
}
