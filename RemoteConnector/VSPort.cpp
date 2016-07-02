#include "stdafx.h"
#include "VSPort.h"


VSPort::VSPort(RC_CHANNEL channel, IPortHandler& Handler, const std::string& name)
	: m_nChannel(channel), m_Handler(Handler), m_Name(name), m_hPort(0)
{
}


VSPort::~VSPort()
{
}

bool VSPort::Create()
{

	if (!FtVspcCreatePort(m_Name.c_str())) {
		if (FtVspcGetLastError() != ftvspcErrorPortAlreadyExists)
		{
			ShowError();
			return false;
		}
	}

	m_hPort = FtVspcAttach(m_Name.c_str(), OnVspcPortEvents, (LONG_PTR)this);
	if (m_hPort == 0)
	{
		ShowError();
		return false;
	}
	//int port = FtVspcAttach(m_Name.c_str(), OnVspcPortEvents, (LONG_PTR)this);
	
	if (!FtVspcSetCts(m_hPort, FALSE)
		|| !FtVspcSetDcd(m_hPort, FALSE)
		|| !FtVspcSetDsr(m_hPort, FALSE)
		|| !FtVspcSetRing(m_hPort, FALSE)
		|| !FtVspcSetBreak(m_hPort, FALSE)
		|| !FtVspcSetParity(m_hPort, FALSE)
		|| !FtVspcSetOverrun(m_hPort, FALSE)
		|| !FtVspcSetFraming(m_hPort, FALSE))
	{
		ShowError();
		return false;
	}

	return true;
}

bool VSPort::Remove()
{
	if (m_hPort != NULL)
	{
		if (!FtVspcDetach(m_hPort))
		{
			ShowError();
			return false;
		}
		m_hPort = NULL;

		if (!FtVspcRemovePort(m_Name.c_str()))
		{
			if (FtVspcGetLastError() != ftvspcErrorPortInUse)
			{
				ShowError();
				return false;
			}
		}
	}
	return true;
}

int VSPort::OnData(void * buf, size_t len)
{
	BOOL ret = FtVspcWrite(m_hPort, buf, len);
	if (!ret) {
		ShowError();
		return -1;
	}
	return 0;
}

int VSPort::OnEvent(StreamEvent evt)
{
	return 0;
}

LONG_PTR __cdecl VSPort::OnVspcPortEvents(
	FtVspc_PortEvent Event,	// event type
	ULONG_PTR ulValue,	// value (depends of event type)
	LONG_PTR Context	// user value (set by function FtVspcApiInit)
	)
{
	FT_VSPC_APP *pFVA = NULL;
	COMMTIMEOUTS *pCT = NULL;
	VSPort *pThis = (VSPort *)Context;
	LONG_PTR lpRes = NULL;
	BOOL bVal;

	if (pThis == NULL)
		return NULL;

	switch (Event)
	{
	case ftvspcPortEventOpen:
		pFVA = (FT_VSPC_APP *)ulValue;
		pThis->Info("Port is opened");
		break;
	case ftvspcPortEventOpenBeforeAttach:
		pFVA = (FT_VSPC_APP *)ulValue;
		pThis->Info("Application with PID:%u, path(%s) opened port", pFVA->dwPid, pFVA->wcAppPath);
		break;
	case ftvspcPortEventQueryOpen:
		pFVA = (FT_VSPC_APP *)ulValue;
		pThis->Info("Application with pid(%d)/path(%s) open port", pFVA->dwPid, pFVA->wcAppPath);
		//if (pDlg->MessageBox(_buf, TEXT("Query open", MB_YESNO | MB_ICONQUESTION) == IDYES)
		//	lpRes = 1;
		lpRes = 1;
		break;
	case ftvspcPortEventClose:
		pThis->Info("Port is closed");
		break;
	case ftvspcPortEventRxChar:
	{
		size_t nBytes, cbRead;
		if (!FtVspcGetInQueueBytes(pThis->m_hPort, &nBytes))
		{
			pThis->ShowError();
			break;
		}
		pThis->Info("Application has written %u bytes.", nBytes);

		if (nBytes > 0)
		{
			// pDlg->SendMessage(MSG_UPADTE_AND_SCROLL_CONTROLS);
			char* buf = (char *)malloc(nBytes + 1);
			memset(buf, 0, nBytes + 1);
			if (buf == NULL)
			{
				//::MessageBoxA(NULL, "Insuficient memory!", "VSPCDemo", MB_ICONERROR);
				break;
			}
			if (!FtVspcRead(pThis->m_hPort, buf, nBytes, &cbRead))
			{
				pThis->ShowError();
				break;
			}
			pThis->m_Handler.Send(pThis->m_nChannel, buf, cbRead);
			free(buf);
		}
	} break;
	case ftvspcPortEventDtr:
		bVal = (BOOL)ulValue;
		pThis->Info("Application has set DTR to %u.", bVal);
		break;
	case ftvspcPortEventRts:
		bVal = (BOOL)ulValue;
		pThis->Info("Application has set RTS to %u.", bVal);
		break;
	case ftvspcPortEventBaudRate:
		pThis->Info("Application has set baud rate to %u.", ulValue);
		break;
	case ftvspcPortEventDataBits:
		pThis->Info("Application has set ByteSize to %u.", ulValue);
		break;
	case ftvspcPortEventParity:
		pThis->Info("Application has set Parity flag to %u.", ulValue);
		break;
	case ftvspcPortEventStopBits:
		pThis->Info("Application has set StopBits to %u.", ulValue);
		break;
	case ftvspcPortEventBreak:
		bVal = (BOOL)ulValue;
		if (bVal)
			pThis->Info("Application has called SetCommBreak");
		else
			pThis->Info("Application has called ClearCommBreak");
		break;
	case ftvspcPortEventPurge:
		pThis->Info("Application has called PurgeComm with dwFlags = %x", ulValue);
		break;
	case ftvspcPortEventXonLim:
		pThis->Info("Application has set XonLim to %u.", ulValue);
		break;
	case ftvspcPortEventXoffLim:
		pThis->Info("Application has set XoffLim to %u.", ulValue);
		break;
	case ftvspcPortEventXonChar:
		pThis->Info("Application has set XonChar to %x.", ulValue);
		break;
	case ftvspcPortEventXoffChar:
		pThis->Info("Application has set XoffChar to %x.", ulValue);
		break;
	case ftvspcPortEventErrorChar:
		pThis->Info("Application has set ErrorChar to %x.", ulValue);
		break;
	case ftvspcPortEventEofChar:
		pThis->Info("Application has set EofChar to %x.", ulValue);
		break;
	case ftvspcPortEventEvtChar:
		pThis->Info("Application has set EvtChar to %x.", ulValue);
		break;
	case ftvspcPortEventBreakChar:
		pThis->Info("Application has set BreakChar to %x.", ulValue);
		break;
	case ftvspcPortEventTimeouts:
		pCT = (COMMTIMEOUTS *)ulValue;
		pThis->Info("Application has set timeouts to %u:%u:%u:%u:%u.", pCT->ReadIntervalTimeout,
			pCT->ReadTotalTimeoutMultiplier, pCT->ReadTotalTimeoutConstant,
			pCT->WriteTotalTimeoutMultiplier, pCT->WriteTotalTimeoutConstant);
		break;
	case ftvspcPortEventOutxCtsFlow:
		bVal = (BOOL)ulValue;
		pThis->Info("Application has set fOutxCtsFlow flag to %u.", ulValue);
		break;
	case ftvspcPortEventOutxDsrFlow:
		bVal = (BOOL)ulValue;
		pThis->Info("Application has set fOutxDsrFlow flag to %u.", bVal);
		break;
	case ftvspcPortEventDtrControl:
		pThis->Info("Application has set fDtrControl flag to %u.", ulValue);
		break;
	case ftvspcPortEventDsrSensitivity:
		bVal = (BOOL)ulValue;
		pThis->Info("Application has set fDsrSensitivity flag to %u.", bVal);
		break;
	case ftvspcPortEventTXContinueOnXoff:
		bVal = (BOOL)ulValue;
		pThis->Info("Application has set fTXContinueOnXoff flag to %u.", bVal);
		break;
	case ftvspcPortEventOutX:
		bVal = (BOOL)ulValue;
		pThis->Info("Application has set fOutX flag to %u.", bVal);
		break;
	case ftvspcPortEventInX:
		bVal = (BOOL)ulValue;
		pThis->Info("Application has set fInX flag to %u.", bVal);
		break;
	case ftvspcPortEventNull:
		bVal = (BOOL)ulValue;
		pThis->Info("Application has set fNull flag to %u.", bVal);
		break;
	case ftvspcPortEventRtsControl:
		pThis->Info("Application has set fRtsControl to %u.", ulValue);
		break;
	case ftvspcPortEventAbortOnError:
		bVal = (BOOL)ulValue;
		pThis->Info("Application has set fAbortOnError flag to %u.", bVal);
		break;
	case ftvspcPortEventUseErrorChar:
		bVal = (BOOL)ulValue;
		pThis->Info("Application has set fUseErrorChar flag to %u.", bVal);
		break;
	}

	return lpRes;
}



void VSPort::ShowError()
{
	char MessageBuffer[1024];
	FtVspc_ErrorCode ftErr = FtVspcGetLastError();

	if (!FtVspcGetErrorMessage(ftErr, MessageBuffer, 1024))
	{
		sprintf(MessageBuffer, "%s", "!!!Error calling FtVspcGetErrorMessage API function!!!");
	}
	TRACE("VSPD_ERROR: %s\n", MessageBuffer);
	Error("%s", MessageBuffer);
}
