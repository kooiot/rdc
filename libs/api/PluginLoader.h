#pragma once

#include "PlugInApi.h"
#include "DataDefs.h"
#include <map>
#include <list>
#include <string>

typedef void* PluginHandle;

class CPluginLoader
{
public:
	CPluginLoader(RDC_PLUGIN_TYPE type);
	~CPluginLoader();

	void Load(const char* folder);
	void UnLoad();

	PluginApi* Find(const char* name);
	void List(std::list<std::string>& list);
private:
	void LoadPlugin(const char* dll);
private:
	RDC_PLUGIN_TYPE m_Type;
	std::map<std::string, PluginApi*> m_Plugins;
};

