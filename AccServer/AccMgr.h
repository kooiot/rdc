#pragma once

#include <map>
#include <string>
#include "ClientMgr.h"
#include "AccDatabase.h"
#include <koo_zmq_helpers.h>

class CAccMgr
{
public:
	CAccMgr(CClientMgr& ClientMgr, CAccDatabase& Database);
	~CAccMgr();

	void* Init(void* ctx, const char* bip, int port);
	void Close();
	void OnRecv();
private:
	void HandleKZPacket(const KZPacket& cmd);

private:
	void* m_pReply;
	CClientMgr& m_ClientMgr;
	CAccDatabase& m_Database;
};

