
#pragma once

#ifndef _RDC_PLUGIN_DEFS_H_
#define _RDC_PLUGIN_DEFS_H_


#ifdef __cplusplus
extern "C" {
#endif

#ifdef RDC_PLUGIN_EXPORTS
#define RDC_PLUGIN_API __declspec(dllexport)
#else
#define RDC_PLUGIN_API __declspec(dllimport)
#endif

	RDC_PLUGIN_API const char* GetName();
	RDC_PLUGIN_API long CreateHandle(char *config);
	RDC_PLUGIN_API int Destory(long Handle);
	RDC_PLUGIN_API int Start(long Handle);
	RDC_PLUGIN_API int Stop(long Handle);

#ifdef __cplusplus
}
#endif

#endif //_RDC_PLUGIN_DEFS_H_
