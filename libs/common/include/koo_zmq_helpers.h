#pragma once

#include <zmq.h>
#include <string>
#include <sstream>
#include <cassert>
#include <json.hpp>

using json = nlohmann::json;

struct KZPacket {
	KZPacket() {
		zmq_msg_init(&_msg);
	}
	KZPacket(const std::string& id, const std::string& cmd) {
		zmq_msg_init(&_msg);
		_json["id"] = id;
		_json["cmd"] = cmd;
	}

	~KZPacket() {
		zmq_msg_close(&_msg);
	}

	const std::string& id() const {
		const json& v = _json["id"];
		return v.get_ref<const json::string_t&>();
	}
	const std::string& cmd() const {
		const json& v = _json["cmd"];
		return v.get_ref<const json::string_t&>();
	}
	const json& get(const std::string& name) const{
		return _json[name];
	}
	template<class TYPE>
	const json& set(const std::string& name, const TYPE& v) {
		_json[name] = v;
	}

	json _json;
	zmq_msg_t _msg;
};

inline int koo_zmq_recv(void* skt, KZPacket& packet) {
	int rc = zmq_msg_recv(&packet._msg, skt, 0);
	if (rc == -1)
		return -1;

	char* str = (char*)zmq_msg_data(&packet._msg);
	try {
		packet._json = json::parse(str);
	}
	catch (...) {
		return -1;
	}
	return 0;
}

inline int koo_zmq_send(void* skt, const KZPacket& packet) {
	std::stringstream str;
	packet._json >> str;
	return zmq_send(skt, str.str().c_str(), str.str().length(), 0);
}

inline int koo_zmq_send_result(void* skt, const KZPacket& request, const json& result) {
	try {
		KZPacket packet;
		packet._json["id"] = request.id();
		packet._json["cmd"] = request.cmd();
		packet._json["result"] = result;
		return koo_zmq_send(skt, packet);
	}
	catch(...) {
		return -1;
	}
}
