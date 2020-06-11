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

	long nextBreak = bufferOffset;
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
			long remLen = bufferLength - bufferOffset;
			memmove(lineBuffer, line, remLen);
			file.read(lineBuffer + remLen, bufferOffset);
			before += bufferOffset;
			if (file.eof()) {
				bufferLength = fileLength - before;
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