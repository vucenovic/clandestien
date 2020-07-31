#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>

class DDSImage {
public:
	unsigned char* data;
	unsigned int width;
	unsigned int height;
	unsigned int size;
	GLenum format;

	DDSImage() : data(nullptr), width(0), height(0), size(0), format(GL_NONE) {}
	DDSImage(const DDSImage& img) = delete;
	DDSImage(DDSImage&& img) : data(img.data), width(img.width), height(img.height), size(img.size), format(img.format) {
		img.data = nullptr;
	}
	DDSImage& operator=(const DDSImage& img) = delete;
	DDSImage& operator=(DDSImage&& img) {
		data = img.data;
		img.data = nullptr;
		width = img.width;
		height = img.height;
		size = img.size;
		format = img.format;
		return *this;
	};

	~DDSImage() {
		if (data != nullptr) {
			delete[] data;
			data = nullptr;
		}
	}

	static DDSImage loadDDS(const char* file);
};