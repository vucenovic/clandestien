#pragma once

#include <fstream>

/*
	A lot faster than std::getline but also a lot less safe
	if a line in the file is longer than the buffer length then it will not be fully read //TODO fix this (maybe? it would become slower)
*/
class lineReader {
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

	bool getline() {
		line = lineBuffer + bufferOffset;
		bool notFound = true;

		size_t nextBreak = bufferOffset;
		for (int i = bufferOffset; i < bufferLength; i++) {
			if (lineBuffer[i] == '\n') {
				nextBreak = i;
				notFound = false;
				break;
			}
		}
		if (notFound) { //no endl found
			if (file.eof()) {
				if (last) {
					bufferOffset = bufferLength;
					last = false;
					return true;
				}
				return false;
			}
			else {
				size_t remLen = bufferLength - bufferOffset;
				memmove(lineBuffer, line, remLen);
				size_t before = file.tellg();
				file.read(lineBuffer + remLen, bufferOffset);
				if (file.eof()) {
					bufferLength = fileLength - before + remLen; //Return end of line on last line instead of beginning (TODO FIX)
				}
				bufferOffset = 0;
				return getline(); //easiest way of doing this
			}
		}
		else {
			bufferOffset = nextBreak + 1;
			return true;
		}
	}
};