#include "../include/uri_parser.h"
#include "pre_regex.h"
#include "koo_time_util.h"
#include "koo_string_util.h"

uri_parser::uri_parser(const std::string& uri)
	: m_uri(uri)
{
	m_is_ready = false;
	regex_text_finder rtf;
	if (rtf.find(uri, "(.+?)://(.+?)/(.+)"))
	{
		m_protocol_name = k_str_to_lower(rtf[1]);
		std::string endpoint = rtf[2];
		std::string path = rtf[3];
		if (path.find("?") != std::string::npos)
		{
			if (rtf.find(path, "(.+?)\\?(.+$)"))
			{
				m_resource_path = rtf[1];
				m_param_str = rtf[2];
			}
		}
		else
		{
			m_resource_path = path;
		}
		if (rtf.find(endpoint, "(.+?)@(.+$)"))
		{
			std::string auth = rtf[1];
			_parse_user_pass(auth);
			endpoint = rtf[2];
		}
		if (_parse_endpoint(endpoint))
		{
			m_is_ready = true;
		}
	}
	
}


uri_parser::~uri_parser()
{
}

std::string uri_parser::std_to_uri_time(const std::string& stdTime)
{
	char data[32] = { 0 };
	boost::posix_time::ptime t = k_ptime_from_str(stdTime);
	sprintf(data, "%4d%02d%02d%02d%02d%02d", t.date().year(), t.date().month(), t.date().day(),
		t.time_of_day().hours(), t.time_of_day().minutes(), t.time_of_day().seconds());
	return move(std::string(data));
}

std::string uri_parser::uri_to_std_time(const std::string &uriTime)
{
	int year, month, day, hour, minute, second;
	sscanf(uriTime.data(), "%4d-%02d-%02d %02d:%02d:%02d", &year, &month,
		&day, &hour, &minute, &second);
	char data[32] = { 0 };
	sprintf(data, "%4d%2d%2d%2d%2d%2d", year, month, day, hour, minute, second);
	return move(std::string(data));
}

bool uri_parser::_parse_user_pass(const std::string& auth_str)
{
	regex_text_finder rtf;
	if (rtf.find(auth_str, "(.+?):(.+)"))
	{
		m_username = rtf[1];
		m_password = rtf[2];
		return true;
	}
	return false;
}

bool uri_parser::_parse_endpoint(const std::string& endpoint_str)
{
	if (endpoint_str.find(":") != std::string::npos)
	{

		regex_text_finder rtf;

		if (rtf.find(endpoint_str, "(.+?):(\\d+)"))
		{
			m_hostname = rtf[1];
			m_port = atoi(rtf[2].c_str());
		}
		else
		{
			return false;
		}
	}
	else
	{
		m_hostname = endpoint_str;
		if (m_protocol_name == "rtsp")
		{
			m_port = 554;
		}
		else
		{
			m_port = 80;
		}
	}
	return true;
}


