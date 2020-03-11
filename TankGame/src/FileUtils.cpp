#include "FileUtils.h"

#include <sstream>
#include <string>

std::string loadFileAsString(const std::string & filepath)
{
	std::ifstream t(filepath);
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

bool lineReader::getline() {
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
				bufferLength = fileLength - before + remLen;
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