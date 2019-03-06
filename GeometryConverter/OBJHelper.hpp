#pragma once
#ifndef __OBJ_HELPER_HPP__
#define __OBJ_HELPER_HPP__

#include <sstream>
#include "GeometryFile.hpp"

void GenerateOBJVertecies(std::string * text, GeometryFile * geoFile) {
	// Vertecies
	std::uint32_t vertexCount = *geoFile->vertexCount;
	for (int i = 0; i < vertexCount; ++i) {
		XYZ * xyz = reinterpret_cast<XYZ*>(geoFile->vertexCoordinates + i);
		*text
			+="v "
			+ std::to_string(xyz->x) + " "
			+ std::to_string(xyz->y) + " "
			+ std::to_string(xyz->z) + "\n";
	}
}

void GenerateOBJTextureUV(std::string * text, GeometryFile * geoFile) {
	// Texture UV
	if (geoFile->textureCoordinates != nullptr) {
		std::uint32_t vertexCount = *geoFile->vertexCount;
		for (int i = 0; i < vertexCount; ++i) {
			UV * uv = reinterpret_cast<UV*>(geoFile->textureCoordinates + i);
			*text
				+="vt "
				+ std::to_string(uv->u) + " "
				+ std::to_string(uv->v) + "\n";
		}
	}
}

void GenerateOBJNormals(std::string * text, GeometryFile * geoFile) {
	// Normals
	if (geoFile->vertexNormals != nullptr) {
		std::uint32_t vertexCount = *geoFile->vertexCount;
		for (int i = 0; i < vertexCount; ++i) {
			XYZ * xyz = reinterpret_cast<XYZ*>(geoFile->vertexNormals + i);
			*text
				+="vn "
				+ std::to_string(xyz->x) + " "
				+ std::to_string(xyz->y) + " "
				+ std::to_string(xyz->z) + "\n";
		}
	}
}

void GenerateOBJTriangles(std::string * text, GeometryFile * geoFile) {
	// Combine
	std::uint32_t indexCount = *geoFile->indexCount;
	for (int i = 0; i < indexCount; i += 3) {
		std::uint32_t * triangles = geoFile->triangles + i;
		*text
			+="f "
			+ std::to_string(*reinterpret_cast<std::uint32_t*>(triangles    ) + 1) + " "
			+ std::to_string(*reinterpret_cast<std::uint32_t*>(triangles + 1) + 1) + " "
			+ std::to_string(*reinterpret_cast<std::uint32_t*>(triangles + 2) + 1) + "\n";
	}
}


#endif // !__OBJ_HELPER_HPP__
