#pragma once

#include <AL\al.h>
#include <AL\alc.h>
#include <string>
#include <memory>

class AudioFile {
public:
	ALshort * data;
	ALuint size;
	ALuint frequency;
	ALenum format;

	AudioFile(const AudioFile & o) = delete; // delete copy
	~AudioFile();

	AudioFile(const std::string filePath);
	//TODO: add way to generate AudioFile from code
};