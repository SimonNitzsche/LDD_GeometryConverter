#ifndef __UTILS_FILEUTILS_HPP_
#define __UTILS_FILEUTILS_HPP_
#include <iostream>

#include <cstdio>
#include <string>
#include <memory>

namespace FileUtils {
	inline std::unique_ptr<unsigned char[]> ReadFileCompletely(std::string filename, uint32_t * fsize = 0) {
		FILE * file;
		fopen_s(&file, filename.c_str(), "r+b");
		if (file == nullptr) {
			throw std::string("Couldn't load file \"" + filename + "\"");
		}
		fseek(file, 0, SEEK_END);
		long int size = ftell(file);
		fclose(file);
		// Reading data to array of unsigned chars
		fopen_s(&file, filename.c_str(), "r+b");
		std::unique_ptr<unsigned char[]> data = std::make_unique<unsigned char[]>(size);
		int bytes_read = fread(data.get(), sizeof(unsigned char), size, file);

		// Cleanup
		fclose(file);

		// return
		*fsize = bytes_read;
		return data;
	}
};

#endif // !__UTILS_FILEUTILS_HPP_