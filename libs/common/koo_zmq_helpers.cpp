#include "koo_zmq_helpers.h"

KZPacket::KZPacket()
{
	zmq_msg_init(&data);
}

KZPacket::~KZPacket()
{
	zmq_msg_close(&data);
}

void * KZPacket::GetData() const
{
	return zmq_msg_data((zmq_msg_t*)&data);
}
void KZPacket::SetData(void* buf, size_t len)
{
	zmq_msg_close(&data);
	zmq_msg_init_size(&data, len);
	memcpy(zmq_msg_data(&data), buf, len);
}
std::string KZPacket::GetStr() const
{
	return std::string((char*)zmq_msg_data((zmq_msg_t*)&data), zmq_msg_size((zmq_msg_t*)&data));
}

void KZPacket::SetStr(const char* str)
{
	SetData((void*)str, strlen(str) + 1);
}

bool koo_zmq_recv_string(void* skt, std::string& str, bool more) {
	zmq_msg_t part;
	int rc = zmq_msg_init(&part);

	assert(rc == 0);
	rc = zmq_msg_recv(&part, skt, 0);
	if (rc == -1)
		return false;

	if (more && !zmq_msg_more(&part)) {
		zmq_msg_close(&part);
		return false;
	}

	str = std::string((char*)zmq_msg_data(&part), zmq_msg_size(&part));
	zmq_msg_close(&part);
	return true;
}
bool koo_zmq_recv_msg(void* skt, zmq_msg_t& msg, bool more) {
	int rc = zmq_msg_recv(&msg, skt, 0);
	if (rc == -1)
		return false;

	if (more && !zmq_msg_more(&msg)) {
		return false;
	}
	return true;
}
#define RECV_STRING(skt, x, more) \
	if (!koo_zmq_recv_string(skt, x, more)) \
		return -1;
#define RECV_ZMQ_MSG(skt, x, more) \
	if (!koo_zmq_recv_msg(skt, x, more)) \
		return -1;

int koo_zmq_recv_cmd(void* skt, KZPacket& cmd) {
	RECV_STRING(skt, cmd.id, true);
	RECV_STRING(skt, cmd.cmd, true);
	RECV_ZMQ_MSG(skt, cmd.data, false);
	return 0;
}

int koo_zmq_send_msg(void* skt, const std::string& data, bool more) {
	if (data.empty())
		return zmq_send(skt, (void*)data.c_str(), 1, more ? ZMQ_SNDMORE : 0);
	else
		return zmq_send(skt, (void*)data.c_str(), data.length(), more ? ZMQ_SNDMORE : 0);
}

#define SEND_MSG(skt, data, more) \
	if (!koo_zmq_send_msg(skt, data, more)) \
		return -1;

int koo_zmq_send_reply(void* skt, const KZPacket& cmd, const std::string& rep) {
	SEND_MSG(skt, cmd.id, true);
	SEND_MSG(skt, cmd.cmd, true);
	SEND_MSG(skt, rep, false);
	return 0;
}

int koo_zmq_send_reply(void * skt, const KZPacket & cmd, void * data, int len)
{
	SEND_MSG(skt, cmd.id, true);
	SEND_MSG(skt, cmd.cmd, true);
	int rc = zmq_send(skt, data, len, 0);
	if (rc != -1)
		return 0;
	return -1;
}

int koo_zmq_send_cmd(void* skt, const KZPacket& cmd) {
	SEND_MSG(skt, cmd.id, true);
	SEND_MSG(skt, cmd.cmd, true);

	int rc = zmq_send(skt, zmq_msg_data((zmq_msg_t*)&cmd.data), 
		zmq_msg_size((zmq_msg_t*)&cmd.data), 0);

	if (rc != -1)
		return 0;
	return -1;
}
