#include "Auth.h"



CAuth::CAuth()
{
}


CAuth::~CAuth()
{
}

int CAuth::Init()
{
	return 0;
}

int CAuth::IsValidMapper(const std::string & mapper_id)
{
	return 0;
}

int CAuth::Login(const std::string & id, const std::string & password)
{
	return 0;
}

int CAuth::Logout(const std::string & id)
{
	return 0;
}

int CAuth::Access(const std::string & id, const std::string & mapper_id)
{
	return 0;
}

int CAuth::Allow(const std::string & id, const std::string & mapper_id)
{
	return 0;
}

int CAuth::Deny(const std::string & id, const std::string & mapper_id)
{
	return 0;
}
