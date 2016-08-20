
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
	typedef void* PLUGIN_HANDLE;
	enum RDC_PLUGIN_TYPE {
		RPT_MAPPER = 0x01,
		RPT_CLIENT = 0x02,
	};

	typedef int (__stdcall * PluginSendCB)(const char* buf, size_t len, void* ptr);
	typedef int (__stdcall * PluginCloseCB)(void* ptr);

	typedef RDC_PLUGIN_TYPE(* FGetType)();
	typedef const char* (* FGetName)();
	typedef PLUGIN_HANDLE(* FCreateHandle)(char *config,
		PluginSendCB send,
		PluginCloseCB close,
		void* ptr);
	typedef int (* FDestory)(PLUGIN_HANDLE Handle);
	typedef int (* FOpen)(PLUGIN_HANDLE Handle);
	typedef int (* FClose)(PLUGIN_HANDLE Handle);
	typedef int (* FWrite)(PLUGIN_HANDLE Handle, const char* buf, size_t len);
	typedef const char*(*FGetInfo)();

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
		FGetInfo GetInfo;
	} PluginApi;

	// LoadLibrary()
	int RDC_PluginLoad(const char* dll, PluginApi* api);
	// FreeLibrary()
	void RDC_PluginClose(PluginApi* api);

#ifdef __cplusplus
}
#endif

#endif 
