/*******************************************************************************
//
//	ftvspc.c
//
//	Virtual Serial Port Control
//
//	Implementaion of loading and unloading ftvspc.dll for dynamic linking
//
//	Copyright (c) FabulaTech
//
*******************************************************************************/
#ifdef _MFC_VER
#include "afxwin.h"
#else
#include "windows.h"
#endif
#include "ftvspc.h"

static HMODULE g_hVspcApi = NULL;

FTVSPCAPIINITA FtVspcApiInitA = NULL;
FTVSPCAPIINITW FtVspcApiInitW = NULL;
FTVSPAPPLYKEYA FtVspcApplyKeyA = NULL;
FTVSPAPPLYKEYW FtVspcApplyKeyW = NULL;
FTVSPCAPICLOSE FtVspcApiClose;
FTVSPCCREATEPORTBYNUM FtVspcCreatePortByNum;
FTVSPCCREATEPORTA FtVspcCreatePortA = NULL;
FTVSPCCREATEPORTW FtVspcCreatePortW = NULL;
FTVSPCCREATEPORTOVERLAPPEDBYNUMA FtVspcCreatePortOverlappedByNumA = NULL;
FTVSPCCREATEPORTOVERLAPPEDBYNUMW FtVspcCreatePortOverlappedByNumW = NULL;
FTVSPCCREATEPORTOVERLAPPEDA FtVspcCreatePortOverlappedA = NULL;
FTVSPCCREATEPORTOVERLAPPEDW FtVspcCreatePortOverlappedW = NULL;
FTVSPCREMOVEPORTBYNUM FtVspcRemovePortByNum;
FTVSPCREMOVEPORTA FtVspcRemovePortA = NULL;
FTVSPCREMOVEPORTW FtVspcRemovePortW = NULL;
FTVSPCGETINFOA FtVspcGetInfoA = NULL;
FTVSPCGETINFOW FtVspcGetInfoW = NULL;
FTVSPCGETLICENSEINFOA FtVspcGetLicenseInfoA = NULL;
FTVSPCGETLICENSEINFOW FtVspcGetLicenseInfoW = NULL;
FTVSPCENUMPHYSICAL FtVspcEnumPhysical;
FTVSPCENUMVIRTUAL FtVspcEnumVirtual;
FTVSPCSETPERMANENTBYNUM FtVspcSetPermanentByNum;
FTVSPCSETPERMANENTW FtVspcSetPermanentW = NULL;
FTVSPCSETPERMANENTA FtVspcSetPermanentA = NULL;
FTVSPCGETPERMANENTBYNUM FtVspcGetPermanentByNum;
FTVSPCGETPERMANENTW FtVspcGetPermanentW = NULL;
FTVSPCGETPERMANENTA FtVspcGetPermanentA = NULL;
FTVSPCSETQUERYOPENBYNUM FtVspcSetQueryOpenByNum;
FTVSPCSETQUERYOPENW FtVspcSetQueryOpenW = NULL;
FTVSPCSETQUERYOPENA FtVspcSetQueryOpenA = NULL;
FTVSPCGETQUERYOPENBYNUM FtVspcGetQueryOpenByNum;
FTVSPCGETQUERYOPENW FtVspcGetQueryOpenW = NULL;
FTVSPCGETQUERYOPENA FtVspcGetQueryOpenA = NULL;
FTVSPCATTACHBYNUM FtVspcAttachByNum;
FTVSPCATTACHA FtVspcAttachA = NULL;
FTVSPCATTACHW FtVspcAttachW = NULL;
FTVSPCWRITE FtVspcWrite;
FTVSPCDETACH FtVspcDetach;
FTVSPCSETCTS FtVspcSetCts;
FTVSPCGETCTS FtVspcGetCts;
FTVSPCSETDCD FtVspcSetDcd;
FTVSPCGETDCD FtVspcGetDcd;
FTVSPCSETDSR FtVspcSetDsr;
FTVSPCGETDSR FtVspcGetDsr;
FTVSPCSETRING FtVspcSetRing;
FTVSPCGETRING FtVspcGetRing;
FTVSPCSETBITRATEEMULATION FtVspcSetBitrateEmulation;
FTVSPCGETBITRATEEMULATION FtVspcGetBitrateEmulation;
FTVSPCGETINQUEUEBYTES FtVspcGetInQueueBytes;
FTVSPCREAD FtVspcRead;
FTVSPCSETBREAK FtVspcSetBreak;
FTVSPCGETBREAK FtVspcGetBreak;
FTVSPCSETPARITY FtVspcSetParity;
FTVSPCGETPARITY FtVspcGetParity;
FTVSPCSETOVERRUN FtVspcSetOverrun;
FTVSPCGETOVERRUN FtVspcGetOverrun;
FTVSPCSETFRAMING FtVspcSetFraming;
FTVSPCGETFRAMING FtVspcGetFraming;
FTVSPCGETPHYSICALW FtVspcGetPhysicalW = NULL;
FTVSPCGETPHYSICALA FtVspcGetPhysicalA = NULL;
FTVSPCGETPHYSICALNUM FtVspcGetPhysicalNum;
FTVSPCGETVIRTUALNUM FtVspcGetVirtualNum;
FTVSPCGETVIRTUALA FtVspcGetVirtualA = NULL;
FTVSPCGETVIRTUALW FtVspcGetVirtualW = NULL;
FTVSPCGETLASTERROR FtVspcGetLastError;
FTVSPCGETERRORMESSAGEW FtVspcGetErrorMessageW = NULL;
FTVSPCGETERRORMESSAGEA FtVspcGetErrorMessageA = NULL;


BOOL
FtVspcDllInit()
{
	DWORD dwErr;

	if (NULL != g_hVspcApi)
		return TRUE;

	g_hVspcApi = LoadLibraryW(L"ftvspc.dll");

	if (NULL == g_hVspcApi)
		return FALSE;

	do
	{
		FtVspcApiInitW = (FTVSPCAPIINITW)GetProcAddress(
			g_hVspcApi, "FtVspcApiInitW");

		if (NULL == FtVspcApiInitW)
			break;

		FtVspcApiInitA = (FTVSPCAPIINITA)GetProcAddress(
			g_hVspcApi, "FtVspcApiInitA");

		if (NULL == FtVspcApiInitA)
			break;

		FtVspcApiClose = (FTVSPCAPICLOSE)GetProcAddress(
			g_hVspcApi, "FtVspcApiClose");

		if (NULL == FtVspcApiClose)
			break;

		FtVspcCreatePortByNum = (FTVSPCCREATEPORTBYNUM)GetProcAddress(
			g_hVspcApi, "FtVspcCreatePortByNum");
		if (NULL == FtVspcCreatePortByNum)
			break;

		FtVspcCreatePortA = (FTVSPCCREATEPORTA)GetProcAddress(
			g_hVspcApi, "FtVspcCreatePortA");

		if (NULL == FtVspcCreatePortA)
			break;

		FtVspcCreatePortW = (FTVSPCCREATEPORTW)GetProcAddress(
			g_hVspcApi, "FtVspcCreatePortW");

		if (NULL == FtVspcCreatePortW)
			break;

		FtVspcCreatePortOverlappedByNumA = (FTVSPCCREATEPORTOVERLAPPEDBYNUMA)GetProcAddress(
			g_hVspcApi, "FtVspcCreatePortOverlappedByNumA");

		if (NULL == FtVspcCreatePortOverlappedByNumA)
			break;

		FtVspcCreatePortOverlappedByNumW = (FTVSPCCREATEPORTOVERLAPPEDBYNUMW)GetProcAddress(
			g_hVspcApi, "FtVspcCreatePortOverlappedByNumW");

		if (NULL == FtVspcCreatePortOverlappedByNumW)
			break;

		FtVspcCreatePortOverlappedA = (FTVSPCCREATEPORTOVERLAPPEDA)GetProcAddress(
			g_hVspcApi, "FtVspcCreatePortOverlappedA");

		if (NULL == FtVspcCreatePortOverlappedByNumA)
			break;

		FtVspcCreatePortOverlappedW = (FTVSPCCREATEPORTOVERLAPPEDW)GetProcAddress(
			g_hVspcApi, "FtVspcCreatePortOverlappedW");

		if (NULL == FtVspcCreatePortOverlappedByNumW)
			break;

		FtVspcRemovePortByNum = (FTVSPCREMOVEPORTBYNUM)GetProcAddress(
			g_hVspcApi, "FtVspcRemovePortByNum");

		if (NULL == FtVspcRemovePortByNum)
			break;

		FtVspcRemovePortA = (FTVSPCREMOVEPORTA)GetProcAddress(
			g_hVspcApi, "FtVspcRemovePortA");

		if (NULL == FtVspcRemovePortA)
			break;

		FtVspcRemovePortW = (FTVSPCREMOVEPORTW)GetProcAddress(
			g_hVspcApi, "FtVspcRemovePortW");

		if (NULL == FtVspcRemovePortW)
			break;

		FtVspcGetInfoW = (FTVSPCGETINFOW)GetProcAddress(
			g_hVspcApi, "FtVspcGetInfoW");

		if (NULL == FtVspcGetInfoW)
			break;

		FtVspcGetInfoA = (FTVSPCGETINFOA)GetProcAddress(
			g_hVspcApi, "FtVspcGetInfoA");

		if (NULL == FtVspcGetInfoA)
			break;

		FtVspcEnumPhysical = (FTVSPCENUMPHYSICAL)GetProcAddress(
			g_hVspcApi, "FtVspcEnumPhysical");

		if (NULL == FtVspcEnumPhysical)
			break;

		FtVspcEnumVirtual = (FTVSPCENUMVIRTUAL)GetProcAddress(
			g_hVspcApi, "FtVspcEnumVirtual");

		if (NULL == FtVspcEnumVirtual)
			break;

		FtVspcSetPermanentByNum = (FTVSPCSETPERMANENTBYNUM)GetProcAddress(
			g_hVspcApi, "FtVspcSetPermanentByNum");

		if (NULL == FtVspcSetPermanentByNum)
			break;

		FtVspcSetPermanentA = (FTVSPCSETPERMANENTA)GetProcAddress(
			g_hVspcApi, "FtVspcSetPermanentA");

		if (NULL == FtVspcSetPermanentA)
			break;

		FtVspcSetPermanentW = (FTVSPCSETPERMANENTW)GetProcAddress(
			g_hVspcApi, "FtVspcSetPermanentW");

		if (NULL == FtVspcSetPermanentW)
			break;

		FtVspcGetPermanentByNum = (FTVSPCGETPERMANENTBYNUM)GetProcAddress(
			g_hVspcApi, "FtVspcGetPermanentByNum");

		if (NULL == FtVspcGetPermanentByNum)
			break;

		FtVspcGetPermanentA = (FTVSPCGETPERMANENTA)GetProcAddress(
			g_hVspcApi, "FtVspcGetPermanentA");

		if (NULL == FtVspcGetPermanentA)
			break;

		FtVspcGetPermanentW = (FTVSPCGETPERMANENTW)GetProcAddress(
			g_hVspcApi, "FtVspcGetPermanentW");

		if (NULL == FtVspcGetPermanentW)
			break;

		FtVspcSetQueryOpenByNum = (FTVSPCSETQUERYOPENBYNUM)GetProcAddress(
			g_hVspcApi, "FtVspcSetQueryOpenByNum");

		if (NULL == FtVspcSetQueryOpenByNum)
			break;

		FtVspcSetQueryOpenA = (FTVSPCSETQUERYOPENA)GetProcAddress(
			g_hVspcApi, "FtVspcSetQueryOpenA");

		if (NULL == FtVspcSetQueryOpenA)
			break;

		FtVspcSetQueryOpenW = (FTVSPCSETQUERYOPENW)GetProcAddress(
			g_hVspcApi, "FtVspcSetQueryOpenW");

		if (NULL == FtVspcSetQueryOpenW)
			break;

		FtVspcGetQueryOpenByNum = (FTVSPCGETQUERYOPENBYNUM)GetProcAddress(
			g_hVspcApi, "FtVspcGetQueryOpenByNum");

		if (NULL == FtVspcGetQueryOpenByNum)
			break;

		FtVspcGetQueryOpenA = (FTVSPCGETQUERYOPENA)GetProcAddress(
			g_hVspcApi, "FtVspcGetQueryOpenA");

		if (NULL == FtVspcGetQueryOpenA)
			break;

		FtVspcGetQueryOpenW = (FTVSPCGETQUERYOPENW)GetProcAddress(
			g_hVspcApi, "FtVspcGetQueryOpenW");

		if (NULL == FtVspcGetQueryOpenW)
			break;

		FtVspcAttachByNum = (FTVSPCATTACHBYNUM)GetProcAddress(
			g_hVspcApi, "FtVspcAttachByNum");

		if (NULL == FtVspcAttachByNum)
			break;

		FtVspcAttachA = (FTVSPCATTACHA)GetProcAddress(
			g_hVspcApi, "FtVspcAttachA");

		if (NULL == FtVspcAttachA)
			break;

		FtVspcAttachW = (FTVSPCATTACHW)GetProcAddress(
			g_hVspcApi, "FtVspcAttachW");

		if (NULL == FtVspcAttachA)
			break;

		FtVspcWrite = (FTVSPCWRITE)GetProcAddress(
			g_hVspcApi, "FtVspcWrite");

		if (NULL == FtVspcWrite)
			break;

		FtVspcDetach = (FTVSPCDETACH)GetProcAddress(
			g_hVspcApi, "FtVspcDetach");

		if (NULL == FtVspcDetach)
			break;

		FtVspcSetCts = (FTVSPCSETCTS)GetProcAddress(
			g_hVspcApi, "FtVspcSetCts");

		if (NULL == FtVspcSetCts)
			break;

		FtVspcGetCts = (FTVSPCGETCTS)GetProcAddress(
			g_hVspcApi, "FtVspcGetCts");

		if (NULL == FtVspcSetCts)
			break;

		FtVspcSetDcd = (FTVSPCSETDCD)GetProcAddress(
			g_hVspcApi, "FtVspcSetDcd");

		if (NULL == FtVspcSetDcd)
			break;

		FtVspcGetDcd = (FTVSPCGETDCD)GetProcAddress(
			g_hVspcApi, "FtVspcGetDcd");

		if (NULL == FtVspcGetDcd)
			break;

		FtVspcSetDsr = (FTVSPCSETDSR)GetProcAddress(
			g_hVspcApi, "FtVspcSetDsr");

		if (NULL == FtVspcSetDsr)
			break;

		FtVspcGetDsr = (FTVSPCGETDSR)GetProcAddress(
			g_hVspcApi, "FtVspcGetDsr");

		if (NULL == FtVspcGetDsr)
			break;

		FtVspcSetRing = (FTVSPCSETRING)GetProcAddress(
			g_hVspcApi, "FtVspcSetRing");

		if (NULL == FtVspcSetRing)
			break;

		FtVspcGetRing = (FTVSPCGETRING)GetProcAddress(
			g_hVspcApi, "FtVspcGetRing");

		if (NULL == FtVspcGetRing)
			break;

		FtVspcSetBitrateEmulation = (FTVSPCSETBITRATEEMULATION)GetProcAddress(
			g_hVspcApi, "FtVspcSetBitrateEmulation");

		if (NULL == FtVspcSetBitrateEmulation)
			break;

		FtVspcGetBitrateEmulation = (FTVSPCGETBITRATEEMULATION)GetProcAddress(
			g_hVspcApi, "FtVspcGetBitrateEmulation");

		if (NULL == FtVspcGetBitrateEmulation)
			break;

		FtVspcGetInQueueBytes = (FTVSPCGETINQUEUEBYTES)GetProcAddress(
			g_hVspcApi, "FtVspcGetInQueueBytes");

		if (NULL == FtVspcGetInQueueBytes)
			break;

		FtVspcRead = (FTVSPCREAD)GetProcAddress(
			g_hVspcApi, "FtVspcRead");

		if (NULL == FtVspcRead)
			break;

		FtVspcSetBreak = (FTVSPCSETBREAK)GetProcAddress(
			g_hVspcApi, "FtVspcSetBreak");

		if (NULL == FtVspcSetBreak)
			break;

		FtVspcGetBreak = (FTVSPCGETBREAK)GetProcAddress(
			g_hVspcApi, "FtVspcGetBreak");

		if (NULL == FtVspcGetBreak)
			break;

		FtVspcSetParity = (FTVSPCSETPARITY)GetProcAddress(
			g_hVspcApi, "FtVspcSetParity");

		if (NULL == FtVspcSetParity)
			break;

		FtVspcGetParity = (FTVSPCGETPARITY)GetProcAddress(
			g_hVspcApi, "FtVspcGetParity");

		if (NULL == FtVspcGetParity)
			break;

		FtVspcSetOverrun = (FTVSPCSETOVERRUN)GetProcAddress(
			g_hVspcApi, "FtVspcSetOverrun");

		if (NULL == FtVspcSetOverrun)
			break;

		FtVspcGetOverrun = (FTVSPCGETOVERRUN)GetProcAddress(
			g_hVspcApi, "FtVspcGetOverrun");

		if (NULL == FtVspcGetOverrun)
			break;

		FtVspcSetFraming = (FTVSPCSETFRAMING)GetProcAddress(
			g_hVspcApi, "FtVspcSetFraming");

		if (NULL == FtVspcSetFraming)
			break;

		FtVspcGetFraming = (FTVSPCGETFRAMING)GetProcAddress(
			g_hVspcApi, "FtVspcGetFraming");

		if (NULL == FtVspcGetFraming)
			break;

		FtVspcGetPhysicalW = (FTVSPCGETPHYSICALW)GetProcAddress(
			g_hVspcApi, "FtVspcGetPhysicalW");

		if (NULL == FtVspcGetPhysicalW)
			break;

		FtVspcGetPhysicalA = (FTVSPCGETPHYSICALA)GetProcAddress(
			g_hVspcApi, "FtVspcGetPhysicalA");

		if (NULL == FtVspcGetPhysicalA)
			break;

		FtVspcGetPhysicalNum = (FTVSPCGETPHYSICALNUM)GetProcAddress(
			g_hVspcApi, "FtVspcGetPhysicalNum");

		if (NULL == FtVspcGetPhysical)
			break;

		FtVspcGetVirtualNum = (FTVSPCGETVIRTUALNUM)GetProcAddress(
			g_hVspcApi, "FtVspcGetVirtualNum");

		if (NULL == FtVspcGetVirtualNum)
			break;

		FtVspcGetVirtualA = (FTVSPCGETVIRTUALA)GetProcAddress(
			g_hVspcApi, "FtVspcGetVirtualA");

		if (NULL == FtVspcGetVirtualA)
			break;

		FtVspcGetVirtualW = (FTVSPCGETVIRTUALW)GetProcAddress(
			g_hVspcApi, "FtVspcGetVirtualW");

		if (NULL == FtVspcGetVirtualW)
			break;

		FtVspcGetLastError = (FTVSPCGETLASTERROR)GetProcAddress(
			g_hVspcApi, "FtVspcGetLastError");

		if (NULL == FtVspcGetLastError)
			break;

		FtVspcGetErrorMessageA = (FTVSPCGETERRORMESSAGEA)GetProcAddress(
			g_hVspcApi, "FtVspcGetErrorMessageA");

		if (NULL == FtVspcGetErrorMessageA)
			break;

		FtVspcGetErrorMessageW = (FTVSPCGETERRORMESSAGEW)GetProcAddress(
			g_hVspcApi, "FtVspcGetErrorMessageW");

		if (NULL == FtVspcGetErrorMessageW)
			break;

		return TRUE;

	} while(0);

	////
	dwErr = GetLastError();

	// close dll when any error occured
	FtVspcDllClose();

	SetLastError(dwErr);

	return FALSE;
}

void
FtVspcDllClose(void)
{
	FtVspcApiInitW = NULL;
	FtVspcApiInitA = NULL;
	FtVspcApiClose = NULL;
	FtVspcCreatePortByNum = NULL;
	FtVspcCreatePortA = NULL;
	FtVspcCreatePortW = NULL;
	FtVspcCreatePortOverlappedByNumA = NULL;
	FtVspcCreatePortOverlappedByNumW = NULL;
	FtVspcCreatePortOverlappedA = NULL;
	FtVspcCreatePortOverlappedW = NULL;
	FtVspcRemovePortByNum = NULL;
	FtVspcGetInfoW = NULL;
	FtVspcGetInfoA = NULL;
	FtVspcEnumPhysical = NULL;
	FtVspcEnumVirtual = NULL;
	FtVspcSetPermanentByNum = NULL;
	FtVspcSetPermanentA = NULL;
	FtVspcSetPermanentW = NULL;
	FtVspcGetPermanentByNum = NULL;
	FtVspcGetPermanentA = NULL;
	FtVspcGetPermanentW = NULL;
	FtVspcSetQueryOpenByNum = NULL;
	FtVspcSetQueryOpenA = NULL;
	FtVspcSetQueryOpenW = NULL;
	FtVspcGetQueryOpenByNum = NULL;
	FtVspcGetQueryOpenA = NULL;
	FtVspcGetQueryOpenW = NULL;
	FtVspcAttachA = NULL;
	FtVspcAttachW = NULL;
	FtVspcAttachByNum = NULL;
	FtVspcWrite = NULL;
	FtVspcDetach = NULL;
	FtVspcSetCts = NULL;
	FtVspcGetCts = NULL;
	FtVspcSetDcd = NULL;
	FtVspcGetDcd = NULL;
	FtVspcSetDsr = NULL;
	FtVspcGetDsr = NULL;
	FtVspcSetRing = NULL;
	FtVspcGetRing = NULL;
	FtVspcSetBitrateEmulation = NULL;
	FtVspcGetBitrateEmulation = NULL;
	FtVspcGetInQueueBytes = NULL;
	FtVspcRead = NULL;
	FtVspcSetBreak = NULL;
	FtVspcGetBreak = NULL;
	FtVspcSetParity = NULL;
	FtVspcGetParity = NULL;
	FtVspcSetOverrun = NULL;
	FtVspcGetOverrun = NULL;
	FtVspcSetFraming = NULL;
	FtVspcGetFraming = NULL;
	FtVspcGetPhysicalW = NULL;
	FtVspcGetPhysicalA = NULL;
	FtVspcGetPhysicalNum = NULL;
	FtVspcGetVirtualNum = NULL;
	FtVspcGetVirtualA = NULL;
	FtVspcGetVirtualW = NULL;
	FtVspcGetLastError = NULL;
	FtVspcGetErrorMessageA = NULL;
	FtVspcGetErrorMessageW = NULL;

	if (NULL != g_hVspcApi)
		FreeLibrary(g_hVspcApi);

	g_hVspcApi = NULL;
}
