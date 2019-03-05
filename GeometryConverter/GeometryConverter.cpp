
#include "pch.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include "GeometryFile.hpp"
#include "OBJHelper.hpp"
#include "TimerBenchFromStackOverflow.hpp"

bool noPause = false;

void ConvertSingle(const char * fileIn, const char* fileOut);

int main(int argc, char ** argv){

	try {
		// First open the file to check if it's a GBAT or 10GB file
		GeometryFile testGeo(argv[1]);
		if (*testGeo.magic == 0x54414247) {
			// GBAT
			throw std::string("GBAT is unsupported right now.");
		}
		else {
			// Simply Convert
			ConvertSingle(argv[1], argv[1]);
		}

		
	}
	catch (std::string ex){
		std::cout << ex << std::endl;
	}
	// Wait for any key press
	if(!noPause)
		system("pause");
}


void ConvertSingle(const char * fileIn, const char* fileOut) {
	std::uint64_t timeA = GetTimeMs64();

	// Load .g file
	GeometryFile geoFile(fileIn);

	if (*geoFile.magic != 0x42473031) {
		throw std::string("This is not a valid .g file (invalid magic).");
	}

	std::uint64_t timeB = GetTimeMs64();

	// Generate .obj file
	std::string verteciesStream;
	std::thread verteciesThread(GenerateOBJVertecies, &verteciesStream, &geoFile);

	std::string textureStream;
	std::thread textureThread(GenerateOBJTextureUV, &textureStream, &geoFile);

	std::string normalsStream;
	std::thread normalsThread(GenerateOBJNormals, &normalsStream, &geoFile);

	std::string trianglesStream;
	std::thread trianglesThread(GenerateOBJTriangles, &trianglesStream, &geoFile);

	// Wait for .obj file generation
	verteciesThread.join();
	textureThread.join();
	normalsThread.join();
	trianglesThread.join();

	std::uint64_t timeC = GetTimeMs64();

	// Save .obj file
	std::ofstream outFile;
	outFile.open((std::string(fileOut) + ".obj").c_str());
	outFile
		<< verteciesStream
		+ textureStream
		+ normalsStream
		+ trianglesStream;
	outFile.close();

	std::uint64_t timeD = GetTimeMs64();

	// Calculate benchmark
	timeD = timeD - timeC;
	timeC = timeC - timeB;
	timeB = timeB - timeA;
	timeA = timeB + timeC + timeD;

	// Log
	std::cout << "Took a total of " << timeA << "ms.\n";
	std::cout << "Reading .g file: " << timeB << "ms.\n";
	std::cout << "Generating .obj file: " << timeC << "ms.\n";
	std::cout << "Saving .obj file: " << timeD << "ms.\n";
}
