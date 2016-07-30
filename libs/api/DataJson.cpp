#include "DataJson.h"
#include <string>
#include <cstring>

#define GET_NODE_STRING(NODE, NAME, BUF, BUF_LEN) \
	strncpy(BUF, NODE[NAME].get<std::string>().c_str(), BUF_LEN)

#define GET_NODE_TIME(NODE, NAME, VAL) \
	VAL = str2time(NODE[NAME].get<std::string>())

#define GET_NODE_ENUM(NODE, NAME, ETYPE, VAL) \
	VAL = (ETYPE)NODE[NAME].get<int>()

namespace koo {

time_t str2time(const std::string &strTime)
{
	if (strTime.empty())
		return 0;
	struct tm sTime;
	sscanf(strTime.c_str(), "%d-%d-%d %d:%d:%d", &sTime.tm_year, &sTime.tm_mon, &sTime.tm_mday, &sTime.tm_hour, &sTime.tm_min, &sTime.tm_sec);
	sTime.tm_year -= 1900;
	sTime.tm_mon -= 1;
	time_t ft = mktime(&sTime);
	return ft;
}

const std::string time2str(const time_t *_Time = NULL) {
	char buffer[80];
	time_t now = _Time != NULL ? *_Time : time(NULL);

	struct tm * timeinfo = localtime(&now);
	strftime(buffer, 80, "%F %T", timeinfo);

	return std::string(buffer);
}

bool parse_json(SerialInfo& info, const json& j) {
	try {
		GET_NODE_STRING(j, "dev", info.dev, RC_MAX_NAME_LEN);
		info.baudrate = j["baudrate"];
		GET_NODE_ENUM(j, "bytesize", bytesize, info.bytesize);
		GET_NODE_ENUM(j, "parity", parity, info.parity);
		GET_NODE_ENUM(j, "stopbits", stopbits, info.stopbits);
		GET_NODE_ENUM(j, "flowcontrol", flowcontrol, info.flowcontrol);
	}
	catch(...) {
	}
	return false;
}

bool parse_json(TCPClientInfo& info, const json& j) {
	return parse_json(info.bind, j["bind"])
		&& parse_json(info.remote, j["remote"]);
}

bool parse_json(UDPInfo& info, const json& j) {
	return parse_json(info.bind, j["bind"])
		&& parse_json(info.remote, j["remote"]);
}

bool parse_json(TCPServerInfo& info, const json& j) {
	return parse_json(info.bind, j["bind"])
		&& parse_json(info.remote, j["remote"]);
}

bool parse_json(PluginInfo& info, const json& j) {
	try {
		GET_NODE_STRING(j, "name", info.Name, RC_MAX_PLUGIN_NAME_LEN);
		GET_NODE_STRING(j, "data", info.Data, RC_MAX_PLUGIN_DATA_LEN);
		return true;
	}
	catch(...) {}
	return false;
}

bool parse_json(ConnectionInfo& info, const json& j) {
	try {
		GET_NODE_ENUM(j, "type", ConnectionType, info.Type);
		GET_NODE_ENUM(j, "channel", RC_CHANNEL, info.Channel);
		GET_NODE_STRING(j, "dev_sn", info.DevSN, RC_MAX_SN_LEN);

		bool ret = true;
		switch(info.Type) {
		case CT_SERIAL:
			ret = parse_json(info.Serial, j["info"]);
		case CT_TCPC:
			ret = parse_json(info.TCPClient, j["info"]);
		case CT_UDP:
			ret = parse_json(info.UDP, j["info"]);
		case CT_TCPS:
			ret = parse_json(info.TCPServer, j["info"]);
		case CT_PLUGIN:
			ret = parse_json(info.Plugin, j["info"]);
		case CT_TEST:
		default:
			break;
		}
		return ret;
	}
	catch(...) {
	}
	return false;
}

bool parse_json(DeviceInfo& info, const json& j) {
	try {
		info.Index = j["index"];
		GET_NODE_STRING(j, "sn", info.SN, RC_MAX_SN_LEN);
		GET_NODE_STRING(j, "name", info.Name, RC_MAX_NAME_LEN);
		GET_NODE_STRING(j, "desc", info.Desc, RC_MAX_DESC_LEN);
		GET_NODE_TIME(j, "create_time", info.CreateTime); 
		GET_NODE_TIME(j, "valid_time", info.ValidTime); 
		return true;
	}
	catch(...) {
		return false;
	}
}

bool parse_json(UserInfo& info, const json& j) {
	try {
		info.Index = j["index"];
		info.Level = j["level"];
		GET_NODE_STRING(j, "id", info.ID, RC_MAX_ID_LEN);
		GET_NODE_STRING(j, "name", info.Name, RC_MAX_NAME_LEN);
		GET_NODE_STRING(j, "desc", info.Desc, RC_MAX_DESC_LEN);
		GET_NODE_STRING(j, "passwd", info.Passwd, RC_MAX_PASSWD_LEN);
		GET_NODE_STRING(j, "email", info.Email, RC_MAX_EMAIL_LEN);
		GET_NODE_STRING(j, "phone", info.Phone, RC_MAX_PHONE_LEN);
		GET_NODE_TIME(j, "create_time", info.CreateTime); 
		GET_NODE_TIME(j, "valid_time", info.ValidTime); 
		return true;
	}
	catch(...) {
		return false;
	}
}

bool parse_json(StreamProcess& sp, const json& j) {
	try {
		sp.Index = j["index"];
		sp.Mask = j["mask"];
		GET_NODE_STRING(j, "ip", sp.StreamIP, RC_MAX_IP_LEN);
		sp.Port = j["port"];
		return true;
	}
	catch(...) {
		return false;
	}
}

bool parse_json(IPInfo& info, const json& j) {
	try {
		GET_NODE_STRING(j, "ip", info.sip, RC_MAX_IP_LEN);
		info.port = j["port"];
		return true;
	}
	catch(...) {}
	return false;

}

bool parse_json(AllowInfo& info, const json& j) {
	try {
		GET_NODE_STRING(j, "id", info.ID, RC_MAX_ID_LEN);
		GET_NODE_STRING(j, "dev_sn", info.DevSN, RC_MAX_SN_LEN);
		GET_NODE_TIME(j, "valid_time", info.ValidTime); 
		return true;
	}
	catch(...) {
		return false;
	}
}

bool parse_json(DenyInfo& info, const json& j) {
	try {
		GET_NODE_STRING(j, "id", info.ID, RC_MAX_ID_LEN);
		GET_NODE_STRING(j, "dev_sn", info.DevSN, RC_MAX_SN_LEN);
		return true;
	}
	catch(...) {
		return false;
	}
}

json generate_json(const UserInfo& info) {
	json j;
	j["index"] = info.Index;
	j["level"] = info.Level;
	j["id"] = info.ID;
	j["name"] = info.Name;
	j["desc"] = info.Desc;
	j["passwd"] = info.Passwd;
	j["email"] = info.Email;
	j["phone"] = info.Phone;
	j["create_time"] = time2str(&info.CreateTime);
	j["valid_time"] = time2str(&info.ValidTime);
	return j;
}

json generate_json(const DeviceInfo& info) {
	json j;
	j["index"] = info.Index;
	j["sn"] = info.SN;
	j["name"] = info.Name;
	j["desc"] = info.Desc;
	j["create_time"] = time2str(&info.CreateTime);
	j["valid_time"] = time2str(&info.ValidTime);
	return j;
}

json generate_json(const SerialInfo& info) {
	json j;
	j["dev"] = info.dev;
	j["baudrate"] = info.baudrate;
	j["bytesize"] = (int)info.bytesize;
	j["parity"] = (int)info.parity;
	j["stopbits"] = (int)info.stopbits;
	j["flowcontrol"] = (int)info.flowcontrol;
	return j;
}

json generate_json(const TCPClientInfo& info) {
	json j;
	j["bind"] = generate_json(info.bind);
	j["remote"] = generate_json(info.bind);
	return j;
}
json generate_json(const UDPInfo& info) {
	json j;
	j["bind"] = generate_json(info.bind);
	j["remote"] = generate_json(info.bind);
	return j;
}

json generate_json(const TCPServerInfo& info) {
	json j;
	j["bind"] = generate_json(info.bind);
	j["remote"] = generate_json(info.bind);
	return j;
}

json generate_json(const PluginInfo& info) {
	json j;
	j["name"] = info.Name;
	j["data"] = info.Data;
	return j;
}

json generate_json(const ConnectionInfo& info) {
	json j;
	j["type"] = (int)info.Type;
	j["channel"] = info.Channel;
	j["dev_sn"] = info.DevSN;

	switch(info.Type) {
		case CT_SERIAL:
			j["info"] = generate_json(info.Serial);
		case CT_TCPC:
			j["info"] = generate_json(info.TCPClient);
		case CT_UDP:
			j["info"] = generate_json(info.UDP);
		case CT_TCPS:
			j["info"] = generate_json(info.TCPServer);
		case CT_PLUGIN:
			j["info"] = generate_json(info.Plugin);
		case CT_TEST:
		default:
			j["info"] = "";
			break;
	}
	return j;
}

json generate_json(const StreamProcess& sp) {
	json j;
	j["index"] = sp.Index;
	j["mask"] = sp.Mask;
	j["ip"] = sp.StreamIP;
	j["port"] = sp.Port;
	return j;
}

json generate_json(const IPInfo& info) {
	json j;
	j["ip"] = info.sip;
	j["port"] = info.port;
	return j;
}

json generate_json(const AllowInfo& info) {
	json j;
	j["id"] = info.ID;
	j["dev_sn"] = info.DevSN;
	j["valid_time"] = time2str(&info.ValidTime);
	return j;
}

json generate_json(const DenyInfo& info) {
	json j;
	j["id"] = info.ID;
	j["dev_sn"] = info.DevSN;
	return j;
}

}
