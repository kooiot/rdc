// AccServer.cpp : 定义控制台应用程序的入口点。
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <zmq.h>
#include <json.hpp>
using json = nlohmann::json;

#include "StreamServerMgr.h"
#include "ClientMgr.h"

void* ctx = NULL;
CStreamServerMgr StreamMgr;
CClientMgr ClientMgr(StreamMgr);

void on_close() {
	printf("Exit function called\n");
	StreamMgr.Close();
	ClientMgr.Close();
	zmq_ctx_term(ctx);
}

std::string GetModuleFilePath();

void load_conf(std::string& bip, int& port_rep, int& port_pub, int& port_stream)
{
	try {
#ifndef RDC_LINUX_SYS
		std::ifstream file(GetModuleFilePath() + "\\AccServer.conf");
#else
		std::ifstream file(GetModuleFilePath() + "/acc.conf");
#endif
		json doc;
		doc << file;
		file.close();
		bip = doc["ip"];
		port_rep = doc["reply_port"];
		port_pub = doc["public_port"];
		port_stream = doc["stream_port"];

	}
	catch (...) {

	}
}

void save_conf(const std::string& bip, int port_rep, int port_pub, int port_stream)
{
	try {
#ifndef RDC_LINUX_SYS
		std::ofstream file(GetModuleFilePath() + "\\AccServer.conf");
#else
		std::ofstream file(GetModuleFilePath() + "/acc.conf");
#endif
		json doc;
		doc["ip"] = bip;
		doc["reply_port"] = port_rep;
		doc["public_port"] = port_pub;
		doc["stream_port"] = port_stream;

		std::cout << doc.dump(0) << std::endl;
		file << doc.dump(0);
		file.close();
	}
	catch (...) {

	}
}
int main(int argc, char* argv[])
{
	int r = atexit(on_close);

	std::string bip = "127.0.0.1";
	int port_rep = 6600;
	int port_pub = 6601;
	int port_stream = 6602;

	load_conf(bip, port_rep, port_pub, port_stream);

	if (argc >= 2)
		bip = argv[1];
	if (argc >= 3)
		port_rep = atoi(argv[2]);
	if (argc >= 4)
		port_pub = atoi(argv[3]);
	if (argc >= 5)
		port_stream = atoi(argv[4]);

	std::cout << "Bind IP: \t" << bip << std::endl;
	std::cout << "Port_REP: \t" << port_rep << std::endl;
	std::cout << "Port_PUB: \t" << port_pub << std::endl;
	std::cout << "Port_STREAM: \t" << port_stream << std::endl;

	save_conf(bip, port_rep, port_pub, port_stream);

	ctx = zmq_ctx_new();

	void* sm_skt = StreamMgr.Init(ctx, bip.c_str(), port_stream);
	void* cli_skt = ClientMgr.Init(ctx, bip.c_str(), port_rep, port_pub);

	while (true) {
		zmq_pollitem_t items[] = {
			{ sm_skt,   0, ZMQ_POLLIN, 0 },
			{ cli_skt,   0, ZMQ_POLLIN, 0 },
		};
		zmq_poll(items, 2, 1000);
		if (items[0].revents & ZMQ_POLLIN) {
			StreamMgr.OnRecv();
		}
		if (items[1].revents & ZMQ_POLLIN) {
			ClientMgr.OnRecv();
		}
		ClientMgr.OnTimer(time(NULL));
	}

	on_close();

    return 0;
}

