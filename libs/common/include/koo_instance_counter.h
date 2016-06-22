#ifndef __koo_instance_counter_h__
#define __koo_instance_counter_h__
#pragma once

class koo_instance_counter
{
private:
	koo_instance_counter(void);
	mutex m_mtx;
	map<string,int> m_count_map;
	static shared_ptr<koo_instance_counter> g_instance_counter;

	static mutex g_mtx_koo_instance_counter;
public:
	~koo_instance_counter(void);
	int inc(const string& key);
	int dec(const string& key);
	static  shared_ptr<koo_instance_counter> const instance_counter();
	static void init();
};
typedef koo_instance_counter visteck_ic;
typedef shared_ptr<visteck_ic> visteck_ic_ptr;
#endif