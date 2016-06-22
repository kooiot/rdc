#ifndef koo_process_h__
#define koo_process_h__


#pragma once
#include "pre_std_basic.h"
#include "koo_file_util.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif
class koo_process
{
private:
	std::string m_rolename;
	std::string m_workpath;
	std::string m_execname;
	std::string m_arguments;
	bool m_run_flag;
	bool m_create_new_window;
#ifdef _WIN32
	PROCESS_INFORMATION m_proc_info;
#else
        int m_pid = 0;
#endif
	std::shared_ptr<std::thread> trd_proc_control;
	std::mutex mtx_proc_control;
	std::condition_variable_any cond_proc_control;
public:
	koo_process(const std::string& rolename, const std::string& workpath, const std::string& execname, const std::string& args, bool bCreateNewConsole = true);
	~koo_process();
private:
	int _open_process();
	void _close_process();
	int _proc_exec();
public:
	int start_exec();
	int wait_exec(int timeout = 2000);
	int start_once(int timeout = 2000);
	int start();
	int stop();
	int pid()
	{
#ifdef _WIN32
		return m_proc_info.dwProcessId;
#else
		return m_pid;
#endif
	}
};
extern "C"
{
	int k_kill_process(const std::string& process_name, int timeout = 1000);
	int k_kill_process_by_id(int pid, int timeout = 0);
}
typedef std::shared_ptr<koo_process> koo_proc_ptr;
#endif // koo_process_h__
