#include "stdafx.h"
#include "ServiceMgr.h"
#include <iostream>
#include <fstream>
#include "json.hpp"

ServiceMgr::ServiceMgr()
{
}


ServiceMgr::~ServiceMgr()
{
}

void ServiceMgr::Load(const char * conf)
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


void ServiceMgr::Save(const char* conf)
{
	try {
		ServiceNodeList::iterator ptr = m_Nodes.begin();
		nlohmann::json doc;
		int i = 0;
		for (; ptr != m_Nodes.end(); ++ptr, ++i) {
			doc[i]["Name"] = ptr->Name;
			doc[i]["Desc"] = ptr->Desc;
			doc[i]["Exec"] = ptr->Exec;
			doc[i]["WorkDir"] = ptr->WorkDir;
			doc[i]["Args"] = ptr->Args;
			doc[i]["Mode"] = (int)ptr->Mode;
		}
		std::ofstream output(conf);
		doc >> output;
	}
	catch(...) {

	}
}

void ServiceMgr::Run()
{
	Save(m_Config.c_str());
}