#include "koo_zmq_helpers.h"

bool recv_string(void* skt, std::string& str, bool more) {
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
	if (!recv_string(skt, x, more)) \
		return -1;

int recv_cmd(void* skt, CMD& cmd) {
	RECV_STRING(skt, cmd.id, true);
	RECV_STRING(skt, cmd.cmd, true);
	RECV_STRING(skt, cmd.data, false);
	return 0;
}

int send_msg(void* skt, const std::string& data, bool more) {
	int rc = zmq_send(skt, (void*)data.c_str(), data.length(), more ? ZMQ_SNDMORE : 0);
	return rc;
}

#define SEND_MSG(skt, data, more) \
	if (!send_msg(skt, data, more)) \
		return -1;

int send_reply(void* skt, const CMD& cmd, const std::string& rep) {
	SEND_MSG(skt, cmd.id, true);
	SEND_MSG(skt, cmd.cmd, true);
	SEND_MSG(skt, rep, false);
	return 0;
}

int send_cmd(void* skt, const CMD& cmd) {
	SEND_MSG(skt, cmd.id, true);
	SEND_MSG(skt, cmd.cmd, true);
	SEND_MSG(skt, cmd.data, false);
	return 0;
}
