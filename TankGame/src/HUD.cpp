#include "HUD.h"

/*
This code was taken from StackOverflow and modified (https://stackoverflow.com/questions/20595340/loading-a-tga-bmp-file-in-c-opengl, visited on 13th May 2020).
*/

typedef union PixelInfo
{
	std::uint32_t Colour;
	struct
	{
		std::uint8_t B, G, R, A;
	};
} *PPixelInfo;

class BMP
{
private:
	std::uint32_t width, height;
	std::uint16_t BitsPerPixel;
	std::vector<std::uint8_t> Pixels;

public:
	BMP(const char* FilePath);
	std::vector<std::uint8_t> GetPixels() const { return this->Pixels; }
	std::uint32_t GetWidth() const { return this->width; }
	std::uint32_t GetHeight() const { return this->height; }
	bool HasAlphaChannel() { return BitsPerPixel == 32; }
};

BMP::BMP(const char* FilePath)
{
	std::fstream hFile(FilePath, std::ios::in | std::ios::binary);
	try {
		if (!hFile.is_open()) {
			throw std::invalid_argument("Error: File Not Found.");
		}
	}
	catch(const std::exception& e) {
			std::cout << "Didn't open file, file not found";
		}

	try {
		hFile.seekg(0, std::ios::end);
	}
	catch(const std::exception& e) {
		std::cout << e.what();
	}
	std::size_t Length = hFile.tellg();
	hFile.seekg(0, std::ios::beg);
	std::vector<std::uint8_t> FileInfo(Length);
	hFile.read(reinterpret_cast<char*>(FileInfo.data()), 54);

	try {
		if (FileInfo[0] != 'B' && FileInfo[1] != 'M')
		{
			hFile.close();
			throw std::invalid_argument("Error: Invalid File Format. Bitmap Required.");
		}
	}
	catch (const std::exception& e) {
		std::cout << e.what();
	}
		
	try {
		if (FileInfo[28] != 24 && FileInfo[28] != 32)
		{
			hFile.close();
			throw std::invalid_argument("Error: Invalid File Format. 24 or 32 bit Image Required.");
		}
	}
	catch (const std::exception& e) {
		std::cout << e.what();
	}
		

	std::uint32_t PixelsOffset;
	std::uint32_t size;
	try {
		BitsPerPixel = FileInfo[28];
		width = FileInfo[18] + (FileInfo[19] << 8);
		height = FileInfo[22] + (FileInfo[23] << 8);
		PixelsOffset = FileInfo[10] + (FileInfo[11] << 8);
		size = ((width * BitsPerPixel + 31) / 32) * 4 * height;
		Pixels.resize(size);
		throw std::invalid_argument("");
	}
	catch(const std::exception& e) {
		std::cout << e.what();
	}
		

	hFile.seekg(PixelsOffset, std::ios::beg);
	hFile.read(reinterpret_cast<char*>(Pixels.data()), size);
	hFile.close();
}

/*
	End code from StackOverflow.
*/

void HUD::addHUD(const char* filepath)
{
	BMP textureIMG = BMP(filepath);
	this->texture2D = (GLuint)textureIMG.GetPixels().data();
}

void HUD::drawHUDquad(int width, int height)
{
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, width, height, 0.0, 1.0, 0.1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glBindTexture(GL_TEXTURE_2D, this->texture2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);
	glTexCoord2d(0.0, 1.0);
	glVertex2f(0.3, 0.05);
	glTexCoord2d(1.0, 0);
	glVertex2f(0.3, 0.15);
	glTexCoord2d(0, 0);
	glVertex2f(0.05, 0.15);
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
}
