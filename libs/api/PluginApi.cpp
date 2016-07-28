#include "PluginApi.h"
#include <cstring>
#include <cassert>
#include <cstdio>
#include <json.hpp>

using json = nlohmann::json;

#ifdef RDC_LINUX_SYS
#define LOAD_DLL_FUNC(HANDLE, FUNC, TYPE, NAME) \
	{ \
		FUNC = (TYPE)dlsym(HANDLE, #NAME);\
		char* err = dlerror(); \
		if (FUNC == NULL || err != NULL) { \
			dlclose(HANDLE); \
			fprintf(stderr, "%s\n", err);\
			break; \
		}\
	}
#else
#define LOAD_DLL_FUNC(HANDLE, FUNC, TYPE, NAME) \
	{ \
		FUNC = (TYPE)GetProcAddress(hApiHandle, #NAME); \
		if (FUNC == NULL) { \
			fprintf(stderr, "Failed Load %s\n", #NAME);\
			break; \
		}\
	}
#endif

#ifdef RDC_LINUX_SYS
#define LOAD_DLL_FUNC_OPT(HANDLE, FUNC, TYPE, NAME, DEFAULT) \
	do { \
		FUNC = (TYPE)dlsym(HANDLE, #NAME);\
		if (!FUNC) \
			FUNC = DEFAULT; \
	} while (0)
#else
#define LOAD_DLL_FUNC_OPT(HANDLE, FUNC, TYPE, NAME, DEFAULT) \
	do { \
		FUNC = (TYPE)GetProcAddress(hApiHandle, #NAME); \
		if (!FUNC) \
			FUNC = DEFAULT; \
	} while (0)
#endif

const char* NO_GetInfo() {
	static json info;
	info["desc"] = "Plugin does not implement the GetInfo function.";
	info["version"] = "0.0.0";
	info["author"] = "Unknown";
	static std::stringstream ss;
	info >> ss;
	return ss.str().c_str();
}
int RDC_PluginLoad(const char * dll, PluginApi * api)
{
	assert(NULL != dll);
	assert(NULL != api);


#ifndef RDC_LINUX_SYS
	DWORD dwErr = 0;
	HMODULE hApiHandle = LoadLibrary(dll);
#else
	void*	hApiHandle = dlopen(dll, RTLD_LAZY);
#endif

	memset(api, 0, sizeof(PluginApi));

	if (NULL == hApiHandle)
		return -1;

	api->DllHandle = hApiHandle;

	do
	{
		LOAD_DLL_FUNC(hApiHandle, api->GetType, FGetType, GetType);
		LOAD_DLL_FUNC(hApiHandle, api->GetName, FGetName, GetName);
		LOAD_DLL_FUNC(hApiHandle, api->CreateHandle, FCreateHandle, CreateHandle);
		LOAD_DLL_FUNC(hApiHandle, api->Destory, FDestory, Destory);
		LOAD_DLL_FUNC(hApiHandle, api->Open, FOpen, Open);
		LOAD_DLL_FUNC(hApiHandle, api->Close, FClose, Close);
		LOAD_DLL_FUNC(hApiHandle, api->Write, FWrite, Write);
		LOAD_DLL_FUNC_OPT(hApiHandle, api->GetInfo, FGetInfo, GetInfo, NO_GetInfo);
		return 0;

	} while (0);

#ifndef RDC_LINUX_SYS
	////
	dwErr = GetLastError();
#endif

	// close dll when any error occured
	RDC_PluginClose(api);

#ifndef RDC_LINUX_SYS
	SetLastError(dwErr);
#endif

	return -1;
}

void RDC_PluginClose(PluginApi* api)
{
#ifndef RDC_LINUX_SYS
	if (NULL != api->DllHandle)
		FreeLibrary(api->DllHandle);
#else
	if (NULL != api->DllHandle)
		dlclose(api->DllHandle);
#endif

	memset(api, 0, sizeof(PluginApi));
}

