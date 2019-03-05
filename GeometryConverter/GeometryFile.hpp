#pragma once
#ifndef __GEOMETRYFILE_HPP__
#define __GEOMETRYFILE_HPP__

#include <memory>
#include <stdlib.h>

struct XYZ;
struct UV;

class GeometryFile
{
private:
	std::unique_ptr<unsigned char[]> fileData;

public:
	std::uint32_t * magic;
	std::uint32_t * vertexCount;
	std::uint32_t * indexCount;
	std::uint32_t * options;

public:
	XYZ * vertexCoordinates;
public:
	XYZ * vertexNormals;
public:
	UV * textureCoordinates;
public:
	std::uint32_t * triangles;

public:
	GeometryFile(const char* filepath);
	~GeometryFile();
};

struct XYZ {
public:
	float x;
	float y;
	float z;
};

struct UV{
public:
	float u;
	float v;
};

#endif // !__GEOMETRYFILE_HPP__