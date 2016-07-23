#include "stdafx.h"
#include <zmq.h>
#include <iostream>
#include <string>
#include <fstream>
#include <json.hpp>
#include <koo_process.h>

#include "ServiceMgr.h"

ServiceMgr::ServiceMgr()
{
}


ServiceMgr::~ServiceMgr()
{
}

#define GET_NODE_STRING(NODE, NAME, BUF, BUF_LEN) \
	strncpy(BUF, NODE[#NAME].get<std::string>().c_str(), BUF_LEN)

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
#ifdef _DEBUG
		ServiceNode Node;
		strcpy(Node.Name, "Test");
		strcpy(Node.Desc, "This is one test server");
		strcpy(Node.Exec, "test.ext");
		strcpy(Node.WorkDir, "C:\\What a folder\\aaa");
		strcpy(Node.Args, "--conf=test --port=12313 --pxss=ture");
		Node.Mode = SM_DISABLE;
		AddNode(Node);
#endif
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
#define COMPARE_MSG_STR(msg, str) \
	strncmp((char*)zmq_msg_data(&msg), str, strlen(str)) == 0

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
	while (true) {
		zmq_msg_t cmd;
		int rc = zmq_msg_init(&cmd);

		rc = zmq_msg_recv(&cmd, rep, 0);
		if (rc == -1)
			continue;


		if (!zmq_msg_more(&cmd)) {
			zmq_msg_close(&cmd);
			continue;
		}

		zmq_msg_t data;
		rc = zmq_msg_init(&data);

		rc = zmq_msg_recv(&cmd, rep, 0);
		if (rc == -1)
			continue;
		
		if (COMPARE_MSG_STR(cmd, "LIST")) {
			nlohmann::json doc;
			int i = 0;
			ServiceNodeMap::iterator ptr = m_Nodes.begin();
			for (; ptr != m_Nodes.end(); ++ptr, ++i) {
				ServiceNodeEx* pNode = ptr->second;
				doc[i]["Name"] = pNode->Name;
				doc[i]["Desc"] = pNode->Desc;
				doc[i]["Exec"] = pNode->Exec;
				doc[i]["WorkDir"] = pNode->WorkDir;
				doc[i]["Args"] = pNode->Args;
				doc[i]["Mode"] = (int)pNode->Mode;
				doc[i]["Status"] = pNode->Process == NULL ? "STOPED" : "RUNNING";
			}

			// 
			continue;
		}
		else if (COMPARE_MSG_STR(cmd, "ADD")) {
			std::string str((char*)zmq_msg_data(&data), zmq_msg_size(&data));
			nlohmann::json doc(str);
			ServiceNode node;
			GET_NODE_STRING(doc, "Name", node.Name, RC_MAX_NAME_LEN);
			GET_NODE_STRING(doc, "Desc", node.Desc, RC_MAX_DESC_LEN);
			GET_NODE_STRING(doc, "Exec", node.Exec, RC_MAX_PATH);
			GET_NODE_STRING(doc, "WorkDir", node.WorkDir, RC_MAX_PATH);
			GET_NODE_STRING(doc, "Args", node.Args, RC_MAX_PATH);
			node.Mode = (ServiceMode)(int)doc["Mode"];
			rc = AddNode(node);
		}
		else if (COMPARE_MSG_STR(cmd, "DELETE")) {
			std::string name((char*)zmq_msg_data(&data), zmq_msg_size(&data));
			rc = DeleteNode(name);
		}
		else if (COMPARE_MSG_STR(cmd, "UPDATE")) {
			std::string str((char*)zmq_msg_data(&data), zmq_msg_size(&data));
			nlohmann::json doc(str);
			ServiceNode node;
			GET_NODE_STRING(doc, "Name", node.Name, RC_MAX_NAME_LEN);
			GET_NODE_STRING(doc, "Desc", node.Desc, RC_MAX_DESC_LEN);
			GET_NODE_STRING(doc, "Exec", node.Exec, RC_MAX_PATH);
			GET_NODE_STRING(doc, "WorkDir", node.WorkDir, RC_MAX_PATH);
			GET_NODE_STRING(doc, "Args", node.Args, RC_MAX_PATH);
			node.Mode = (ServiceMode)(int)doc["Mode"];
			rc = UpdateNode(node);
		}
		else if (COMPARE_MSG_STR(cmd, "START")) {
			std::string name((char*)zmq_msg_data(&data), zmq_msg_size(&data));
			rc = StartNode(name);
		}
		else if (COMPARE_MSG_STR(cmd, "STOP")) {
			std::string name((char*)zmq_msg_data(&data), zmq_msg_size(&data));
			rc = StopNode(name);
		}
		// TODO: Fire result
	}
}

int ServiceMgr::AddNode(const ServiceNode & node)
{
	ServiceNodeMap::iterator ptr = m_Nodes.find(node.Name);
	if (ptr != m_Nodes.end()) {
		return -1;
	}
	ServiceNodeEx* pNode = new ServiceNodeEx();
	memcpy(pNode, &node, sizeof(ServiceNode));

	m_Nodes[node.Name] = pNode;

	if (pNode->Mode != SM_DISABLE) {
		pNode->Process = new koo_process(node.Name, node.WorkDir, node.Exec, node.Args, false);
	}
	
	if (pNode->Mode == SM_ONCE)
		pNode->Process->start_once();
	if (pNode->Mode == SM_AUTO)
		pNode->Process->start();

	return 0;
}

int ServiceMgr::UpdateNode(const ServiceNode & node)
{
	ServiceNodeMap::iterator ptr = m_Nodes.find(node.Name);
	if (ptr != m_Nodes.end()) {
		return -1;
	}
	ServiceNodeEx* pNode = ptr->second;
	memcpy(pNode, &node, sizeof(ServiceNode));

	if (pNode->Mode == SM_DISABLE) {
		if (pNode->Process) {
			pNode->Process->stop();
			delete pNode->Process;
			pNode->Process = NULL;
		}
	}
	else {
		if (pNode->Process == NULL) {
			pNode->Process = new koo_process(pNode->Name, pNode->WorkDir, pNode->Exec, pNode->Args, false);
		}

		if (pNode->Mode == SM_ONCE)
			pNode->Process->start_once();
		if (pNode->Mode == SM_AUTO)
			pNode->Process->start();
	}
	return 0;
}

int ServiceMgr::DeleteNode(const std::string & name)
{
	ServiceNodeMap::iterator ptr = m_Nodes.find(name);
	if (ptr != m_Nodes.end()) {
		return -1;
	}
	ServiceNodeEx* pNode = ptr->second;

	if (pNode->Process) {
		pNode->Process->stop();
		delete pNode->Process;
		pNode->Process = NULL;
	}
	m_Nodes.erase(ptr);
}

int ServiceMgr::StartNode(const std::string & name)
{
	ServiceNodeMap::iterator ptr = m_Nodes.find(name);
	if (ptr != m_Nodes.end()) {
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

int ServiceMgr::StopNode(const std::string & name)
{
	ServiceNodeMap::iterator ptr = m_Nodes.find(name);
	if (ptr != m_Nodes.end()) {
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
