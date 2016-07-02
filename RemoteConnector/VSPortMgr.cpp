#include "stdafx.h"
#include "VSPortMgr.h"


VSPortMgr::VSPortMgr()
{
}


VSPortMgr::~VSPortMgr()
{
}

int VSPortMgr::Init()
{
	BOOL loaded = FtVspcDllInit();
	if (!loaded)
		return -1;

	LPCTSTR key = NULL;
	/*
	// For example, how use your key:
	LPCTSTR key = (_T("-----BEGIN VIRTUALSERIALPORTCONTROL KEY BLOCK-----")
	_T("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
	_T("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
	_T("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
	_T("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
	_T("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
	_T("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
	_T("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
	_T("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
	_T("xxxxxxxx")
	_T("-----END VIRTUALSERIALPORTCONTROL KEY BLOCK-----")
	);
	*/

	if (!FtVspcApiInit(OnVspcControlEvents, (LONG_PTR)this, key)) {
		//ShowError();
		return -1;
	}
	return 0;
}

int VSPortMgr::Close()
{
	FtVspcApiClose();
	FtVspcDllClose();
	return 0;
}

VSPort * VSPortMgr::CreatePort(RC_CHANNEL channel, IPortHandler & handler, const std::string & name)
{
	VSPort* port = new VSPort(channel, handler, name);
	if (port->Create())
		return port;
	delete port;
	return nullptr;
}

void VSPortMgr::FreePort(VSPort * port)
{
	port->Remove();
	delete port;
}


void __cdecl VSPortMgr::OnVspcControlEvents(
	FtVspc_Event Event,	// event type
	ULONG_PTR ulValue,	// value (depends of event type)
	LONG_PTR Context	// user value (set by function FtVspcApiInit)
	)
{
	FT_VSPC_PORT *pFtVspcPort;
	VSPort *pThis = (VSPort *)Context;

	switch (Event)
	{
	case ftvspcEventThirdPartyPortCreate:
		pFtVspcPort = (FT_VSPC_PORT *)ulValue;
		/* To Do place your event handler here */
		break;
	case ftvspcEventThirdPartyPortRemove:
		pFtVspcPort = (FT_VSPC_PORT *)ulValue;
		/* To Do place your event handler here */
		break;
	case ftvspcEventPortCreate:
		pFtVspcPort = (FT_VSPC_PORT *)ulValue;
		/* To Do place your event handler here */
		break;
	case ftvspcEventPortRemove:
		pFtVspcPort = (FT_VSPC_PORT *)ulValue;
		/* To Do place your event handler here */
		break;
	case ftvspcEventTrialExpired:
		pFtVspcPort = (FT_VSPC_PORT *)ulValue;
		/* To Do place your event handler here */
		break;
	case ftvspcEventPortLimitExceeded:
		pFtVspcPort = (FT_VSPC_PORT *)ulValue;
		/* To Do place your event handler here */
		break;
	case ftvspcEventLicenseQuotaExceeded:
		/* To Do place your event handler here */
		break;
	};
}

