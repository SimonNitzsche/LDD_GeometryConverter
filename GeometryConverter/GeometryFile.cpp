#include "pch.h"
#include "GeometryFile.hpp"
#include <fstream>
#include <vector>
#include <iostream>
#include "FileUtils.hpp"

GeometryFile::GeometryFile(const char * filepath) {
	if (filepath == nullptr) filepath = "";

	this->fileData = FileUtils::ReadFileCompletely(std::string(filepath));

	this->magic = reinterpret_cast<std::uint32_t*>(fileData.get() + 0);
	this->vertexCount = reinterpret_cast<std::uint32_t*>(fileData.get() + 4);
	this->indexCount = reinterpret_cast<std::uint32_t*>(fileData.get() + 8);
	this->options = reinterpret_cast<std::uint32_t*>(fileData.get() + 12);

	this->vertexCoordinates = reinterpret_cast<XYZ*>(fileData.get() + 16);

	unsigned char * filePosition = reinterpret_cast<unsigned char*>(this->vertexCoordinates + *this->vertexCount);
	
	if ((*this->options & 0x02) != 0) {
		this->vertexNormals = reinterpret_cast<XYZ*>(filePosition);
		filePosition = reinterpret_cast<unsigned char*>(this->vertexNormals + *this->vertexCount);
	}

	if ((*this->options & 0x01) != 0) {
		this->textureCoordinates = reinterpret_cast<UV*>(filePosition);
		filePosition = reinterpret_cast<unsigned char*>(this->textureCoordinates + *this->vertexCount);
	}

	this->triangles = reinterpret_cast<std::uint32_t *>(filePosition);
}


GeometryFile::~GeometryFile()
{
}
