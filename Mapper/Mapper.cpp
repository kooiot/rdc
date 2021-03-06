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
#include <koo_zmq_helpers.h>
#include <DataDefs.h>
#include <DataJson.h>
#include <PluginLoader.h>

#include "StreamMgr.h"

const char* g_sn = "4C05D6F6-92EA-4a23-8EFF-179F91CBAA6A";
#ifdef _DEBUG
const char* g_sip = "127.0.0.1";
#else
const char* g_sip = "123.57.13.218";
#endif
int g_port_req = 6600;
int g_port_sub = 6601;

void* g_req_socket;
void* g_sub_socket;
StreamMgr* g_StreamMgr = NULL;
CPluginLoader g_PluginLoader(RPT_MAPPER);

#define COMPARE_MSG_STR(msg, str) \
	strncmp((char*)zmq_msg_data(&msg), str, strlen(str)) == 0

void handle_sub_msg(zmq_msg_t& cmd, zmq_msg_t& data) {
	printf("CMD: %s\n", std::string((char*)zmq_msg_data(&cmd), zmq_msg_size(&cmd)).c_str());
	char* pCmd = (char*)zmq_msg_data(&cmd);
	
	if (strncmp(pCmd, "ALL ", strlen("ALL ")) == 0)
		return;
	
	// Remov the SN prefix
	assert(strncmp(pCmd, g_sn, strlen(g_sn)) == 0);
	pCmd = pCmd + strlen(g_sn) + 1;

	json _json;
	std::string str((char*)zmq_msg_data(&data), zmq_msg_size(&data));
	try {
		_json = json::parse(str);
	}
	catch (...) {
		std::cerr << "Json Parse Exception!!! " << std::endl;
		return;
	}

	if (0 == strncmp(pCmd, "CREATE", strlen("CREATE"))) {
		StreamProcess sp;
		ConnectionInfo info;
		if (KOO_PARSE_JSON(sp, _json["sp"]) && KOO_PARSE_JSON(info, _json["info"])) {
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
			std::cerr << "Recevied Incorrect CREATE JSON" << std::endl;
		}
	}
	else if (0 == strncmp(pCmd, "DESTROY", strlen("DESTROY"))) {
		StreamProcess sp;
		if (!KOO_PARSE_JSON(sp, _json["sp"])) {
			std::cerr << "Recevied Incorrect JSON of DESTROY " << std::endl;
		}
		else {
			int channel = _json["channel"];
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

int SendCmd(const KZPacket& packet) {
	void* sock = g_req_socket;
	int rc = koo_zmq_send(sock, packet);
	if (rc >= 0) {
		KZPacket data;
		rc = koo_zmq_recv(sock, data);
		if (rc == 0) {
			if (packet.cmd() == data.cmd())
				return data.get("result");
			else if (data.cmd() == "TIMEOUT")
				return -10000;
			else
				return -20000;
		}
	}

	return rc;
}

int Login() {
	KZPacket packet("@DEVICE@", "LOGIN");
	packet.set("sn", g_sn);
	return SendCmd(packet);
}

int Logout() {
	KZPacket packet("@DEVICE@", "LOGOUT");
	packet.set("sn", g_sn);
	return SendCmd(packet);
}

int Heartbeat() {
	KZPacket packet("@DEVICE@", "HEARTBEAT");
	packet.set("sn", g_sn);
	return SendCmd(packet);
}
void Heartbeat_Timer(uv_timer_t* handle){
	std::cout << "Send Heartbeat..." << std::endl;
	int rc = Heartbeat();
	if (rc == -10000) {
		std::cerr << "Timeout!!! need login again" << std::endl;
		exit(EXIT_FAILURE);
	}
}


#ifdef RDC_LINUX_SYS
#define MAX_PATH 512
int GetModuleFileName(char* sModuleName, char* sFileName, int nSize)
{
	int ret = 0;
	char* p = getenv("_");
	if (p != NULL)
	{
		if (sModuleName != NULL)
			strstr(p, sModuleName);
		if (p != NULL)
			strcpy(sFileName, p);
		ret = strlen(sFileName);
	}
	return ret;
}
#endif

std::string GetModuleFilePath()
{
	char szFile[MAX_PATH] = { 0 };
	int dwRet = ::GetModuleFileName(NULL, szFile, 255);
	if (dwRet != 0)
	{
		printf("Module File Name: %s \n", szFile);
		std::string str = szFile;
#ifndef RDC_LINUX_SYS
		size_t nPos = str.rfind('\\');
#else
		size_t nPos = str.rfind('/');
#endif
		if (nPos != std::string::npos)
		{
			str = str.substr(0, nPos);
		}
		return str;
	}
	return "";
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

#ifndef RDC_LINUX_SYS
	std::string plugin_folder = GetModuleFilePath() + "\\plugins";
#else
	//std::string plugin_folder = GetModuleFilePath() + "/plugins";
	std::string plugin_folder = "plugins";
#endif
	g_PluginLoader.Load(plugin_folder.c_str());

	g_StreamMgr = new StreamMgr(loop);
	g_StreamMgr->Init();

	void * ctx;
	ctx = zmq_ctx_new();
	assert(ctx);
	int rc = create_req_socket(ctx);
	if (rc != 0)
		exit(EXIT_FAILURE);
	rc = Login();
	if (rc != 0) {
		if (rc == -99) {
			printf("Device SN not authed!\n");
#ifndef RDC_LINUX_SYS
			Sleep(5000);
#else
			sleep(5);
#endif
		}
		exit(EXIT_FAILURE);
	}
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
	g_PluginLoader.UnLoad();
	enet_deinitialize();
#ifdef _DEBUG
	system("pause");
#endif
    return 0;
}
