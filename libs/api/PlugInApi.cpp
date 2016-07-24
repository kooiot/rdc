#include "PlugInApi.h"
#include <cstring>
#include <cassert>
#include <cstdio>

#ifdef RDC_LINUX_SYS
#define LOAD_DLL_FUNC(HANDLE, FUNC, TYPE, NAME) \
	do { \
		FUNC = (TYPE)dlsym(HANDLE, #NAME);\
		err = dlerror(); \
		if (FUNC == NULL || err != NULL) { \
			dlclose(HANDLE); \
			fprintf(stderr, "%s\n", err);\
			return -1; \
		}\
	} while (0)
#else
#define LOAD_DLL_FUNC(HANDLE, FUNC, TYPE, NAME) \
	do { \
		FUNC = (TYPE)GetProcAddress(hApiHandle, #NAME); \
		if (FUNC == NULL) { \
			fprintf(stderr, "Failed Load %s\n", #NAME);\
			break; \
		}\
	} while (0)
#endif

int RDC_PluginLoad(const char * dll, PluginApi * api)
{
	assert(NULL != dll);
	assert(NULL != api);

	DWORD dwErr = 0;

#ifndef RDC_LINUX_SYS
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
#ifndef RDC_LINXU_SYS
	if (NULL != api->DllHandle)
		FreeLibrary(api->DllHandle);
#else
	if (NULL != api->DllHandle)
		dlclose(api->DllHandle);
#endif

	memset(api, 0, sizeof(PluginApi));
}

