#include "DataJson.h"

namespace koo {

bool parse_json(ConnectionInfo&, const json&) {
	return true;
}

bool parse_json(DeviceInfo&, const json&) {
	return true;
}

bool parse_json(UserInfo&, const json&) {
	return true;
}

bool parse_json(StreamProcess&, const json&) {
	return true;
}

bool parse_json(IPInfo&, const json&) {
	return true;
}

bool parse_json(AllowInfo&, const json&) {
	return true;
}

bool parse_json(DenyInfo&, const json&) {
	return true;
}

json generate_json(const UserInfo&) {
	return json();
}

json generate_json(const DeviceInfo&) {
	return json();
}

json generate_json(const ConnectionInfo&) {
	return json();
}

json generate_json(const StreamProcess&) {
	return json();
}

json generate_json(const IPInfo&) {
	return json();
}

json generate_json(const AllowInfo&) {
	return json();
}

json generate_json(const DenyInfo&) {
	return json();
}

}
