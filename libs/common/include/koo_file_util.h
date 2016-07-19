#ifndef koo_file_util_h__
#define koo_file_util_h__

#pragma once
#ifdef __linux
#ifndef BOOST_NO_CXX11_SCOPED_ENUMS
#define BOOST_NO_CXX11_SCOPED_ENUMS
#endif
#endif
#include <boost/filesystem.hpp>
#include "pre_boost_basic.h"
#include <string>
namespace fsys = boost::filesystem;
#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
inline int k_file_chmod(const std::string& target, int mod)
{
    boost::system::error_code ec;
    fsys::path p(target);
    
    //if(fsys::exists(p, ec))
    {
        int error_code = chmod(p.string().c_str(), mod);
        // S_ISUID | S_ISGID | S_ISVTX | S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IXOTH);
        if (error_code != 0) 
        {
            printf("%s%d\n", "chomd() failed; error code = ", error_code);
        }
        return error_code;
    }
    return 0;
}
#endif
inline int k_file_get_sub_dir(const fsys::path& dir, std::vector<fsys::path>& subs)
{
	boost_error_code ec;
	fsys::path p(dir);
	if (fsys::exists(p, ec) && fsys::is_directory(p, ec))
	{
		fsys::directory_iterator enditer;
		int count = 0;
		try
		{
			for (fsys::directory_iterator iter(p); iter != enditer; ++iter)
			{
				auto pp = iter->path();
				if (fsys::is_directory(pp, ec))
				{
					subs.push_back(pp);
					count++;
				}
			}
		}
		catch (std::exception& e)
		{
			std::cerr << __FUNCTION__ << " Exception " << e.what() << std::endl;
		}
		catch (const char* e)
		{
			std::cerr << __FUNCTION__ << " Exception " << e << std::endl;
		}
		return count;
	}
	return 0;
}
inline int k_file_get_sub_path(const fsys::path& dir, std::vector<fsys::path>& subs)
{
	fsys::path p(dir);
	if (fsys::exists(p) && fsys::is_directory(p))
	{
		fsys::directory_iterator enditer;
		int count = 0;
		try
		{
			for (fsys::directory_iterator iter(p); iter != enditer; ++iter)
			{
				auto pp = iter->path();
				subs.push_back(pp);
				count++;
			}
		}
		catch (std::exception& e)
		{
			std::cerr << __FUNCTION__ << " Exception " << e.what() << std::endl;
		}
		catch (const char* e)
		{
			std::cerr << __FUNCTION__ << " Exception " << e << std::endl;
		}
		return count;
	}
	return 0;
}

class koo_file_stream
{
	FILE* m_file = 0;
public:
	koo_file_stream(const std::string& fn)
	{

	}
	koo_file_stream(const fsys::path p)
	{

	}
};
#endif // koo_file_util_h__
