
#include "pch.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <vector>
#include <filesystem>
#include "GeometryFile.hpp"
#include "OBJHelper.hpp"
#include "TimerBenchFromStackOverflow.hpp"

bool noPause = false;
bool minimalLog = false;

void processArgs(int argc, char ** argv);
void ConvertSingle(const char * fileIn, const char* fileOut);
void ConvertMultiple(const char* gbatFile);

int main(int argc, char ** argv){
	try {
		// First open the file to check if it's a GBAT or 10GB file
		GeometryFile testGeo(argv[1]);
		if (*testGeo.magic == 0x54414247) {
			// GBAT
			minimalLog = true;
			processArgs(argc, argv);
			ConvertMultiple(argv[1]);
		}
		else {
			// Simply Convert
			processArgs(argc, argv);
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

void processArgs(int argc, char** argv) {
	for (int i = 2; i < argc; ++i) {
		if (argv[i] == "+minimalLog") {
			minimalLog = true;
		}
		else if (argv[i] == "-minimalLog") {
			minimalLog = false;
		}
		else if (argv[i] == "+noPause") {
			noPause = true;
		}
		else if (argv[i] == "-noPause") {
			noPause = false;
		}
	}
}

void ConvertSingle(const char * fileIn, const char* fileOut) {
	std::uint64_t timeA, timeB, timeC, timeD;
	
	if(!minimalLog)
		timeA = GetTimeMs64();

	// Load .g file
	GeometryFile geoFile(fileIn);

	if (*geoFile.magic != 0x42473031) {
		throw std::string("This is not a valid .g file (invalid magic).");
	}

	if (!minimalLog)
		timeB = GetTimeMs64();

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

	if (!minimalLog)
		timeC = GetTimeMs64();

	// Save .obj file
	std::ofstream outFile;
	outFile.open((std::string(fileOut) + ".obj").c_str(), std::ofstream::binary);
	std::string outData = (verteciesStream
		+ textureStream
		+ normalsStream
		+ trianglesStream);
	outFile.write(outData.c_str(), outData.size());
	outFile.close();

	if (!minimalLog)
		timeD = GetTimeMs64();

	if (!minimalLog) {
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
}

void ConvertMultiple(const char * gbatFile) {
	// Read GBAT
	std::ifstream gbatFileStream(gbatFile);
	int index = 0;
	std::string buffer;

	std::string inDir;
	std::string outDir;
	std::vector<std::string> fileList;
	std::uint32_t threadedFileIndex = 0;
	std::uint32_t localFileIndex = 0;

	while (std::getline(gbatFileStream, buffer)) {
		if (index < 3) {
			if (index == 0) {
				if (buffer != "GBAT")
					throw std::string("This is not a valid GBAT file (Invalid magic).");
			}
			else if (index == 1) {
				inDir = buffer;
			}
			else
				outDir = buffer;
		}
		else {
			fileList.push_back(buffer);
			
			// Get dirname
			std::string fstr(outDir + '\\' + buffer);
			const char * fileOut = fstr.c_str();
			const char * path = strrchr(fileOut, '\\');
			std::string pstr(fileOut, path);
			std::wstring wname = std::wstring(pstr.begin(), pstr.end());
			struct stat info;

			// Make dir
			if(stat(pstr.c_str(), &info) != 0)
				system(std::string("mkdir \""+pstr+"\" >> NUL").c_str());
		}
		++index;
	}

	std::uint64_t totalMsStart = GetTimeMs64();

	// Make thread to do the stuff.
	std::thread multiThread([](std::string * inDir, std::string * outDir, std::vector<std::string> * fileList, std::uint32_t * threadedFileIndex) {
		for (*threadedFileIndex = 0; *threadedFileIndex < fileList->size(); ++*threadedFileIndex) {
			try {
				ConvertSingle((std::string(*inDir) + "\\" + (*fileList)[*threadedFileIndex]).c_str(), (std::string(*outDir) + "\\" + (*fileList)[*threadedFileIndex]).c_str());
			}
			catch (std::string ex) {
				std::cout << ex << std::endl;
			}
		}

	}, &inDir, &outDir, &fileList, &threadedFileIndex);
	 

	// Make thread to log the stuff.
	std::thread loggingThread([](std::uint32_t * threadedFileIndexPtr, std::uint32_t * localFileIndex, std::vector<std::string>* fileList) {
		std::uint32_t threadedFileIndex;
		std::uint64_t lastTs = GetTimeMs64();

		// When there are files converted that haven't been processed
		while (*localFileIndex < fileList->size()) {
			// and haven't been logged
			while (*localFileIndex < (threadedFileIndex = *threadedFileIndexPtr)) {
				// log them.
				std::uint64_t newTs = GetTimeMs64();
				std::cout << (*fileList)[++(*localFileIndex)] << " in " << (newTs-lastTs) << "ms." << std::endl;
				lastTs = newTs;
			}
			Sleep(1);
		}
	}, &threadedFileIndex, &localFileIndex, &fileList);

	// Wait for work to be done.
	multiThread.join();
	std::uint64_t totalMsStop = GetTimeMs64();
	loggingThread.join();

	std::cout << "Took a total of " << (totalMsStop - totalMsStart) << "ms ( " << ((totalMsStop - totalMsStart)*0.001) << "s )\n";
}
