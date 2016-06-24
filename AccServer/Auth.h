#pragma once

#include <string>

class CAuth
{
public:
	CAuth();
	~CAuth();
	
	int Init();

	// Mappers
	int IsValidMapper(const std::string& mapper_id);

	// Clients
	int Login(const std::string& id, const std::string& password);
	int Logout(const std::string& id);

	// Permissions
	int Access(const std::string& id, const std::string& mapper_id);
	int Allow(const std::string& id, const std::string& mapper_id);
	int Deny(const std::string& id, const std::string& mapper_id);

private:
};

