#include "PluginLoader.h"

#include <cstdio>
#include <cstdlib>

#ifndef RDC_LINUX_SYS
#include <io.h>
#else
#include<sys/types.h>
#include<dirent.h>
#endif

CPluginLoader::CPluginLoader(RDC_PLUGIN_TYPE type) : m_Type(type)
{
}


CPluginLoader::~CPluginLoader()
{
}

void CPluginLoader::Load(const char * folder)
{
#ifndef RDC_LINUX_SYS
	intptr_t Handle;
	struct _finddata_t FileInfo;
	char temp[MAX_PATH];
	sprintf(temp, "%s\\*.dll", folder);

	Handle = _findfirst(temp, &FileInfo);
	if (Handle == -1) {
		fprintf(stderr, "Dll file not found\n");
		return;
	}

	while(true) {
		sprintf(temp, "%s\\%s", folder, FileInfo.name);
		LoadPlugin(temp);

		if (_findnext(Handle, &FileInfo) != 0)
			break;
	}

	_findclose(Handle);
	return;
#else
	dirent *filedata = NULL;
	DIR *curdir = opendir(folder);// opendir takes the dir path "." is current
	char temp[512];
	if (NULL == curdir)
	{
		fprintf(stderr, "directory not found\n");
		return; // Handle appropiatelly?
	}
	while (NULL != (filedata = readdir(curdir)))
	{
		if (filedata->d_name[0] == '.')
			continue;
		sprintf(temp, "%s/%s", folder, filedata->d_name);
		LoadPlugin(temp);
	}
	closedir(curdir);
#endif
}

void CPluginLoader::UnLoad()
{
	for (auto & ptr : m_Plugins) {
		RDC_PluginClose(ptr.second);
		delete ptr.second;
	}
	m_Plugins.clear();
}

PluginApi * CPluginLoader::Find(const char * name)
{
	auto ptr = m_Plugins.find(name);
	if (ptr != m_Plugins.end())
		return ptr->second;
	return nullptr;
}

void CPluginLoader::List(std::list<std::string>& list)
{
	list.clear();
	for (auto & ptr : m_Plugins) {
		list.push_back(ptr.first);
	}
}

void CPluginLoader::LoadPlugin(const char * dll)
{
	PluginApi* api = new PluginApi();
	int rc = RDC_PluginLoad(dll, api);
	if (rc != 0) {
		printf("Failed to load %s\n", dll);
		delete api;
		return;
	}
	int nType = api->GetType();
	if (!(nType | m_Type)) {
		printf("Incorrect plugin type loaded %s\n", dll);
		delete api;
		return;
	}

	printf("Load %s ok!\n", dll);
	const char* name = api->GetName();
	if (m_Plugins[name] != NULL) {
		printf("Plugin already loaded %s\n", name);
		RDC_PluginClose(api);
		delete api;
		return;
	}
	printf("Added plugin %s ok!\n", name);
	m_Plugins[name] = api;
}
