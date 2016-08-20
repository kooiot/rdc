#include "stdafx.h"
#include <DataJson.h>
#include <PluginDefs.h>
#include "NetStream.h"
#include <cstring>

NetStream::NetStream(CPortMgr& mgr, StreamPortInfo& info)
	: StreamPortBase(info)
	, m_Mgr(mgr)
{
	m_Port = m_Mgr.Create(info.ConnInfo.Channel, this, info.ConnInfo);
}


NetStream::~NetStream()
{
	m_Mgr.Destory(m_Port);
}

bool NetStream::Open()
{
	return m_Port->Open();
}


void NetStream::Close()
{
	m_Port->Close();
	FireEvent(SE_CHANNEL_CLOSED);
}

int NetStream::OnWrite(void * data, size_t len)
{
	return m_Port->Write(data, len);
}

int NetStream::OnLog(RC_CHANNEL channel, const char * type, const char * content)
{
	printf("%s Channel %d Type %s : %s\n", __FUNCTION__, channel, type, content);
	return 0;
}

int NetStream::OnRecv(RC_CHANNEL channel, void * buf, size_t len)
{
	return this->SendData(buf, len);
}

void NetStream::OnClose(RC_CHANNEL channel)
{
	this->OnStreamClose();
}

void NetStream::OnOpen(RC_CHANNEL channel, bool open)
{
	if (open)
		FireEvent(SE_CHANNEL_OPENED);
	else
		FireEvent(SE_CHANNEL_OPEN_FAILED, "Failed to Open Network Stream");
}
