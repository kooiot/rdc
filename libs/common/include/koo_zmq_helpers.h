#pragma once

#include <zmq.h>
#include <string>
#include <cassert>

struct CMD {
	std::string id;
	std::string cmd;
	std::string data;
};

struct StreamProcess {
	int Counter;
	char StreamIP[128];
	int Port;
};

bool recv_string(void* skt, std::string& str, bool more);
int recv_cmd(void* skt, CMD& cmd);
int send_msg(void* skt, const std::string& data, bool more);
int send_reply(void* skt, const CMD& cmd, const std::string& rep);
int send_cmd(void* skt, const CMD& cmd);
