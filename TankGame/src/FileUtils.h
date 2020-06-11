#pragma once

#include <fstream>

std::string loadFileAsString(const std::string & filepath);

/*
	A lot faster than std::getline but also a lot less safe
	if a line in the file is longer than the buffer length then it will not be fully read //TODO fix this (maybe? it would become slower)
*/
class lineReader {
private:
	size_t before = 0;
public:
	char * lineBuffer;
	char * line;
	size_t bufferLength;
	size_t bufferOffset = 0;
	std::ifstream & file;

	size_t fileLength;
	bool last = true;

	inline char * LineEnd() const { return lineBuffer + bufferOffset; };

	//Bufferlength must be sufficient to contain the line
	lineReader(size_t bufferLength, std::ifstream & file, size_t fileLength) : bufferLength(bufferLength), lineBuffer(new char[bufferLength]), file(file), line(nullptr), fileLength(fileLength) {
		file.read(lineBuffer, bufferLength);
	};
	~lineReader() { delete[] lineBuffer; };

	bool getline();
};