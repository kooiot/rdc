// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// StreamServer.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"
#include <string>
#include <cstring>
#ifndef RDC_LINUX_SYS
#include <Windows.h>
#endif

// TODO: �� STDAFX.H �������κ�����ĸ���ͷ�ļ���
//�������ڴ��ļ�������

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
