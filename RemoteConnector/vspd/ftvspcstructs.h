/*******************************************************************************
//
//	ftvspcstructs.h
//
//	Virtual Serial Port Control
//
//	VSPC API structures declaration
//
//	Copyright (c) FabulaTech
//
*******************************************************************************/
#pragma once

#ifndef FTVSPCAX
#include <WTypes.h>
#include "ftvspctypes.h"
#endif	//FTVSPCAX

#pragma pack(push, 4)

#define PORT_NAME_LENGTH 35

typedef struct _FT_VSPC_APP
{
	DWORD dwPid;				// process id
	CHAR cAppPath[MAX_PATH];	// ASCIIZ path to application
	WCHAR wcAppPath[MAX_PATH];	// UNICODE path to application

#ifdef __cplusplus
	_FT_VSPC_APP()
	{
		dwPid = 0;
		cAppPath[0] = 0;
		wcAppPath[0] = 0;
	}
#endif
} FT_VSPC_APP;

typedef struct _FTVSPC_INFOA
{
	UINT unVspcInfoSize;		// size of FTVSPC_INFOA
	CHAR cVersion[32];			// application version (string)
	FtVspc_LicenseType LicenseType;	// license type
	UINT unNumberOfLicenses;	// license count (0 - w\o limitation)
	UINT unNumberOfPorts;		// count of allowed ports (0 - w\o limitation)
	UINT unPortTrialTime;		// time of the port working in minutes (0 - w\o limitation)
	CHAR cLicensedUser[255];	// licensed user
	CHAR cLicensedCompany[255];	// licensed company
	CHAR cExpirationDate[64];	// sting with end date of the trial period (empty string - w\o time limitation)
#ifndef FTVSPCAX
#ifdef __cplusplus
	_FTVSPC_INFOA()
	{
		unVspcInfoSize = 0;
		cVersion[0] = 0;
		LicenseType = ftvspcLicenseDemo;
		unNumberOfLicenses = 0;
		unNumberOfPorts = 0;
		unPortTrialTime = 0;
		cLicensedUser[0] = 0;
		cLicensedCompany[0] = 0;
		cExpirationDate[0] = 0;
	}
#endif
#endif
} FTVSPC_INFOA;

typedef struct _FTVSPC_INFOW
{
	UINT unVspcInfoSize;
	WCHAR cVersion[32];
	FtVspc_LicenseType LicenseType;
	UINT unNumberOfLicenses;
	UINT unNumberOfPorts;
	UINT unPortTrialTime;
	WCHAR cLicensedUser[255];
	WCHAR cLicensedCompany[255];
	WCHAR cExpirationDate[64]; // if empty - no any time limitation

#ifndef FTVSPCAX
#ifdef __cplusplus
	_FTVSPC_INFOW()
	{
		unVspcInfoSize = 0;
		cVersion[0] = 0;
		LicenseType = ftvspcLicenseDemo;
		unNumberOfLicenses = 0;
		unNumberOfPorts = 0;
		unPortTrialTime = 0;
		cLicensedUser[0] = 0;
		cLicensedCompany[0] = 0;
		cExpirationDate[0] = 0;
	}
#endif
#endif
} FTVSPC_INFOW;

typedef struct _FT_VSPC_PORT
{
	UINT unPortNo;		// COM-port number
	CHAR cPortNameA[PORT_NAME_LENGTH];	// ASCIIZ port name
	WCHAR cPortNameW[PORT_NAME_LENGTH];	// UNICODE port name
} FT_VSPC_PORT;

#ifdef UNICODE
#define FTVSPC_INFO FTVSPC_INFOW
#else
#define FTVSPC_INFO FTVSPC_INFOA
#endif // !UNICODE

#pragma pack(pop)