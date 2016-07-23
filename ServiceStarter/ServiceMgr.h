#pragma once
#include <list>

enum ServiceMode {
	SM_DISABLE = 0,
	SM_ONCE = 1,
	SM_AUTO = 2,
};
struct ServiceNode {
	std::string Name;
	std::string Desc;
	std::string Exec;
	std::string WorkDir;
	std::string Args;
	ServiceMode Mode;
};

typedef std::list<ServiceNode> ServiceNodeList;

class ServiceMgr
{
public:
	ServiceMgr();
	~ServiceMgr();

	void Load(const std::string& conf);
	void Save();

	void Run();
private:
	int AddNode(const ServiceNode& node);
	int UpdateNode(const ServiceNode& node);
	int DeleteNode(const std::string& name);
	int StartNode(const std::string& name);
	int StopNode(const std::string& name);
private:
	std::string m_Config;
	ServiceNodeList m_Nodes;
};

