
#pragma once

#ifndef _RDC_PLUGIN_API_H_
#define _RDC_PLUGIN_API_H_

#ifndef RDC_LINUX_SYS
#include "Windows.h"
#define RDC_DLL_HANDLE HMODULE
#else
#inclue <dlopen.h>
#define RDC_DLL_HANDLE int
#endif

#ifdef __cplusplus
extern "C" {
#endif

	typedef const char* (__cdecl *FGetName)();
	typedef long (__cdecl *FCreateHandle)(char *config);
	typedef int (__cdecl *FDestory)(long Handle);
	typedef int (__cdecl *FStart)(long Handle);
	typedef int (__cdecl *FStop)(long Handle);

	typedef struct _PluginApi {
		RDC_DLL_HANDLE DllHandle;
		FGetName GetName;
		FCreateHandle CreateHandle;
		FDestory Destory;
		FStart Start;
		FStop Stop;
	} PluginApi;

	// LoadLibrary()
	int RDC_PluginLoad(const char* dll, PluginApi* api);
	// FreeLibrary()
	void RDC_PluginClose(PluginApi* api);

#ifdef __cplusplus
}
#endif

#endif 
