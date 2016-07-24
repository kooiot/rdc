#pragma once
#include <map>
#include <string>
#include <DataDefs.h>
class koo_process;

struct ServiceNodeEx : public ServiceNode
{
	ServiceNodeEx() : Process(NULL) {};
	koo_process* Process;
};

typedef std::map<std::string, ServiceNodeEx*> ServiceNodeMap;

class ServiceMgr
{
public:
	ServiceMgr();
	~ServiceMgr();

	void Load(const std::string& conf);
	void Save();

	void Run();
	void Stop();
private:
	void ProcessReq(void* socket);
	int AddNode(const ServiceNode& node);
	int UpdateNode(const ServiceNode& node);
	int DeleteNode(const std::string& name);
	int StartNode(const std::string& name);
	int StopNode(const std::string& name);
private:
	bool m_bAbort;
	std::string m_Config;
	ServiceNodeMap m_Nodes;
};

