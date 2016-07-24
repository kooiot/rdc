
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
	// 插件类型
	enum RDC_PLUGIN_TYPE {
		RPT_MAPPER = 0,
		RPT_CLIENT = 1,
	};
	// 插件向外发送数据的接口
	typedef int(__stdcall *PluginSendCB)(const char* buf, size_t len, void* ptr);
	// 插件触发关闭
	typedef int(__stdcall *PluginCloseCB)(void* ptr);

	// 获取当前插件类型（为了防止放置错误类型的插件）
	RDC_PLUGIN_API RDC_PLUGIN_TYPE GetType();
	// 获取插件名称（必须保证唯一性）限制长度32
	RDC_PLUGIN_API const char* GetName();
	// 创建插件实例
	RDC_PLUGIN_API long CreateHandle(char *config,
		PluginSendCB send,
		PluginCloseCB close,
		void* ptr);

	// 销毁插件实例
	RDC_PLUGIN_API int Destory(long Handle);
	// 开启插件插件实例
	RDC_PLUGIN_API int Open(long Handle);
	// 停止插件插件实例
	RDC_PLUGIN_API int Close(long Handle);
	// 往插件写入数据
	RDC_PLUGIN_API int Write(long Handle, const char* buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif //_RDC_PLUGIN_DEFS_H_
