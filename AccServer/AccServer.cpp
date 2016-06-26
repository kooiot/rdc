// AccServer.cpp : 定义控制台应用程序的入口点。
//
#include <iostream>
#include <sstream>
#include <cassert>
#include <zmq.h>

#include "StreamServerMgr.h"
#include "ClientMgr.h"
#include "koo_process.h"

void* ctx = NULL;
CStreamServerMgr StreamMgr;
CClientMgr ClientMgr(StreamMgr);
koo_process *process = NULL;

void on_close() {
	printf("Exit function called\n");
	if (process)
		process->stop();
	StreamMgr.Close();
	ClientMgr.Close();
	zmq_ctx_term(ctx);
}



int main(int argc, char* argv[])
{
	int r = atexit(on_close);

	char* bip = "127.0.0.1";
	int port_rep = 6600;
	int port_pub = 6601;
	int port_stream = 6602;

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
	
	ctx = zmq_ctx_new();

	void* sm_skt = StreamMgr.Init(ctx, bip, port_stream);
	void* cli_skt = ClientMgr.Init(ctx, bip, port_rep, port_pub);

	k_kill_process("StreamServer.exe");
	std::stringstream args;
	args << RC_STREAM_SERVER_ID_BASE << " ";
	args << bip << " ";
	args << port_stream << " ";
	args << bip << " ";
	args << 6800 << " ";

	process = new koo_process("StreamServer", "", "StreamServer.exe", args.str(), true);
	process->start();

	while (true) {
		zmq_pollitem_t items[] = {
			{ sm_skt,   0, ZMQ_POLLIN, 0 },
			{ cli_skt,   0, ZMQ_POLLIN, 0 },
		};
		zmq_poll(items, 2, -1);
		if (items[0].revents & ZMQ_POLLIN) {
			StreamMgr.OnRecv();
		}
		if (items[1].revents & ZMQ_POLLIN) {
			ClientMgr.OnRecv();
		}
	}

	on_close();

    return 0;
}

