
#pragma once

#ifndef _PLUG_IN_API_H_
#define _PLUG_IN_API_H_


#ifdef __cplusplus
extern "C" {
#endif

#ifdef PLUG_IN_EXPORTS
#define PLUG_IN_API __declspec(dllexport)
#else
#define PLUG_IN_API __declspec(dllimport)
#endif

typedef void* RDCP_HANDLE;

	PLUG_IN_API const char* GetName();
	PLUG_IN_API RDCP_HANDLE CreateHandle(char *config);
	PLUG_IN_API int Destory(RDCP_HANDLE Handle);
	PLUG_IN_API int Start(RDCP_HANDLE Handle);
	PLUG_IN_API int Stop(RDCP_HANDLE Handle);

#ifdef __cplusplus
}
#endif

#endif 
