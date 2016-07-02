/*******************************************************************************
//
//	ftvspctypes.h
//
//	Virtual Serial Port Control
//
//	VSPC API types declaration
//
//	Copyright (c) FabulaTech
//
*******************************************************************************/

#pragma once

typedef LONG_PTR FTVSPCHANDLE;

typedef enum _FtVspc_ErrorCode
{
	ftvspcErrorOk = 0,
	ftvspcErrorFailed,
	ftvspcErrorInvalidParameter,
	ftvspcErrorAlreadyInitialized,
	ftvspcErrorInsufficientBuffer,
	ftvspcErrorInvalidLicense,
	ftvspcErrorIncompatibleVersions,
	ftvspcErrorPortAlreadyExists = 100,
	ftvspcErrorNoSuchPort,
	ftvspcErrorNoMoreItems,
	ftvspcErrorPortAttached = 200,
	ftvspcErrorPortInUse,
	ftvspcErrorMarkedForDeletion,
	ftvspcErrorInvalidFunction = 10000,	// Non-public statuses (for internal use only)
	ftvspcErrorOperationUnsupported		// Used only by .NET-control and ActiveX

} FtVspc_ErrorCode;

typedef enum _FtVspc_LicenseType
{
	ftvspcLicenseDemo,
	ftvspcLicenseDeveloper,
	ftvspcLicenseOem,
	ftvspcLicenseUser

} FtVspc_LicenseType;

typedef enum _FtVspc_DataBits
{
	ftvspcDataBits5,
	ftvspcDataBits6,
	ftvspcDataBits7,
	ftvspcDataBits8
} FtVspc_DataBits;

typedef enum _FtVspc_Parity
{
	ftvspcParityNone,
	ftvspcParityOdd,
	ftvspcParityEven,
	ftvspcParityMark,
	ftvspcParitySpace
} FtVspc_Parity;

typedef enum _FtVspc_StopBits
{
	ftvspcStopBitsOne,
	ftvspcStopBitsOne5,
	ftvspcStopBitsTwo
} FtVspc_StopBits;

typedef enum _FtVspc_Event
{
	ftvspcEventThirdPartyPortCreate = 0,
	ftvspcEventThirdPartyPortRemove,
	ftvspcEventPortCreate,
	ftvspcEventPortRemove,
	ftvspcEventTrialExpired = 100,
	ftvspcEventPortLimitExceeded,
	ftvspcEventLicenseQuotaExceeded	//	<Win32 only>

} FtVspc_Event;

typedef enum _FtVspc_PortEvent
{
	ftvspcPortEventOpen = 0,
	ftvspcPortEventOpenBeforeAttach,
	ftvspcPortEventQueryOpen,
	ftvspcPortEventClose, 
	ftvspcPortEventRxChar,
	ftvspcPortEventDtr,
	ftvspcPortEventRts,
	ftvspcPortEventBaudRate,
	ftvspcPortEventDataBits,
	ftvspcPortEventParity,
	ftvspcPortEventStopBits,
	ftvspcPortEventBreak,
	ftvspcPortEventPurge,
	ftvspcPortEventXonLim,
	ftvspcPortEventXoffLim,
	ftvspcPortEventXonChar,
	ftvspcPortEventXoffChar,
	ftvspcPortEventErrorChar,
	ftvspcPortEventEofChar,
	ftvspcPortEventEvtChar,
	ftvspcPortEventBreakChar,
	ftvspcPortEventTimeouts,
	ftvspcPortEventOutxCtsFlow,			// fOutxCtsFlow			SERIAL_CTS_HANDSHAKE
	ftvspcPortEventOutxDsrFlow,			// fOutxDsrFlow			SERIAL_DSR_HANDSHAKE
	ftvspcPortEventDtrControl,			// fDtrControl			SERIAL_DTR_MASK = SERIAL_DTR_CONTROL | SERIAL_DTR_HANDSHAKE
	ftvspcPortEventDsrSensitivity,		// fDsrSensitivity		SERIAL_DSR_SENSITIVITY
	ftvspcPortEventTXContinueOnXoff,	// fTXContinueOnXoff	SERIAL_XOFF_CONTINUE
	ftvspcPortEventOutX,				// fOutX				SERIAL_AUTO_TRANSMIT
	ftvspcPortEventInX,					// fInX					SERIAL_AUTO_RECEIVE
	ftvspcPortEventNull,				// fNull				SERIAL_NULL_STRIPPING
	ftvspcPortEventRtsControl,			// fRtsControl			SERIAL_RTS_MASK = SERIAL_RTS_CONTROL | SERIAL_RTS_HANDSHAKE | SERIAL_TRANSMIT_TOGGLE
	ftvspcPortEventAbortOnError,		// fAbortOnError		SERIAL_ERROR_ABORT
	ftvspcPortEventUseErrorChar			// fErrorChar			SERIAL_ERROR_CHAR

} FtVspc_PortEvent;

typedef
void (__cdecl *LPFNVSPCEVENTS) (
								FtVspc_Event Event,	// event type
								ULONG_PTR ulValue,	// value (depends of event type)
								LONG_PTR Context	// user value (set by function FtVspcApiInit)
								);

typedef
LONG_PTR (__cdecl *LPFNVSPCPORTEVENTS) (
										FtVspc_PortEvent PortEvent,	// event type
										ULONG_PTR ulValue,	// value (depends of event type)
										LONG_PTR Context	// user value (set by function FtVspcAttach)
										);
