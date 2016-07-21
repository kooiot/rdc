#include "PlugInApi.h"
#include <cstring>
#include <cassert>


int RDC_PluginLoad(const char * dll, PluginApi * api)
{
	assert(NULL != dll);
	assert(NULL != api);

	DWORD dwErr = 0;
	HMODULE hApiHandle = NULL;

	memset(api, 0, sizeof(PluginApi));
	hApiHandle = LoadLibrary(dll);

	if (NULL == hApiHandle)
		return -1;

	api->DllHandle = hApiHandle;

	do
	{
		api->GetName = (FGetName)GetProcAddress(
			hApiHandle, "GetName");
		if (NULL == api->GetName)
			break;

		api->CreateHandle = (FCreateHandle)GetProcAddress(
			hApiHandle, "CreateHandle");
		if (NULL == api->CreateHandle)
			break;

		api->Destory = (FDestory)GetProcAddress(
			hApiHandle, "Destory");
		if (NULL == api->Destory)
			break;

		api->Start = (FStart)GetProcAddress(
			hApiHandle, "Start");
		if (NULL == api->Start)
			break;

		api->Stop = (FStop)GetProcAddress(
			hApiHandle, "Stop");
		if (NULL == api->Stop)
			break;

		return 0;

	} while (0);

	////
	dwErr = GetLastError();

	// close dll when any error occured
	RDC_PluginClose(api);

	SetLastError(dwErr);

	return -1;
}

void RDC_PluginClose(PluginApi* api)
{
	if (NULL != api->DllHandle)
		FreeLibrary(api->DllHandle);

	memset(api, 0, sizeof(PluginApi));
}

