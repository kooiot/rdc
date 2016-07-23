#include "stdafx.h"
#include <zmq.h>
#include <iostream>
#include <string>
#include <fstream>
#include "json.hpp"

#include "ServiceMgr.h"

ServiceMgr::ServiceMgr()
{
}


ServiceMgr::~ServiceMgr()
{
}

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
			Node.Name = node["Name"];
			Node.Desc = node["Desc"];
			Node.Exec = node["Exec"];
			Node.WorkDir = node["WorkDir"];
			Node.Args = node["Args"];
			Node.Mode = (ServiceMode)(int)node["Mode"];
		
			m_Nodes.push_back(Node);
		}
		input.close();
	}
	catch (const std::exception& ex)
	{
		ServiceNode Node;
		Node.Name = "Test";
		Node.Desc = "This is one test server";
		Node.Exec = "test.ext";
		Node.WorkDir = "C:\\What a folder\\aaa";
		Node.Args = "--conf=test --port=12313 --pxss=ture";
		Node.Mode = SM_DISABLE;
		m_Nodes.push_back(Node);

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
		ServiceNodeList::iterator ptr = m_Nodes.begin();
		for (; ptr != m_Nodes.end(); ++ptr, ++i) {
			doc[i]["Name"] = ptr->Name;
			doc[i]["Desc"] = ptr->Desc;
			doc[i]["Exec"] = ptr->Exec;
			doc[i]["WorkDir"] = ptr->WorkDir;
			doc[i]["Args"] = ptr->Args;
			doc[i]["Mode"] = (int)ptr->Mode;
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
			ServiceNodeList::iterator ptr = m_Nodes.begin();
			for (; ptr != m_Nodes.end(); ++ptr, ++i) {
				doc[i]["Name"] = ptr->Name;
				doc[i]["Desc"] = ptr->Desc;
				doc[i]["Exec"] = ptr->Exec;
				doc[i]["WorkDir"] = ptr->WorkDir;
				doc[i]["Args"] = ptr->Args;
				doc[i]["Mode"] = (int)ptr->Mode;
				doc[i]["Status"] = "RUNNING"; // STOPED
			}

			// 
			continue;
		}
		else if (COMPARE_MSG_STR(cmd, "ADD")) {
			std::string str((char*)zmq_msg_data(&data), zmq_msg_size(&data));
			nlohmann::json doc(str);
			ServiceNode node;
			node.Name = doc["Name"];
			node.Desc = doc["Desc"];
			node.Exec = doc["Exec"];
			node.WorkDir = doc["WorkDir"];
			node.Args = doc["Args"];
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
			node.Name = doc["Name"];
			node.Desc = doc["Desc"];
			node.Exec = doc["Exec"];
			node.WorkDir = doc["WorkDir"];
			node.Args = doc["Args"];
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
	return 0;
}

int ServiceMgr::UpdateNode(const ServiceNode & node)
{
	return 0;
}

int ServiceMgr::DeleteNode(const std::string & name)
{
	return 0;
}

int ServiceMgr::StartNode(const std::string & name)
{
	return 0;
}

int ServiceMgr::StopNode(const std::string & name)
{
	return 0;
}
