/*******************************************************************************
//
//	ftvspc.h
//
//	Virtual Serial Port Control
//
//	ftvspc.dll header file for dynamic linking
//
//	Copyright (c) FabulaTech
//
*******************************************************************************/
#pragma once

#include "ftvspcstructs.h"

#ifdef __cplusplus
extern "C"
{
#endif

// LoadLibrary()
BOOL FtVspcDllInit();

// FreeLibrary()
void FtVspcDllClose();

/*
 *	Initializes developer API.
 */
typedef BOOL (__cdecl *FTVSPCAPIINITA)(
	LPFNVSPCEVENTS,
	LONG_PTR,
	LPCSTR
	);
extern FTVSPCAPIINITA FtVspcApiInitA;

typedef BOOL (__cdecl *FTVSPCAPIINITW)(
	LPFNVSPCEVENTS,
	LONG_PTR,
	LPCWSTR
	);
extern FTVSPCAPIINITW FtVspcApiInitW;

#ifdef UNICODE
#define FtVspcApiInit FtVspcApiInitW
#else
#define FtVspcApiInit FtVspcApiInitA
#endif // !UNICODE

typedef BOOL (__cdecl *FTVSPAPPLYKEYA)(LPCSTR);
extern FTVSPAPPLYKEYA FtVspcApplyKeyA;

typedef BOOL (__cdecl *FTVSPAPPLYKEYW)(LPCWSTR);
extern FTVSPAPPLYKEYW FtVspcApplyKeyW;

#ifdef UNICODE
#define FtVspcApplyKey FtVspcApplyKeyW
#else
#define FtVspcApplyKey FtVspcApplyKeyA
#endif // !UNICODE

/*
*	Frees all resources allocated earlier.
*/
typedef void (__cdecl *FTVSPCAPICLOSE)(void);
extern FTVSPCAPICLOSE FtVspcApiClose;

typedef BOOL (__cdecl *FTVSPCCREATEPORTBYNUM)(UINT);
extern FTVSPCCREATEPORTBYNUM FtVspcCreatePortByNum;

typedef BOOL (__cdecl *FTVSPCCREATEPORTA)(LPCSTR);
extern FTVSPCCREATEPORTA FtVspcCreatePortA;

typedef BOOL (__cdecl *FTVSPCCREATEPORTW)(LPCWSTR);
extern FTVSPCCREATEPORTW FtVspcCreatePortW;

#ifdef UNICODE
#define FtVspcCreatePort FtVspcCreatePortW
#else
#define FtVspcCreatePort FtVspcCreatePortA
#endif // !UNICODE

typedef BOOL (__cdecl *FTVSPCCREATEPORTOVERLAPPEDBYNUMA)(UINT, LPCSTR);
extern FTVSPCCREATEPORTOVERLAPPEDBYNUMA FtVspcCreatePortOverlappedByNumA;

typedef BOOL (__cdecl *FTVSPCCREATEPORTOVERLAPPEDBYNUMW)(UINT, LPCWSTR);
extern FTVSPCCREATEPORTOVERLAPPEDBYNUMW FtVspcCreatePortOverlappedByNumW;

#ifdef UNICODE
#define FtVspcCreatePortOverlappedByNum FtVspcCreatePortOverlappedByNumW
#else
#define FtVspcCreatePortOverlappedByNum FtVspcCreatePortOverlappedByNumA
#endif // !UNICODE

typedef BOOL (__cdecl *FTVSPCCREATEPORTOVERLAPPEDA)(LPCSTR, LPCSTR);
extern FTVSPCCREATEPORTOVERLAPPEDA FtVspcCreatePortOverlappedA;

typedef BOOL (__cdecl *FTVSPCCREATEPORTOVERLAPPEDW)(LPCWSTR, LPCWSTR);
extern FTVSPCCREATEPORTOVERLAPPEDW FtVspcCreatePortOverlappedW;

#ifdef UNICODE
#define FtVspcCreatePortOverlapped FtVspcCreatePortOverlappedW
#else
#define FtVspcCreatePortOverlapped FtVspcCreatePortOverlappedA
#endif // !UNICODE

typedef BOOL (__cdecl *FTVSPCREMOVEPORTBYNUM)(UINT);
extern FTVSPCREMOVEPORTBYNUM FtVspcRemovePortByNum;

typedef BOOL (__cdecl *FTVSPCREMOVEPORTA)(LPCSTR);
extern FTVSPCREMOVEPORTA FtVspcRemovePortA;

typedef BOOL (__cdecl *FTVSPCREMOVEPORTW)(LPCWSTR);
extern FTVSPCREMOVEPORTW FtVspcRemovePortW;

#ifdef UNICODE
#define FtVspcRemovePort FtVspcRemovePortW
#else
#define FtVspcRemovePort FtVspcRemovePortA
#endif // !UNICODE

typedef BOOL (__cdecl *FTVSPCGETINFOA)(FTVSPC_INFOA*);
extern FTVSPCGETINFOA FtVspcGetInfoA;

typedef BOOL (__cdecl *FTVSPCGETINFOW)(FTVSPC_INFOW*);
extern FTVSPCGETINFOW FtVspcGetInfoW;

#ifdef UNICODE
#define FtVspcGetInfo FtVspcGetInfoW
#else
#define FtVspcGetInfo FtVspcGetInfoA
#endif // !UNICODE

typedef BOOL (__cdecl *FTVSPCGETLICENSEINFOA)(LPCSTR, FTVSPC_INFOA*);
extern FTVSPCGETLICENSEINFOA FtVspcGetLicenseInfoA;

typedef BOOL (__cdecl *FTVSPCGETLICENSEINFOW)(LPCWSTR, FTVSPC_INFOW*);
extern FTVSPCGETLICENSEINFOW FtVspcGetLicenseInfoW;

#ifdef UNICODE
#define FtVspcGetLicenseInfo FtVspcGetLicenseInfoW
#else
#define FtVspcGetLicenseInfo FtVspcGetLicenseInfoA
#endif // !UNICODE

typedef BOOL (__cdecl *FTVSPCENUMPHYSICAL)(UINT*);
extern FTVSPCENUMPHYSICAL FtVspcEnumPhysical;

typedef BOOL (__cdecl *FTVSPCENUMVIRTUAL)(UINT*);
extern FTVSPCENUMVIRTUAL FtVspcEnumVirtual;

typedef BOOL (__cdecl *FTVSPCSETPERMANENTBYNUM)(UINT, BOOL);
extern FTVSPCSETPERMANENTBYNUM FtVspcSetPermanentByNum;

typedef BOOL (__cdecl *FTVSPCSETPERMANENTW)(LPCWSTR, BOOL);
extern FTVSPCSETPERMANENTW FtVspcSetPermanentW;

typedef BOOL (__cdecl *FTVSPCSETPERMANENTA)(LPCSTR, BOOL);
extern FTVSPCSETPERMANENTA FtVspcSetPermanentA;

#ifdef UNICODE
#define FtVspcSetPermanent FtVspcSetPermanentW
#else
#define FtVspcSetPermanent FtVspcSetPermanentA
#endif // !UNICODE

typedef BOOL (__cdecl *FTVSPCGETPERMANENTBYNUM)(UINT, BOOL*);
extern FTVSPCGETPERMANENTBYNUM FtVspcGetPermanentByNum;

typedef BOOL (__cdecl *FTVSPCGETPERMANENTW)(LPCWSTR, BOOL*);
extern FTVSPCGETPERMANENTW FtVspcGetPermanentW;

typedef BOOL (__cdecl *FTVSPCGETPERMANENTA)(LPCSTR, BOOL*);
extern FTVSPCGETPERMANENTA FtVspcGetPermanentA;

#ifdef UNICODE
#define FtVspcGetPermanent FtVspcGetPermanentW
#else
#define FtVspcGetPermanent FtVspcGetPermanentA
#endif // !UNICODE

typedef BOOL (__cdecl *FTVSPCSETQUERYOPENBYNUM)(UINT, BOOL);
extern FTVSPCSETQUERYOPENBYNUM FtVspcSetQueryOpenByNum;

typedef BOOL (__cdecl *FTVSPCSETQUERYOPENW)(LPCWSTR, BOOL);
extern FTVSPCSETQUERYOPENW FtVspcSetQueryOpenW;

typedef BOOL (__cdecl *FTVSPCSETQUERYOPENA)(LPCSTR, BOOL);
extern FTVSPCSETQUERYOPENA FtVspcSetQueryOpenA;

#ifdef UNICODE
#define FtVspcSetQueryOpen FtVspcSetQueryOpenW
#else
#define FtVspcSetQueryOpen FtVspcSetQueryOpenA
#endif // !UNICODE

typedef BOOL (__cdecl *FTVSPCGETQUERYOPENBYNUM)(UINT, BOOL*);
extern FTVSPCGETQUERYOPENBYNUM FtVspcGetQueryOpenByNum;

typedef BOOL (__cdecl *FTVSPCGETQUERYOPENW)(LPCWSTR, BOOL*);
extern FTVSPCGETQUERYOPENW FtVspcGetQueryOpenW;

typedef BOOL (__cdecl *FTVSPCGETQUERYOPENA)(LPCSTR, BOOL*);
extern FTVSPCGETQUERYOPENA FtVspcGetQueryOpenA;

#ifdef UNICODE
#define FtVspcGetQueryOpen FtVspcGetQueryOpenW
#else
#define FtVspcGetQueryOpen FtVspcGetQueryOpenA
#endif // !UNICODE

typedef FTVSPCHANDLE (__cdecl *FTVSPCATTACHBYNUM)(UINT, LPFNVSPCPORTEVENTS, LONG_PTR);
extern FTVSPCATTACHBYNUM FtVspcAttachByNum;

typedef FTVSPCHANDLE (__cdecl *FTVSPCATTACHA)(LPCSTR, LPFNVSPCPORTEVENTS, LONG_PTR);
extern FTVSPCATTACHA FtVspcAttachA;

typedef FTVSPCHANDLE (__cdecl *FTVSPCATTACHW)(LPCWSTR, LPFNVSPCPORTEVENTS, LONG_PTR);
extern FTVSPCATTACHW FtVspcAttachW;

#ifdef UNICODE
#define FtVspcAttach FtVspcAttachW
#else
#define FtVspcAttach FtVspcAttachA
#endif // !UNICODE

typedef BOOL (__cdecl *FTVSPCWRITE)(FTVSPCHANDLE, VOID*, size_t);
extern FTVSPCWRITE FtVspcWrite;

typedef BOOL (__cdecl *FTVSPCDETACH)(FTVSPCHANDLE);
extern FTVSPCDETACH FtVspcDetach;

typedef BOOL (__cdecl *FTVSPCSETCTS)(FTVSPCHANDLE, BOOL);
extern FTVSPCSETCTS FtVspcSetCts;

typedef BOOL (__cdecl *FTVSPCGETCTS)(FTVSPCHANDLE, BOOL*);
extern FTVSPCGETCTS FtVspcGetCts;

typedef BOOL (__cdecl *FTVSPCSETDCD)(FTVSPCHANDLE, BOOL);
extern FTVSPCSETDCD FtVspcSetDcd;

typedef BOOL (__cdecl *FTVSPCGETDCD)(FTVSPCHANDLE, BOOL*);
extern FTVSPCGETDCD FtVspcGetDcd;

typedef BOOL (__cdecl *FTVSPCSETDSR)(FTVSPCHANDLE, BOOL);
extern FTVSPCSETDSR FtVspcSetDsr;

typedef BOOL (__cdecl *FTVSPCGETDSR)(FTVSPCHANDLE, BOOL*);
extern FTVSPCGETDSR FtVspcGetDsr;

typedef BOOL (__cdecl *FTVSPCSETRING)(FTVSPCHANDLE, BOOL);
extern FTVSPCSETRING FtVspcSetRing;

typedef BOOL (__cdecl *FTVSPCGETRING)(FTVSPCHANDLE, BOOL*);
extern FTVSPCGETRING FtVspcGetRing;

typedef BOOL (__cdecl *FTVSPCSETBITRATEEMULATION)(FTVSPCHANDLE, BOOL);
extern FTVSPCSETBITRATEEMULATION FtVspcSetBitrateEmulation;

typedef BOOL (__cdecl *FTVSPCGETBITRATEEMULATION)(FTVSPCHANDLE, BOOL*);
extern FTVSPCGETBITRATEEMULATION FtVspcGetBitrateEmulation;

typedef BOOL (__cdecl *FTVSPCGETINQUEUEBYTES)(FTVSPCHANDLE, size_t*);
extern FTVSPCGETINQUEUEBYTES FtVspcGetInQueueBytes;

typedef BOOL (__cdecl *FTVSPCREAD)(FTVSPCHANDLE, LPVOID, size_t, size_t*);
extern FTVSPCREAD FtVspcRead;

typedef BOOL (__cdecl *FTVSPCSETBREAK)(FTVSPCHANDLE, BOOL);
extern FTVSPCSETBREAK FtVspcSetBreak;

typedef BOOL (__cdecl *FTVSPCGETBREAK)(FTVSPCHANDLE, BOOL*);
extern FTVSPCGETBREAK FtVspcGetBreak;

typedef BOOL (__cdecl *FTVSPCSETPARITY)(FTVSPCHANDLE, BOOL);
extern FTVSPCSETPARITY FtVspcSetParity;

typedef BOOL (__cdecl *FTVSPCGETPARITY)(FTVSPCHANDLE, BOOL*);
extern FTVSPCGETPARITY FtVspcGetParity;

typedef BOOL (__cdecl *FTVSPCSETOVERRUN)(FTVSPCHANDLE, BOOL);
extern FTVSPCSETOVERRUN FtVspcSetOverrun;

typedef BOOL (__cdecl *FTVSPCGETOVERRUN)(FTVSPCHANDLE, BOOL*);
extern FTVSPCGETOVERRUN FtVspcGetOverrun;

typedef BOOL (__cdecl *FTVSPCSETFRAMING)(FTVSPCHANDLE, BOOL);
extern FTVSPCSETFRAMING FtVspcSetFraming;

typedef BOOL (__cdecl *FTVSPCGETFRAMING)(FTVSPCHANDLE, BOOL*);
extern FTVSPCGETFRAMING FtVspcGetFraming;

typedef BOOL (__cdecl *FTVSPCGETPHYSICALW)(UINT, LPCWSTR, UINT);
extern FTVSPCGETPHYSICALW FtVspcGetPhysicalW;

typedef BOOL (__cdecl *FTVSPCGETPHYSICALA)(UINT, LPCSTR, UINT);
extern FTVSPCGETPHYSICALA FtVspcGetPhysicalA;

#ifdef UNICODE
#define FtVspcGetPhysical FtVspcGetPhysicalW
#else
#define FtVspcGetPhysical FtVspcGetPhysicalA
#endif // !UNICODE

typedef BOOL (__cdecl *FTVSPCGETPHYSICALNUM)(UINT, UINT*);
extern FTVSPCGETPHYSICALNUM FtVspcGetPhysicalNum;

typedef BOOL (__cdecl *FTVSPCGETVIRTUALNUM)(UINT, UINT*, BOOL*);
extern FTVSPCGETVIRTUALNUM FtVspcGetVirtualNum;

typedef BOOL (__cdecl *FTVSPCGETVIRTUALA)(UINT, LPCSTR, UINT, BOOL*);
extern FTVSPCGETVIRTUALA FtVspcGetVirtualA;

typedef BOOL (__cdecl *FTVSPCGETVIRTUALW)(UINT, LPCWSTR, UINT, BOOL*);
extern FTVSPCGETVIRTUALW FtVspcGetVirtualW;

#ifdef UNICODE
#define FtVspcGetVirtual FtVspcGetVirtualW
#else
#define FtVspcGetVirtual FtVspcGetVirtualA
#endif // !UNICODE

typedef FtVspc_ErrorCode (__cdecl *FTVSPCGETLASTERROR)(void);
extern FTVSPCGETLASTERROR FtVspcGetLastError;


/*
*	Retrieves the error code description.
*/
typedef size_t (__cdecl *FTVSPCGETERRORMESSAGEW)(FtVspc_ErrorCode, LPWSTR, UINT);
extern FTVSPCGETERRORMESSAGEW FtVspcGetErrorMessageW;

typedef size_t (__cdecl *FTVSPCGETERRORMESSAGEA)(FtVspc_ErrorCode, LPSTR, UINT);
extern FTVSPCGETERRORMESSAGEA FtVspcGetErrorMessageA;

#ifdef UNICODE
#define FtVspcGetErrorMessage FtVspcGetErrorMessageW
#else
#define FtVspcGetErrorMessage FtVspcGetErrorMessageA
#endif // !UNICODE


#ifdef __cplusplus
}
#endif
