#pragma once

#include <string>
#include <map>
#include <mutex>
#include <sqlite3.h>

#define AUTH_OK 0
#define AUTH_ERROR 1
#define AUTH_NO_EXITS 2
#define AUTH_TIMEOUT 3

class CAuth
{
public:
	CAuth();
	~CAuth();
	
	int Init();
	void Close();

	// Mappers
	int IsValidMapper(const std::string& mapper_id);

	// Clients
	int Login(const std::string& id, const std::string& password);
	int Logout(const std::string& id);

	// Permissions
	int Access(const std::string& id, const std::string& mapper_id);
	int Allow(const std::string& id, const std::string& mapper_id, time_t* _Time = NULL);
	int Deny(const std::string& id, const std::string& mapper_id);
protected:
	int GetClientIndex(const std::string& id);
	int GetMapperIndex(const std::string& id);
	int _Access(int user, int dev);
	int _Allow(int user, int dev, time_t* _Time = NULL);
	int _Deny(int user, int dev);
private:
	sqlite3 * m_pDB;
	std::mutex m_Lock;
};

