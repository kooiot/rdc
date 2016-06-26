#pragma once

#include <zmq.h>
#include <string>
#include <cassert>

struct KZPacket {
	KZPacket();
	~KZPacket();

	std::string id;
	std::string cmd;
	zmq_msg_t data;

	void* GetData() const;
	void SetData(const void* data, size_t len);
	std::string GetStr() const;
	void SetStr(const char* str);
};

bool koo_zmq_recv_string(void* skt, std::string& str, bool more);
int koo_zmq_recv_cmd(void* skt, KZPacket& cmd);
int koo_zmq_send_msg(void* skt, const std::string& data, bool more);
int koo_zmq_send_reply(void* skt, const KZPacket& cmd, const std::string& rep);
int koo_zmq_send_reply(void* skt, const KZPacket& cmd, void* data, int len);
int koo_zmq_send_cmd(void* skt, const KZPacket& cmd);
