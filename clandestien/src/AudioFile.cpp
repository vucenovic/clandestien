#include "AudioFile.h"
#include <iostream>
#include <fstream>

/*
//this is a working example of playing a piece of audio with OpenAL (use for reference for integrating into engine)
int main()
{
    std::cout << "Hello World!\n";
	
	ALCdevice *audioDevice;
	audioDevice = alcOpenDevice(NULL);
	if (!audioDevice) {
		//Do stuffs if no worky
	}

	ALCcontext *context;

	context = alcCreateContext(audioDevice, NULL);
	if (!alcMakeContextCurrent(context)) {
		//stuff no worky
	}

	ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

	alListener3f(AL_POSITION, 0, 0, 0);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	alListenerfv(AL_ORIENTATION, listenerOri);

	ALuint source;

	alGenSources(1, &source);

	alSourcef(source, AL_PITCH, 1);
	alSourcef(source, AL_GAIN, 1);
	alSource3f(source, AL_POSITION, 0, 0, 0);
	alSource3f(source, AL_VELOCITY, 0, 0, 0);
	alSourcei(source, AL_LOOPING, AL_FALSE);
	alSourcei(source, AL_SOURCE_RELATIVE, 1);

	ALuint buffer;

	ALuint dataSize = 10000;
	ALuint dataFreq = 1000;
	ALubyte * data = new ALubyte[dataSize];

	for (size_t i = 0; i < dataSize; i++) {
		//data[i] = (i % 4 * 2 - 1) * 20 + 128;
		data[i] = std::rand() % 255;
	}

	AudioFile myAudio("res/waha.wav");

	alGenBuffers(1, &buffer);
	alBufferData(buffer, myAudio.format, myAudio.data, myAudio.size, myAudio.frequency);

	alSourcei(source, AL_BUFFER, buffer);

	alSourcePlay(source);

	ALint source_state;
	alGetSourcei(source, AL_SOURCE_STATE, &source_state);
	// check for errors
	while (source_state == AL_PLAYING) {
		alGetSourcei(source, AL_SOURCE_STATE, &source_state);
	}

	// cleanup context
	delete[] data;
	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);
	audioDevice = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(audioDevice);
}
*/

AudioFile::AudioFile(const std::string filePath) : size(0), data(nullptr), frequency(0), format(0)
{
	using namespace std;
	std::ifstream file(filePath, ios::in | ios::binary | ios::ate);
	if (file.is_open()) {
		{
			size_t totalFileSize = file.tellg();
			file.seekg(0, ios::beg);

			if (totalFileSize < 16) {
				goto ERROR_SKIP;
			}

			unsigned char headerData[12];

			file.read((char *)headerData, 4);
			if (memcmp(headerData, "RIFF", 4) != 0) {
				goto ERROR_SKIP; //Not a RIFF file
			}

			file.read((char *)headerData + 4, 8);
			unsigned int fileSize = (headerData[4] | headerData[5] << 8 | headerData[6] << 16 | headerData[7] << 24) + 8;

			if (memcmp(headerData + 8, "WAVE", 4) != 0) {
				goto ERROR_SKIP; //Not a WAVE file
			}

			unsigned char fmtData[24];
			file.read((char *)fmtData, 24);
			unsigned short formatIndex = fmtData[8] | fmtData[9] << 8;
			unsigned short channelCount = fmtData[10] | fmtData[11] << 8;
			frequency = fmtData[12] | fmtData[13] << 8 | fmtData[14] << 16 | fmtData[15] << 24;
			unsigned short bitSize = fmtData[22] | fmtData[23] << 8;

			if (formatIndex != 1) {
				goto ERROR_SKIP; //not in PCM format
			}

			if (bitSize != 16) {
				goto ERROR_SKIP; //currently not supported
			}

			switch (channelCount) {
			case 1:
				format = AL_FORMAT_MONO16;
				break;
			case 2:
				format = AL_FORMAT_STEREO16;
				break;
			default:
				goto ERROR_SKIP; //unsupported Channel count
			}

			unsigned char dataHeader[8];
			file.read((char *)dataHeader, 8);
			size = dataHeader[4] | dataHeader[5] << 8 | dataHeader[6] << 16 | dataHeader[7] << 24;
			data = new ALshort[size];
			file.read((char *)data, size);
		}

	ERROR_SKIP:
		file.close();
	}
}

AudioFile::~AudioFile()
{
	if (data) delete[] data;
}