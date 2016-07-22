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

	void Load(const char* conf);
	void Save(const char* conf);

	void Run();
private:
	std::string m_Config;
	ServiceNodeList m_Nodes;
};

