#include "koo_process.h"
#include "koo_file_util.h"
#include "koo_error_code.h"
#include "koo_lock_defs.h"
#include "koo_string_util.h"

koo_process::~koo_process()
{
    stop();
}

koo_process::koo_process(const std::string& rolename, const std::string& workpath, const std::string& execname, const std::string& args, bool bCreateNewConsole /*= true*/)
: m_rolename(rolename), m_workpath(workpath), m_execname(execname), m_arguments(args), m_create_new_window(bCreateNewConsole)
{
#ifdef _WIN32
    memset(&m_proc_info, 0, sizeof (m_proc_info));
#endif
}

int koo_process::stop()
{
    if (trd_proc_control != 0)
    {
        _close_process();
        {
            k_lock(lck, mtx_proc_control);
            m_run_flag = false;
            cond_proc_control.notify_one();
        }

        if (trd_proc_control->joinable())
        {
            trd_proc_control->join();
            trd_proc_control.reset();
        }
    }
    return K_OK;
}

int koo_process::start()
{
    if (trd_proc_control == 0)
    {
        m_run_flag = true;
        trd_proc_control.reset(new std::thread(std::bind(&koo_process::_proc_exec, this)));
    }
    return K_OK;
}

int koo_process::_proc_exec()
{
    int64_t duration_ms = 1000;
    bool bExited = true;
    int ret = K_OK;
    while (m_run_flag)
    {
        k_unique_lock(lk, mtx_proc_control);
        std::chrono::system_clock::time_point start_ts = std::chrono::system_clock::now();
        if (bExited)
        {
            ret = _open_process();
            if (ret == K_OK)
            {
                bExited = false;
            }
        }
        if (!bExited)
        {
#ifdef _WIN32
            int32_t x = WaitForSingleObject(m_proc_info.hProcess, 1000);
            if (x == WAIT_OBJECT_0)
            {
                bExited = true;
            }
#else
			int y = 0;
            int x = waitpid(m_pid, &y, WNOHANG);
            if (x == m_pid || x < 0 )
            {
				_close_process();
                bExited = true;
            }
#endif
        }
        if (!m_run_flag)
        {
            break;
        }
        duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_ts).count();
        if (m_run_flag && duration_ms < 3000)
        {
            cond_proc_control.wait_for(lk, std::chrono::seconds(5));
        }
        if (!m_run_flag)
        {
            break;
        }
    }
    _close_process();
    return K_OK;
}

void koo_process::_close_process()
{
#ifdef _WIN32
    if (m_proc_info.hProcess != nullptr)
    {
        TerminateProcess(m_proc_info.hProcess, 0);
        CloseHandle(m_proc_info.hProcess);
        memset(&m_proc_info, 0, sizeof (m_proc_info));
    }
#else
	vvprint_s("process: " << m_pid << " will be killed...");
    if (m_pid != 0)
    {
        kill(m_pid, 9);
		waitpid(m_pid, NULL, 0);
		vvprint_s("process: " << m_pid << " has been killed...");
        m_pid = 0;
    }
#endif
}

int koo_process::_open_process()
{
    boost::filesystem::path w = boost::filesystem::path(m_workpath);
#ifdef _WIN32
    boost::filesystem::path p = w / (m_execname + " " + m_arguments);
    STARTUPINFO si = {sizeof (si)};
    si.dwFlags = STARTF_USESHOWWINDOW;

    std::wstring cmd = p.native();
    DWORD creationFlag = 0;
    if (m_create_new_window)
    {
		si.wShowWindow = SW_SHOWDEFAULT;
        creationFlag |= CREATE_NEW_CONSOLE;
    }
	else
	{
		//si.dwFlags |= STARTF_USESTDHANDLES;
	}
#ifdef _DEBUG
	std::cout << "DEBUG_INFO: begin to exec command: " << k_wstr_to_str(cmd) << std::endl;
    //wcout << "DEBUG_INFO: begin to exec command: " << cmd << endl;
#endif

    //cout << "begin to exec command: " << cmd.c_str() << endl;
    bool result = CreateProcess(NULL, (LPSTR)k_wstr_to_str(cmd).c_str(), NULL, NULL, FALSE, creationFlag, NULL, k_wstr_to_str(w.native()).c_str(), &si, &m_proc_info);
    if (result)
    {
        return K_OK;
    }
    else
    {
        return K_ERR_PROCESS_OPEN_FAILURE;
    }
#else
    boost::filesystem::path p = w / (m_execname);
    vector<string> arg_list;
    arg_list.push_back(p.leaf().string());
    k_str_split(m_arguments, arg_list, " ");
    char* argv[64] = {0};

    for (int i = 0; i < arg_list.size(); i++)
    {
        argv[i] = (char*) arg_list[i].c_str();
    }
    __pid_t pid = vfork();
    if (pid < 0)
    {
		vvprint_w("fork error; failed to create process;");
        _exit(0);
    }
    else if (pid == 0)
    {
        chdir(p.parent_path().string().c_str());
        execv(p.string().c_str(), argv);
    }
    else
    {
        m_pid = pid;
		vvprint_s("process: " << m_pid << " has been created...");
        return K_OK;
    }
#endif
    return K_ERR_Not_Supported;
}
int koo_process::start_exec()
{
	if (pid() > 0)
	{
		_close_process();
	}
	int ret = _open_process();
	return ret;
}
int koo_process::wait_exec(int timeout /*= 2000*/)
{
#ifdef _WIN32
		DWORD result = WaitForSingleObject(m_proc_info.hProcess, timeout);
		if (result == WAIT_OBJECT_0)
		{
			DWORD ec = 0;
			if (GetExitCodeProcess(m_proc_info.hProcess, &ec))
			{
#ifdef _DEBUG
				std::cout << "DEBUG_INFO: Wait Proc returns " << ec << std::endl;
#endif
				if (ec == 128)
				{
					return K_OK;
				}
				return ec;
			}
			return K_OK;
		}
		else if (result == WAIT_TIMEOUT)
		{
			return K_ERR_TIMEOUT;
		}
		else
		{
			return K_ERR_UNKNOWN;
		}
#else
	return K_ERR_Not_Supported;
#endif
}

int koo_process::start_once(int timeout /*= 2000*/)
{
    _close_process();
    int ret = _open_process();
    if (ret == K_OK)
    {
		if (timeout > 0)
		{
#ifdef _WIN32
			DWORD result = WaitForSingleObject(m_proc_info.hProcess, timeout);
			if (result == WAIT_OBJECT_0)
			{
				DWORD ec = 0;
				if (GetExitCodeProcess(m_proc_info.hProcess, &ec))
				{
#ifdef _DEBUG
					std::cout << "DEBUG_INFO: KILL Proc returns " << ec << std::endl;
#endif
					if (ec == 128)
					{
						return K_OK;
					}
					return ec;
				}
				return K_OK;
			}
			ret = K_ERR_TIMEOUT;
#endif
		}
    }
    return ret;
}
#ifdef _WIN32
char kill_process_format[16] = "/F /IM %s /T";
char kill_process_by_id_format1[16] = "/PID %d /F";
char kill_process_by_id_format[64] = "cmd /c ping localhost -n %d & taskkill /PID %d /F";

extern "C" int k_kill_process(const std::string& process_name, int timeout /*= 1000*/)
{
    boost::filesystem::path cmdpath(getenv("ComSpec"));
    char kill_process_arg[64] = {0};
    std::sprintf(kill_process_arg, kill_process_format, process_name.c_str());
    koo_process vp("killtask", cmdpath.parent_path().string(), "taskkill.exe", kill_process_arg);
    return vp.start_once(timeout);
}
extern "C" int k_kill_process_by_id(int pid, int timeout /*= 0*/)
{
	boost::filesystem::path cmdpath(getenv("ComSpec"));
	char kill_process_arg[64] = { 0 };
	std::sprintf(kill_process_arg, kill_process_format, timeout + 1, pid);
	koo_process vp("killtask", cmdpath.parent_path().string(), "taskkill.exe", kill_process_arg);
	return vp.start_once(timeout);
}
#else
char kill_process_format[16] = "-1 %s";

extern "C" int k_kill_process(const std::string& process_name, int timeout /*= 1000*/)
{
    boost::filesystem::path cmdpath("/usr/bin");
    char kill_process_arg[64] = {0};
    std::sprintf(kill_process_arg, kill_process_format, process_name.c_str());
    koo_process vp("killtask", cmdpath.parent_path().string(), "killall", kill_process_arg);
    return vp.start_once(timeout);
}
extern "C" int k_kill_process_by_id(int pid, int timeout)
{
	return K_OK;
}
#endif

