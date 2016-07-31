// stdafx.cpp : 只包括标准包含文件的源文件
// StreamServer.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
#include <string>
#include <cstring>
#ifndef RDC_LINUX_SYS
#include <Windows.h>
#endif

// TODO: 在 STDAFX.H 中引用任何所需的附加头文件，
//而不是在此文件中引用

#ifdef RDC_LINUX_SYS
#define MAX_PATH 512
int GetModuleFileName(char* sModuleName, char* sFileName, int nSize)
{
	int ret = 0;
	char* p = getenv("_");
	if (p != NULL)
	{
		if (sModuleName != NULL)
			strstr(p, sModuleName);
		if (p != NULL)
			strcpy(sFileName, p);
		ret = strlen(sFileName);
	}
	return ret;
}
#endif

std::string GetModuleFilePath()
{
	char szFile[MAX_PATH] = { 0 };
	int dwRet = ::GetModuleFileName(NULL, szFile, 255);
	if (dwRet != 0)
	{
		printf("Module File Name: %s \n", szFile);
		std::string str = szFile;
#ifndef RDC_LINUX_SYS
		size_t nPos = str.rfind('\\');
#else
		size_t nPos = str.rfind('/');
#endif
		if (nPos != std::string::npos)
		{
			str = str.substr(0, nPos);
		}
		return str;
	}
	return "";
}
