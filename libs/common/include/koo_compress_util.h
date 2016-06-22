#ifndef koo_compress_util_h__
#define koo_compress_util_h__

#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <string.h>
struct koo_compress_header
{
	uint32_t magic;
	uint32_t filelength;
	char filename[504];
	koo_compress_header()
	{
		magic = 0;
		filelength = 0;
		memset(filename, 0, sizeof(filename));
	}
	bool is_valid()
	{
		return magic == 0xffffffff;
	}
};
struct koo_file_info
{
	std::string file_name;
	std::vector<uint8_t> file_data;
};
typedef std::shared_ptr<koo_file_info> koo_file_info_ptr;
int k_compress_file_to_buffer(const std::string& filename, std::vector<uint8_t>& buffer);
int k_decompress_buffer_to_file(std::vector<uint8_t>& buffer, const std::string& output);
int k_decompress_buffer(uint8_t* data, size_t size, std::vector<uint8_t>& output);

void k_traversale_dir(const std::string& rt, std::vector<std::string>& result, const std::string& filename);

int k_compress_dir(const std::string& dir, const std::string& output);
//int k_compress_path(const std::string& filename, const std::string& output);
int k_decompress_file_to_path(const std::string& compressFile, const std::string& outputPath);

int k_decompress_buffer_to_mem(std::vector<uint8_t> src, std::vector<koo_file_info_ptr>& dest);
int k_decompress_file_to_mem(const std::string& compressFile, std::vector<koo_file_info_ptr>& output);
class koo_compressor
{
public:
	int compress();
public:
	koo_compressor(const std::string& inputPath, const std::string& outputPath);
private:
	const std::string m_input_path;
	const std::string m_output_path;
	std::string m_root;
	std::vector<std::string> m_filelist;

};
#endif // koo_compress_util_h__
