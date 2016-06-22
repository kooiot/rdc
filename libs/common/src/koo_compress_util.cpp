#include "koo_compress_util.h"
#include "md5.h"
#include "koo_error_code.h"
#include "koo_file_util.h"
#include <boost/iostreams/filtering_stream.hpp>  
#include <boost/iostreams/copy.hpp>  
#include <boost/iostreams/filter/bzip2.hpp>  
#include <boost/iostreams/filter/zlib.hpp>  
#include <boost/iostreams/device/file_descriptor.hpp>  
#include <boost/iostreams/device/file.hpp>  
#include <boost/iostreams/device/array.hpp>  

namespace fsys = boost::filesystem;
namespace bios = boost::iostreams;
int k_compress_file_to_buffer(const std::string& filename, std::vector<uint8_t>& buffer)
{
	try
	{

		size_t len = fsys::file_size(fsys::path(filename));
		if (len == 0)
		{
			return K_OK;
		}
		std::fstream fs(filename, std::ios::binary | std::ios::in);
		if (fs.is_open())
		{
			boost::iostreams::filtering_ostream compress;
			compress.push(boost::iostreams::bzip2_compressor());
			std::stringstream ss;
			compress.push(ss);
			/**/
			bios::copy(fs, compress);
			fs.close();
			ss.seekg(0, std::ios::end);
			size_t size = ss.tellg();
			ss.seekg(0, std::ios::beg);
			buffer.resize(size);
			ss.read((char*)buffer.data(), size);
			return K_OK;
		}
		else
		{
			return K_ERR_Open_File_Failure;
		}

	}
	catch (std::exception& e)
	{
		std::cerr << "compress failed. " << e.what() << std::endl;
	}
	catch (const char* e)
	{
		std::cerr << "compress failed. " << e << std::endl;
	}
	return K_ERR_UNKNOWN;
}

int k_decompress_buffer(uint8_t* data, size_t size, std::vector<uint8_t>& output)
{
	try
	{
		if (size>0)
		{
			boost::iostreams::filtering_istream inbuf;
			inbuf.push(boost::iostreams::bzip2_decompressor());
			std::stringstream ss;
			ss.write((char*)data, size);
			inbuf.push(ss);
			std::stringstream outbuf;
			boost::iostreams::copy(inbuf, outbuf);
			outbuf.seekg(0, std::ios::end);
			size_t len = outbuf.tellp();
			outbuf.seekg(0, std::ios::beg);
			output.resize(len);
			outbuf.read((char*)output.data(), output.size());
		}
		return K_OK;
	}
	catch (std::exception& e)
	{
		std::cerr << "compress failed. " << e.what() << std::endl;
	}
	catch (const char* e)
	{
		std::cerr << "compress failed. " << e << std::endl;
	}
	return K_ERR_UNKNOWN;
}


int k_decompress_buffer_to_file(std::vector<uint8_t>& buffer, const std::string& output)
{
	try
	{
		boost::system::error_code ec;
		fsys::path p(output);
		fsys::path dir = p.remove_filename();
		if (!fsys::exists(dir))
		{
			fsys::create_directories(dir, ec);
		}
		boost::iostreams::file_sink fs(output, std::ios::binary | std::ios::out);
		if (fs.is_open())
		{
			if (buffer.size() > 0)
			{
				boost::iostreams::filtering_istream inbuf;
				inbuf.push(boost::iostreams::bzip2_decompressor());
				std::stringstream ss;
				ss.write((char*)buffer.data(), buffer.size());
				inbuf.push(ss);
				boost::iostreams::copy(inbuf, fs);
			}
			fs.close();
			return K_OK;
		}
		return K_ERR_Open_File_Failure;
	}
	catch (std::exception& e)
	{
		std::cerr << "compress failed. " << e.what() << std::endl;
	}
	catch (const char* e)
	{
		std::cerr << "compress failed. " << e << std::endl;
	}
	return K_ERR_UNKNOWN;
}
void k_traversale_dir(const std::string& rt, std::vector<std::string>& result, const std::string& filename)
{
	namespace fsys = boost::filesystem;
	boost::filesystem::path root(filename);

	if (fsys::exists(filename))
	{
		if (fsys::is_directory(root))
		{
			std::string s = filename.substr(rt.length(), filename.length() - rt.length());
			std::cout << "compress dir: " << s << std::endl;
			fsys::directory_iterator enditer;
			for (fsys::directory_iterator iter(root); iter != enditer; ++iter)
			{
				k_traversale_dir(rt, result, iter->path().string());
			}
		}
		else
		{
			/*if (rt == "")
			{
			rt = root.parent_path().string();
			}*/
			fsys::path rtp(rt);
			std::string rfn = rtp.string();
			std::string s = filename.substr(rfn.length() + 1, filename.length() - (rfn.length() + 1));
			fsys::path parent(rt);
			std::cout << "compress file: " << s << std::endl;
			result.push_back(s);
		}
	}
}


int k_compress_dir(const std::string& dir, const std::string& output)
{
	std::string root_dir = dir;
	std::vector<std::string> file_list;
	k_traversale_dir(root_dir, file_list, dir);
	if (file_list.empty())
	{
		return K_ERR_FILE_NOT_FOUND;
	}
	bios::filtering_ostream fo;
	bios::file_sink sink(output, std::ios::out | std::ios::binary);
	if (!sink.is_open())
	{
		return K_ERR_CREATE_FILE;
	}
	std::vector<uint8_t> buffer;
	MD5 md5_output;
	for (auto x : file_list)
	{
		fsys::path p(root_dir);
		p /= x;
		int ec = k_compress_file_to_buffer(p.string(), buffer);
		if (!ec)
		{
			koo_compress_header vch;
			vch.magic = 0xffffffff;
			vch.filelength = buffer.size();
			memcpy(vch.filename, x.data(), x.length());
			sink.write((char*)&vch, sizeof(vch));
			md5_output.update((char*)&vch, sizeof(vch));
			if (!buffer.empty())
			{
				sink.write((char*)buffer.data(), buffer.size());
				md5_output.update((char*)buffer.data(), buffer.size());
				buffer.resize(0);
			}
		}
		else
		{
			std::cerr << "failed to compress file: " << p.string() << "; error code = " << ec << std::endl;
		}

	}
	sink.close();
	md5_output.finalize();
	std::fstream md5file(output + ".md5", std::ios::out);
	std::string digest = md5_output.hexdigest();
	md5file.write(digest.data(), digest.length());
	md5file.close();
	return K_OK;
}
int k_compress_path(const std::string& filename, const std::string& output)
{
	std::string root = "";
	std::vector<std::string> filelist;
	k_traversale_dir(root, filelist, filename);
	if (filelist.empty())
	{
		return K_ERR_FILE_NOT_FOUND;
	}
	bios::filtering_ostream fo;
	bios::file_sink sink(output, std::ios::out | std::ios::binary);
	if (!sink.is_open())
	{
		return K_ERR_CREATE_FILE;
	}
	std::vector<uint8_t> buffer;
	buffer.reserve(0x8000000);
	MD5 md5_output;
	for (auto x : filelist)
	{
		fsys::path p = root;
		p /= x;
		int ec = k_compress_file_to_buffer(p.string(), buffer);
		if (!ec)
		{
			koo_compress_header vch;
			vch.magic = 0xffffffff;
			vch.filelength = buffer.size();
			memcpy(vch.filename, x.data(), x.length());
			sink.write((char*)&vch, sizeof(vch));
			md5_output.update((char*)&vch, sizeof(vch));
			if (!buffer.empty())
			{
				sink.write((char*)buffer.data(), buffer.size());
				md5_output.update((char*)buffer.data(), buffer.size());
				buffer.resize(0);
			}
		}
		else
		{
			std::cerr << "failed to compress file: " << p.string() << "; error code = " << ec << std::endl;
		}
		
	}
	sink.close();
	md5_output.finalize();
	std::fstream md5file(output + ".md5", std::ios::out);
	std::string digest = md5_output.hexdigest();
	md5file.write(digest.data(), digest.length());
	md5file.close();
	return K_OK;
}

int k_decompress_file_to_path(const std::string& compressFile, const std::string& outputPath)
{
	
	if (!fsys::exists(fsys::path(compressFile)))
	{
		return K_ERR_FILE_NOT_FOUND;
	}
	fsys::path output(outputPath);
	if (!fsys::exists(output))
	{
		if (!fsys::create_directory(output))
		{
			return K_ERR_CREATE_DIR;
		}
	}
	std::fstream fs(compressFile, std::ios::binary | std::ios::in);
	while (!fs.eof())
	{
		koo_compress_header vch;
		fs.read((char*)&vch, sizeof(vch));
		if (vch.is_valid())
		{
			std::vector<uint8_t> temp(vch.filelength);
			if (vch.filelength > 0)
			{
				fs.read((char*)temp.data(), temp.size());
			}
			std::string fn(vch.filename);
			fsys::path p = output / fn;
			k_decompress_buffer_to_file(temp, p.string());
		}
	}
	return K_OK;
}

int k_decompress_buffer_to_mem(std::vector<uint8_t> src, std::vector<koo_file_info_ptr>& dest)
{
	if (src.empty())
	{
		return K_OK;
	}
	uint8_t* buffer = src.data();
	int size = src.size();
	int vch_size = sizeof(koo_compress_header);
	while (size>0)
	{
		koo_compress_header* vch;
		vch = (koo_compress_header*)buffer;
		if (vch->is_valid())
		{
			buffer += vch_size;
			size -= vch_size;
			koo_file_info_ptr vfi(new koo_file_info);
			vfi->file_name = std::string(vch->filename);
			if (vch->filelength > 0)
			{
				k_decompress_buffer(buffer, vch->filelength, vfi->file_data);
				size -= vch->filelength;
				buffer += vch->filelength;
			}
			dest.push_back(vfi);
		}
	}
	return K_OK;
}

int k_decompress_file_to_mem(const std::string& compressFile, std::vector<koo_file_info_ptr>& output)
{
	if (!fsys::exists(fsys::path(compressFile)))
	{
		return K_ERR_FILE_NOT_FOUND;
	}
	std::fstream fs(compressFile, std::ios::binary | std::ios::in);
	while (!fs.eof())
	{
		koo_compress_header vch;
		fs.read((char*)&vch, sizeof(vch));
		if (vch.is_valid())
		{
			std::vector<uint8_t> temp(vch.filelength);
			if (vch.filelength > 0)
			{
				fs.read((char*)temp.data(), temp.size());
			}
			koo_file_info_ptr vfi(new koo_file_info);
			vfi->file_name = std::string(vch.filename);
			if (!k_decompress_buffer(temp.data(), temp.size(), vfi->file_data))
			{
				output.push_back(vfi);
			}
		}
	}
	return K_OK;
}


koo_compressor::koo_compressor(const std::string& inputPath, const std::string& outputPath) : m_input_path(inputPath), m_output_path(outputPath), m_root("")
{

}

int koo_compressor::compress()
{
	k_traversale_dir(m_root, m_filelist, m_input_path);
	if (!m_filelist.empty())
	{
		bios::filtering_ostream fo;
		bios::file_sink sink(m_output_path, std::ios::out | std::ios::binary);
		if (sink.is_open())
		{
			std::vector<uint8_t> buffer;
			for (auto x : m_filelist)
			{
				fsys::path p = m_root;
				p /= x;
				int ret = k_compress_file_to_buffer(p.string(), buffer);
				if (ret == K_OK)
				{
					koo_compress_header vch;
					memcpy(vch.filename, x.data(), x.length());
					vch.filelength = buffer.size();
					sink.write((char*)&vch, sizeof(vch));
					sink.write((char*)buffer.data(), buffer.size());
				}
			}
			sink.close();
		}
	}
	return K_OK;
}
