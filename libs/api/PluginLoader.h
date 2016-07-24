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
	CPluginLoader();
	~CPluginLoader();

	void Load(const char* folder);
	void UnLoad();

	PluginApi* Find(const char* name);
private:
	void LoadPlugin(const char* dll);
private:
	std::map<std::string, PluginApi*> m_Plugins;
};

