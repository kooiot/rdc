#include <json.hpp>
#include <list>
#include <string>
#include "DataDefs.h"

using json = nlohmann::json;


#define KOO_PARSE_JSON(DST, JSON) koo::parse_json(DST, JSON)
#define KOO_GEN_JSON(DATA) koo::generate_json(DATA)

#define JSON_FROM_PACKET(PACKET, TYPE, NAME) \
	TYPE NAME; \
	KOO_PARSE_JSON(NAME, PACKET.get(#NAME));

namespace koo {
	bool parse_json(ConnectionInfo&, const json&);
	bool parse_json(DeviceInfo&, const json&);
	bool parse_json(GroupInfo&, const json&);
	bool parse_json(UserInfo&, const json&);
	bool parse_json(StreamProcess&, const json&);
	bool parse_json(IPInfo&, const json&);
	bool parse_json(AllowInfo&, const json&);
	bool parse_json(DenyInfo&, const json&);
	bool parse_json(StreamEventPacket&, const json&);
	bool parse_json(std::list<std::string>&, const json&);

	json generate_json(const UserInfo&);
	json generate_json(const DeviceInfo&);
	json generate_json(const GroupInfo&);
	json generate_json(const ConnectionInfo&);
	json generate_json(const StreamProcess&);
	json generate_json(const IPInfo&);
	json generate_json(const AllowInfo&);
	json generate_json(const DenyInfo&);
	json generate_json(const StreamEventPacket&);
	json generate_json(const std::list<std::string>&);
}
