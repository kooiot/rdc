#include "koo_zmq_helpers.h"

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
#define RECV_STRING(skt, x, more) \
	if (!koo_zmq_recv_string(skt, x, more)) \
		return -1;

int koo_zmq_recv_cmd(void* skt, KZPacket& cmd) {
	RECV_STRING(skt, cmd.id, true);
	RECV_STRING(skt, cmd.cmd, true);
	RECV_STRING(skt, cmd.data, false);
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
	SEND_MSG(skt, cmd.data, false);
	return 0;
}
