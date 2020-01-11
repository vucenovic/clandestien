#include "DDSImage.h"

#include <iostream>
#include <fstream>
#include <vector>

/*
Heavily modified version of https://gist.github.com/tilkinsc/13191c0c1e5d6b25fbe79bbd2288a673

File Structure:
  Section     Length
  ///////////////////
  FILECODE    4
  HEADER      124
  HEADER_DX10* 20	(https://msdn.microsoft.com/en-us/library/bb943983(v=vs.85).aspx)
  PIXELS      fseek(f, 0, SEEK_END); (ftell(f) - 128) - (fourCC == "DX10" ? 17 or 20 : 0)
* the link tells you that this section isn't written unless its a DX10 file
Supports DXT1, DXT3, DXT5.
The problem with supporting DX10 is you need to know what it is used for and how opengl would use it.
File Byte Order:
typedef unsigned int DWORD;           // 32bits little endian
  type   index    attribute           // description
///////////////////////////////////////////////////////////////////////////////////////////////
  DWORD  0        file_code;          //. always `DDS `, or 0x20534444
  DWORD  4        size;               //. size of the header, always 124 (includes PIXELFORMAT)
  DWORD  8        flags;              //. bitflags that tells you if data is present in the file
									  //      CAPS         0x1
									  //      HEIGHT       0x2
									  //      WIDTH        0x4
									  //      PITCH        0x8
									  //      PIXELFORMAT  0x1000
									  //      MIPMAPCOUNT  0x20000
									  //      LINEARSIZE   0x80000
									  //      DEPTH        0x800000
  DWORD  12       height;             //. height of the base image (biggest mipmap)
  DWORD  16       width;              //. width of the base image (biggest mipmap)
  DWORD  20       pitchOrLinearSize;  //. bytes per scan line in an uncompressed texture, or bytes in the top level texture for a compressed texture
									  //     D3DX11.lib and other similar libraries unreliably or inconsistently provide the pitch, convert with
									  //     DX* && BC*: max( 1, ((width+3)/4) ) * block-size
									  //     *8*8_*8*8 && UYVY && YUY2: ((width+1) >> 1) * 4
									  //     (width * bits-per-pixel + 7)/8 (divide by 8 for byte alignment, whatever that means)
  DWORD  24       depth;              //. Depth of a volume texture (in pixels), garbage if no volume data
  DWORD  28       mipMapCount;        //. number of mipmaps, garbage if no pixel data
  DWORD  32       reserved1[11];      //. unused
  DWORD  76       Size;               //. size of the following 32 bytes (PIXELFORMAT)
  DWORD  80       Flags;              //. bitflags that tells you if data is present in the file for following 28 bytes
									  //      ALPHAPIXELS  0x1
									  //      ALPHA        0x2
									  //      FOURCC       0x4
									  //      RGB          0x40
									  //      YUV          0x200
									  //      LUMINANCE    0x20000
  DWORD  84       FourCC;             //. File format: DXT1, DXT2, DXT3, DXT4, DXT5, DX10.
  DWORD  88       RGBBitCount;        //. Bits per pixel
  DWORD  92       RBitMask;           //. Bit mask for R channel
  DWORD  96       GBitMask;           //. Bit mask for G channel
  DWORD  100      BBitMask;           //. Bit mask for B channel
  DWORD  104      ABitMask;           //. Bit mask for A channel
  DWORD  108      caps;               //. 0x1000 for a texture w/o mipmaps
									  //      0x401008 for a texture w/ mipmaps
									  //      0x1008 for a cube map
  DWORD  112      caps2;              //. bitflags that tells you if data is present in the file
									  //      CUBEMAP           0x200     Required for a cube map.
									  //      CUBEMAP_POSITIVEX 0x400     Required when these surfaces are stored in a cube map.
									  //      CUBEMAP_NEGATIVEX 0x800     ^
									  //      CUBEMAP_POSITIVEY 0x1000    ^
									  //      CUBEMAP_NEGATIVEY 0x2000    ^
									  //      CUBEMAP_POSITIVEZ 0x4000    ^
									  //      CUBEMAP_NEGATIVEZ 0x8000    ^
									  //      VOLUME            0x200000  Required for a volume texture.
  DWORD  114      caps3;              //. unused
  DWORD  116      caps4;              //. unused
  DWORD  120      reserved2;          //. unused
*/
DDSImage DDSImage::loadDDS(const char * path)
{
	using namespace std;

	DDSImage ret = DDSImage();

	unsigned char* header = nullptr;
	unsigned int blockSize;

	GLuint tid = 0;

	// open the DDS file for binary reading and get file size
	ifstream file(path, ios::in | ios::binary | ios::ate);
	if (file.is_open()) {
		size_t file_size = file.tellg();
		file.seekg(0, ios::beg);

		header = new unsigned char[128];
		file.read((char*)header, 128);

		// compare the `DDS ` signature
		if (memcmp(header, "DDS ", 4) != 0)
			goto ERROR_SKIP;

		// extract height, width, and amount of mipmaps - yes it is stored height then width
		ret.height = (header[12]) | (header[13] << 8) | (header[14] << 16) | (header[15] << 24);
		ret.width = (header[16]) | (header[17] << 8) | (header[18] << 16) | (header[19] << 24);
		//mipMapCount = (header[28]) | (header[29] << 8) | (header[30] << 16) | (header[31] << 24);

		// figure out what format to use for what fourCC file type it is
		// block size is about physical chunk storage of compressed data in file (important)
		if (header[84] == 'D') {
			switch (header[87]) {
			case '1': // DXT1
				ret.format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				blockSize = 8;
				break;
			case '3': // DXT3
				ret.format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				blockSize = 16;
				break;
			case '5': // DXT5
				ret.format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				blockSize = 16;
				break;
			case '0': // DX10
				// unsupported, else will error
				// as it adds sizeof(struct DDS_HEADER_DXT10) between pixels
				// so, buffer = malloc((file_size - 128) - sizeof(struct DDS_HEADER_DXT10));
			default: goto ERROR_SKIP;
			}
		}
		else // BC4U/BC4S/ATI2/BC55/R8G8_B8G8/G8R8_G8B8/UYVY-packed/YUY2-packed unsupported
			goto ERROR_SKIP;

		// allocate new unsigned char space with file_size - (file_code + header_size) magnitude
		// read rest of file
		ret.size = ((ret.width + 3) / 4) * ((ret.height + 3) / 4) * blockSize;
		ret.data = new unsigned char[file_size - 128];
		file.read((char*)ret.data, file_size - 128);
	}
ERROR_SKIP:
	delete[] header;
	return ret;
}