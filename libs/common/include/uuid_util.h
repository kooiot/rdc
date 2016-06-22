#ifndef _UUID_UTIL_H_
#define _UUID_UTIL_H_

#pragma once

#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

class k_uuid  : public  boost::uuids::uuid
{
private:
	static boost::uuids::random_generator rgen; //随机生成器
	static boost::uuids::string_generator sgen; //随机生成器
public:
	k_uuid(): uuid(rgen()){}//缺省构造函数，生成随机UUID

	k_uuid(int):uuid(boost::uuids::nil_uuid()){}//0值的uuid构造函数

	k_uuid(const char*str) :uuid(sgen(str)){}//字符串构造函数
	k_uuid(const std::string& str) :uuid(sgen(str)){}//字符串构造函数

	k_uuid(const uuid&u, const char* str) : uuid(boost::uuids::name_generator(u)(str)){}//名字生成器构造函数
	explicit k_uuid(const uuid& u):uuid(u){} //拷贝构造函数
	
	operator uuid()//转换到uuid类型
	{ return static_cast<boost::uuids::uuid&>(*this);}

	operator uuid() const //常函数，转换到const uuid类型
	{ return static_cast<const uuid&>(*this);}
	operator std::string()//转换到string类型
	{
		return this->tostring();
	}
	std::string tostring()
	{
		return boost::lexical_cast<std::string>(*this);
	}
};
#endif