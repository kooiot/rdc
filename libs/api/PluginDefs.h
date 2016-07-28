
#pragma once

#ifndef _RDC_PLUGIN_DEFS_H_
#define _RDC_PLUGIN_DEFS_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RDC_LINUX_SYS
#ifdef RDC_PLUGIN_EXPORTS
#define RDC_PLUGIN_API __declspec(dllexport)
#else
#define RDC_PLUGIN_API __declspec(dllimport)
#endif
#else
#define RDC_PLUGIN_API
#define __stdcall
#endif

#define RDC_MAX_PLUGIN_CONFIG 512

	// �������
	enum RDC_PLUGIN_TYPE {
		RPT_MAPPER = 0x01,
		RPT_CLIENT = 0x02,
	};

	// ������ⷢ�����ݵĽӿ�
	typedef int(__stdcall *PluginSendCB)(const char* buf, size_t len, void* ptr);
	// ��������ر�
	typedef int(__stdcall *PluginCloseCB)(void* ptr);
	
	// ��ȡ��ǰ������ͣ�Ϊ�˷�ֹ���ô������͵Ĳ����
	RDC_PLUGIN_API RDC_PLUGIN_TYPE GetType();
	// ��ȡ������ƣ����뱣֤Ψһ�ԣ����Ƴ���32
	RDC_PLUGIN_API const char* GetName();
	// �������ʵ��
	RDC_PLUGIN_API long CreateHandle(char *config,
		PluginSendCB send,
		PluginCloseCB close,
		void* ptr);

	// ���ٲ��ʵ��
	RDC_PLUGIN_API int Destory(long Handle);
	// ����������ʵ��
	RDC_PLUGIN_API int Open(long Handle);
	// ֹͣ������ʵ��
	RDC_PLUGIN_API int Close(long Handle);
	// �����д������
	RDC_PLUGIN_API int Write(long Handle, const char* buf, size_t len);
	// ��ȡ�����ϸ��ϢJSON(V2)
	RDC_PLUGIN_API const char* GetInfo();
#ifdef __cplusplus
}
#endif

#endif //_RDC_PLUGIN_DEFS_H_
