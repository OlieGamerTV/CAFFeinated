#pragma once
#include <memory.h>

const int HASFRAMES_FLAG = 0x4;

// Code for this was taken and converted from https://github.com/NCDyson/RareView/blob/master/RareView/Texture.cs
enum TEXTURE_FORMAT : int {
	TEX_L8 = 2,
	TEX_RGB565 = 0x44,
	TEX_A8L8 = 0x4A,
	TEX_XRGB4444 = 0x4F,
	TEX_DXT1 = 0x52,
	TEX_DXT3 = 0x53,
	TEX_DXT5 = 0x54,
	TEX_DXTN = 0x71,
	TEX_ARGB8888 = 0x86
};

static char* GetXenonTextureFormatName(int format) {
	char* formatName = new char[12];

	switch (format) {
	case TEXTURE_FORMAT::TEX_L8:
		strcpy(formatName, "L8");
		break;
	case TEXTURE_FORMAT::TEX_RGB565:
		strcpy(formatName, "RGB565");
		break;
	case TEXTURE_FORMAT::TEX_A8L8:
		strcpy(formatName, "A8L8");
		break;
	case TEXTURE_FORMAT::TEX_XRGB4444:
		strcpy(formatName, "XRGB4444");
		break;
	case TEXTURE_FORMAT::TEX_DXT1:
		strcpy(formatName, "DXT1");
		break;
	case TEXTURE_FORMAT::TEX_DXT3:
		strcpy(formatName, "DXT3");
		break;
	case TEXTURE_FORMAT::TEX_DXT5:
		strcpy(formatName, "DXT5");
		break;
	case TEXTURE_FORMAT::TEX_DXTN:
		strcpy(formatName, "DXTN");
		break;
	case TEXTURE_FORMAT::TEX_ARGB8888:
		strcpy(formatName, "ARGB8888");
		break;
	}

	return formatName;
}

static int XGAddress2DTiledX(int Offset, int Width, int TexelPitch)
{
	int AlignedWidth = (Width + 31) & ~31;

	int LogBpp = (TexelPitch >> 2) + ((TexelPitch >> 1) >> (TexelPitch >> 2));
	int OffsetB = Offset << LogBpp;
	int OffsetT = ((OffsetB & ~4095) >> 3) + ((OffsetB & 1792) >> 2) + (OffsetB & 63);
	int OffsetM = OffsetT >> (7 + LogBpp);

	int MacroX = ((OffsetM % (AlignedWidth >> 5)) << 2);
	int Tile = ((((OffsetT >> (5 + LogBpp)) & 2) + (OffsetB >> 6)) & 3);
	int Macro = (MacroX + Tile) << 3;
	int Micro = ((((OffsetT >> 1) & ~15) + (OffsetT & 15)) & ((TexelPitch << 3) - 1)) >> LogBpp;

	return Macro + Micro;
}

static int XGAddress2DTiledY(int Offset, int Width, int TexelPitch)
{
	int AlignedWidth = (Width + 31) & ~31;

	int LogBpp = (TexelPitch >> 2) + ((TexelPitch >> 1) >> (TexelPitch >> 2));
	int OffsetB = Offset << LogBpp;
	int OffsetT = ((OffsetB & ~4095) >> 3) + ((OffsetB & 1792) >> 2) + (OffsetB & 63);
	int OffsetM = OffsetT >> (7 + LogBpp);

	int MacroY = ((OffsetM / (AlignedWidth >> 5)) << 2);
	int Tile = ((OffsetT >> (6 + LogBpp)) & 1) + (((OffsetB & 2048) >> 10));
	int Macro = (MacroY + Tile) << 3;
	int Micro = ((((OffsetT & (((TexelPitch << 6) - 1) & ~31)) + ((OffsetT & 15) << 1)) >> (3 + LogBpp)) & ~1);

	return Macro + Micro + ((OffsetT & 16) >> 4);
}

static int GetTextureDataSize(int width, int height, int texFormat) {
	int texDataSize = 0;

	if (texFormat == TEXTURE_FORMAT::TEX_A8L8) {
		texDataSize = (width * height) * 2;
	}
	if (texFormat == TEXTURE_FORMAT::TEX_L8) {
		texDataSize = width * height;
	}
	if (texFormat == TEXTURE_FORMAT::TEX_DXT1) {
		texDataSize = (width * height) * 2;
	}
	if (texFormat == TEXTURE_FORMAT::TEX_DXT3 || texFormat == TEXTURE_FORMAT::TEX_DXT5 || texFormat == TEXTURE_FORMAT::TEX_DXTN) {
		texDataSize = width * height;
	}
	if (texFormat == TEXTURE_FORMAT::TEX_ARGB8888) {
		texDataSize = (width * height) * 4;
	}
	if (texFormat == TEXTURE_FORMAT::TEX_XRGB4444 || texFormat == TEXTURE_FORMAT::TEX_RGB565) {
		texDataSize = (width * 2) * height;
	}

	return texDataSize;
}

static unsigned char* ModifyLinearTexture(unsigned char* data, int width, int height, int texFormat, bool toLinear) {
	int dataSize = GetTextureDataSize(width, height, texFormat);
	unsigned char* destData = new unsigned char[dataSize];

	int blockSize;
	int texelPitch;

	if (texFormat == TEXTURE_FORMAT::TEX_L8) {
		blockSize = 1;
		texelPitch = 1;
	}
	if (texFormat == TEXTURE_FORMAT::TEX_A8L8 || texFormat == TEXTURE_FORMAT::TEX_XRGB4444 || texFormat == TEXTURE_FORMAT::TEX_RGB565) {
		blockSize = 1;
		texelPitch = 2;
	}
	if (texFormat == TEXTURE_FORMAT::TEX_ARGB8888) {
		blockSize = 1;
		texelPitch = 4;
	}
	if (texFormat == TEXTURE_FORMAT::TEX_DXT1) {
		blockSize = 4;
		texelPitch = 8;
	}
	if (texFormat == TEXTURE_FORMAT::TEX_DXT3 || texFormat == TEXTURE_FORMAT::TEX_DXT5 || texFormat == TEXTURE_FORMAT::TEX_DXTN) {
		blockSize = 4;
		texelPitch = 16;
	}

	int blockWidth = width / blockSize;
	int blockHeight = height / blockSize;

	for (int j = 0; j < blockHeight; j++)
	{
		for (int i = 0; i < blockWidth; i++)
		{
			int blockOffset = j * blockWidth + i;

			int x = XGAddress2DTiledX(blockOffset, blockWidth, texelPitch);
			int y = XGAddress2DTiledY(blockOffset, blockWidth, texelPitch);

			int srcOffset = j * blockWidth * texelPitch + i * texelPitch;
			int destOffset = y * blockWidth * texelPitch + x * texelPitch;
			//TODO: ConvertToLinearTexture apparently breaks on on textures with a height of 64...
			//if (destOffset >= dataSize) continue;

			if (toLinear) {
				memcpy(destData + destOffset, data + srcOffset, texelPitch);
			}
			else {
				memcpy(destData + srcOffset, data + destOffset, texelPitch);
			}
		}
	}

	return destData;
}