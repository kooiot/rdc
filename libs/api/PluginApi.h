
#pragma once

#ifndef _RDC_PLUGIN_API_H_
#define _RDC_PLUGIN_API_H_

#ifndef RDC_LINUX_SYS
#include "Windows.h"
#define RDC_DLL_HANDLE HMODULE
#else
#include <dlfcn.h>
#define RDC_DLL_HANDLE int
#define __stdcall
#endif

#ifdef __cplusplus
extern "C" {
#endif
	enum RDC_PLUGIN_TYPE {
		RPT_MAPPER = 0x01,
		RPT_CLIENT = 0x02,
	};

	typedef int (__stdcall * PluginSendCB)(const char* buf, size_t len, void* ptr);
	typedef int (__stdcall * PluginCloseCB)(void* ptr);

	typedef RDC_PLUGIN_TYPE(__stdcall * FGetType)();
	typedef const char* (__stdcall * FGetName)();
	typedef long (__stdcall * FCreateHandle)(char *config,
		PluginSendCB send,
		PluginCloseCB close,
		void* ptr);
	typedef int (__stdcall * FDestory)(long Handle);
	typedef int (__stdcall * FOpen)(long Handle);
	typedef int (__stdcall * FClose)(long Handle);
	typedef int (__stdcall * FWrite)(long Handle, const char* buf, size_t len);

	typedef struct _PluginApi {
#ifndef RDC_LINUX_SYS
		RDC_DLL_HANDLE DllHandle;
#else
		void* DllHandle;
#endif
		FGetType GetType;
		FGetName GetName;
		FCreateHandle CreateHandle;
		FDestory Destory;
		FOpen Open;
		FClose Close;
		FWrite Write;
	} PluginApi;

	// LoadLibrary()
	int RDC_PluginLoad(const char* dll, PluginApi* api);
	// FreeLibrary()
	void RDC_PluginClose(PluginApi* api);

#ifdef __cplusplus
}
#endif

#endif 
