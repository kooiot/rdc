#pragma once

#include "vspd/ftvspc.h"
#include "VSPort.h"

class VSPortMgr
{
public:
	VSPortMgr();
	~VSPortMgr();

	int Init();
	int Close();

	VSPort* CreatePort(RC_CHANNEL channel, IPortHandler *handler, const std::string& name);
	void FreePort(VSPort* port);
private:
	static void __cdecl OnVspcControlEvents(
		FtVspc_Event Event,	// event type
		ULONG_PTR ulValue,	// value (depends of event type)
		LONG_PTR Context	// user value (set by function FtVspcApiInit)
		);

};

