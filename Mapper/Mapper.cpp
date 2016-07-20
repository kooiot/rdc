// Mapper.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <cstring>
#include <zmq.h>
#include <uv.h>
#include <enet/enet.h>

#include "StreamMgr.h"
#include "../api/DataDefs.h"

const char* g_sn = "4C05D6F6-92EA-4a23-8EFF-179F91CBAA6A";
const char* g_sip = "127.0.0.1";
int g_port_req = 6600;
int g_port_sub = 6601;

void* g_req_socket;
void* g_sub_socket;
StreamMgr* g_StreamMgr = NULL;

#define COMPARE_MSG_STR(msg, str) \
	strncmp((char*)zmq_msg_data(&msg), str, strlen(str)) == 0

void handle_sub_msg(zmq_msg_t& cmd, zmq_msg_t& data) {
	printf("CMD: %s\n", std::string((char*)zmq_msg_data(&cmd), zmq_msg_size(&cmd)).c_str());
	char* pCmd = (char*)zmq_msg_data(&cmd);
	
	if (strncmp(pCmd, "ALL ", strlen("ALL ")) == 0)
		return;

	pCmd = pCmd + strlen(g_sn) + 1;
	if (0 == strncmp(pCmd, "CREATE", strlen("CREATE"))) {
		if (zmq_msg_size(&data) >= sizeof(StreamProcess) + sizeof(ConnectionInfo)) {
			StreamProcess sp;
			ConnectionInfo info;
			memcpy(&sp, zmq_msg_data(&data), sizeof(StreamProcess));
			memcpy(&info, (char*)zmq_msg_data(&data) + sizeof(StreamProcess), sizeof(ConnectionInfo));
			int rc = g_StreamMgr->Create(sp, info);
			// FIXME: For failure handlesss
			if (rc != 0) {
				std::cerr << "StreamMgr Create failure, returns " << rc << std::endl;
			}
			else {
				std::cout << "StreamMgr Create OK" << std::endl;
			}
		}
		else {
			std::cerr << "Recevied Incorrect Size of CREATE " << std::endl;
			std::cerr << "Received Size " << zmq_msg_size(&data);
			std::cerr << " Expected Size " << sizeof(StreamProcess) + sizeof(ConnectionInfo);
			std::cerr << std::endl;
		}
	}
	else if (0 == strncmp(pCmd, "DESTROY", strlen("DESTROY"))) {
		if (zmq_msg_size(&data) < sizeof(StreamProcess) + sizeof(int)) {
			std::cerr << "Recevied Incorrect Size of DESTROY " << std::endl;
			std::cerr << "Received Size " << zmq_msg_size(&data);
			std::cerr << " Expected Size " << sizeof(StreamProcess) + sizeof(int);
			std::cerr << std::endl;
		}
		else {
			StreamProcess sp;
			memcpy(&sp, zmq_msg_data(&data), sizeof(StreamProcess));
			int channel = atoi((char*)zmq_msg_data(&data) + sizeof(StreamProcess));
			int rc = g_StreamMgr->Destroy(sp, channel);
			if (rc != 0) {
				std::cerr << "StreamMgr Destroy failure, returns " << rc << std::endl;
			}
			else {
				std::cout << "StreamMgr Destroy OK" << std::endl;
			}
		}
	}
	else {
	}
}

void proccss_sub_msg(uv_poll_t *req, int status, int events)
{
	void * sub = req->data;
	assert(sub == g_sub_socket);
	if (!(events & UV_READABLE))
		return;


	while (true) {
		int zevents;
		size_t zlen = sizeof(zevents);
		zmq_getsockopt(sub, ZMQ_EVENTS, &zevents, &zlen);
		if (!(zevents & ZMQ_POLLIN))
			break;

		zmq_msg_t cmd;
		zmq_msg_init(&cmd);
		zmq_msg_recv(&cmd, sub, 0);
		if (!zmq_msg_more(&cmd)) {
			zmq_msg_close(&cmd);
			return;
		}
		zmq_msg_t data;
		zmq_msg_init(&data);
		zmq_msg_recv(&data, sub, 0);
		if (!zmq_msg_more(&data)) {
			handle_sub_msg(cmd, data);
		}
		else {
			while (zmq_msg_more(&data)) {
				zmq_msg_recv(&data, sub, 0);
			}
		}
		zmq_msg_close(&cmd);
		zmq_msg_close(&data);
	}
}
int create_req_socket(void* ctx) {
	std::stringstream conn;
	conn << "tcp://" << g_sip << ":" << g_port_req;

	void* req = zmq_socket(ctx, ZMQ_REQ);
	assert(req);
	int timeo = 5 * 1000;
	int rc = zmq_setsockopt(req, ZMQ_RCVTIMEO, &timeo, sizeof(int));
	timeo = 0;
	rc = zmq_setsockopt(req, ZMQ_LINGER, &timeo, sizeof(int));
	rc = zmq_connect(req, conn.str().c_str());
	if (rc == -1) {
		std::cerr << "Error Connect to " << conn.str() << " errno " << errno << std::endl;
		zmq_close(req);
		return -1;
	}
	g_req_socket = req;
	return 0;
}

int create_sub_socket(void* ctx) {
	std::stringstream conn;
	conn << "tcp://" << g_sip << ":" << g_port_sub;
	void * sub = zmq_socket(ctx, ZMQ_SUB);
	int rc = zmq_connect(sub, conn.str().c_str());
	assert(rc == 0);
	if (rc != 0) {
		std::cerr << "Error Connect to " << conn.str() << " errno " << errno << std::endl;
		zmq_close(sub);
		exit(EXIT_FAILURE);
	}

	char temp[256] = { 0 };
	sprintf(temp, "%s ", g_sn);
	printf("Subscribe id: %s\n", temp);
	rc = zmq_setsockopt(sub, ZMQ_SUBSCRIBE, temp, strlen(temp));
	assert(rc == 0);
	rc = zmq_setsockopt(sub, ZMQ_SUBSCRIBE, "ALL ", strlen("ALL "));
	assert(rc == 0);
	g_sub_socket = sub;
	return 0;
}

int GetReturnRC(zmq_msg_t* data) {
	char* pBuf = (char*)zmq_msg_data(data);
	if (!isdigit(*pBuf) && *pBuf != '-')
		return -1;
	return atoi(pBuf);
}

int SendCmd(const char* id, const char* cmd_str, const void* data_str, size_t len) {
	void* sock = g_req_socket;
	int rc = zmq_send(sock, id, strlen(id), ZMQ_SNDMORE);
	if (rc == -1)
		return rc;
	rc = zmq_send(sock, cmd_str, strlen(cmd_str), ZMQ_SNDMORE);
	if (rc == -1)
		return rc;
	rc = zmq_send(sock, data_str, len, 0);
	if (rc == -1)
		return rc;

	zmq_msg_t rid;
	zmq_msg_t cmd;
	zmq_msg_t data;
	zmq_msg_init(&rid);
	zmq_msg_init(&cmd);
	zmq_msg_init(&data);
	int ret = -1;
	rc = zmq_msg_recv(&rid, sock, 0);
	if (rc == -1 || !zmq_msg_more(&rid))
		goto LOGIN_RETURN;
	rc = zmq_msg_recv(&cmd, sock, 0);
	if (rc == -1 || !zmq_msg_more(&cmd))
		goto LOGIN_RETURN;
	rc = zmq_msg_recv(&data, sock, 0);
	if (rc != -1 && !zmq_msg_more(&data)) {
		if (COMPARE_MSG_STR(rid, id)) {
			if (COMPARE_MSG_STR(cmd, cmd_str)) {
				ret = GetReturnRC(&data);
			} 
			else if(COMPARE_MSG_STR(cmd, "TIMEOUT")) {
				return -10000;
			} 
			else {
				return -20000;
			}
		}
		else {
			printf("Incorrect ID returned : %s\n", std::string((char*)zmq_msg_data(&data), zmq_msg_size(&data)).c_str());
		}
	}
	while (zmq_msg_more(&data)) {
		zmq_msg_recv(&data, sock, 0);
	}

LOGIN_RETURN:
	zmq_msg_close(&rid);
	zmq_msg_close(&cmd);
	zmq_msg_close(&data);
	return ret;
}
int Login() {
	return SendCmd("@DEVICE@", "LOGIN", g_sn, strlen(g_sn));
}

int Logout() {
	return SendCmd("@DEVICE@", "LOGOUT", g_sn, strlen(g_sn));
}

int Heartbeat() {
	return SendCmd("@DEVICE@", "HEARTBEAT", g_sn, strlen(g_sn));
}
void Heartbeat_Timer(uv_timer_t* handle){
	std::cout << "Send Heartbeat..." << std::endl;
	int rc = Heartbeat();
	if (rc == -10000) {
		std::cerr << "Timeout!!! need login again" << std::endl;
		exit(EXIT_FAILURE);
	}
}
int main(int argc, char* argv[])
{
	if (argc >= 2)
		g_sn = argv[1];
	if (argc >= 3)
		g_sip = argv[2];
	if (argc >= 4)
		g_port_req = atoi(argv[3]);
	if (argc >= 5)
		g_port_sub = atoi(argv[4]);

	std::cout << "Device SN: \t" << g_sn << std::endl;
	std::cout << "Server IP: \t" << g_sip << std::endl;
	std::cout << "Port_REQ: \t" << g_port_req << std::endl;
	std::cout << "Port_SUB: \t" << g_port_sub << std::endl;

	enet_initialize();
	
	uv_loop_t *loop = uv_default_loop();

	g_StreamMgr = new StreamMgr(loop);
	g_StreamMgr->Init();

	void * ctx;
	ctx = zmq_ctx_new();
	assert(ctx);
	int rc = create_req_socket(ctx);
	if (rc != 0)
		exit(EXIT_FAILURE);
	rc = Login();
	if (rc != 0)
		exit(EXIT_FAILURE);
	rc = create_sub_socket(ctx);
	if (rc != 0)
		exit(EXIT_FAILURE);
	
	uv_poll_t poll_sub;
	uv_os_sock_t socket;
	size_t len = sizeof(uv_os_sock_t);
	zmq_getsockopt(g_sub_socket, ZMQ_FD, &socket, &len);
	rc = uv_poll_init_socket(loop, &poll_sub, socket);
	assert(rc == 0);
	poll_sub.data = g_sub_socket;
	rc = uv_poll_start(&poll_sub, UV_READABLE, proccss_sub_msg);
	assert(rc == 0);

	uv_timer_t timer;
	rc = uv_timer_init(loop, &timer);
	assert(rc == 0);
	rc = uv_timer_start(&timer, Heartbeat_Timer, 1000, 20 * 1000);
	assert(rc == 0);

	//while (true) {
	//	rc = uv_run(loop, UV_RUN_ONCE);
	//}

	uv_run(loop, UV_RUN_DEFAULT);

	rc = Logout();

	uv_timer_stop(&timer);
	uv_poll_stop(&poll_sub);

	zmq_close(g_req_socket);
	zmq_close(g_sub_socket);
	zmq_ctx_term(ctx);
	
	g_StreamMgr->Close();
	delete g_StreamMgr;
	enet_deinitialize();
#ifdef _DEBUG
	system("pause");
#endif
    return 0;
}
