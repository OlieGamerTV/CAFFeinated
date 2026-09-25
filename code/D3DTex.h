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

enum _D3DFORMAT : uint32_t
{
	D3DFMT_LE_INDEX16 = 0,
	D3DFMT_INDEX16 = 1,
	D3DFMT_LE_INDEX32 = 4,
	D3DFMT_INDEX32 = 6,
	D3DFMT_VERTEXDATA = 8,
	
	D3DFMT_UNKNOWN = 0xFF,
	D3DFMT_FORCE_DWORD = 0xFFFFFF7F,
	D3DFMT_DXT1 = 0x5201201A,
	D3DFMT_LIN_DXT1 = 0x5200201A,
	D3DFMT_DXT2 = 0x5301201A,
	D3DFMT_LIN_DXT2 = 0x5300201A,
	D3DFMT_DXT3 = 0x5301201A,
	D3DFMT_LIN_DXT3 = 0x5300201A,
	D3DFMT_DXT3A = 0x7A01201A,
	D3DFMT_LIN_DXT3A = 0x7A00201A,
	D3DFMT_DXT3A_1111 = 0x7D01201A,
	D3DFMT_LIN_DXT3A_1111 = 0x7D00201A,
	D3DFMT_DXT4 = 0x5401201A,
	D3DFMT_LIN_DXT4 = 0x5400201A,
	D3DFMT_DXT5 = 0x5401201A,
	D3DFMT_LIN_DXT5 = 0x5400201A,
	D3DFMT_DXT5A = 0x7B01201A,
	D3DFMT_LIN_DXT5A = 0x7B00201A,
	D3DFMT_DXN = 0x7101201A,
	D3DFMT_LIN_DXN = 0x7100201A,
	D3DFMT_CTX1 = 0x7C01201A,
	D3DFMT_LIN_CTX1 = 0x7C00201A,
	
	D3DFMT_A8 = 0x02019004,
	D3DFMT_LIN_A8 = 0x02009004,
	D3DFMT_L8 = 0x02010028,
	D3DFMT_LIN_L8 = 0x02000028,
	D3DFMT_R5G6B5 = 0x44012828,
	D3DFMT_LIN_R5G6B5 = 0x44002828,
	D3DFMT_R6G5B5 = 0x45012828,
	D3DFMT_LIN_R6G5B5 = 0x45002828,
	D3DFMT_L6V5U5 = 0x450B202A,
	D3DFMT_LIN_L6V5U5 = 0x450A202A,
	D3DFMT_X1R5G5B5 = 0x43012828,
	D3DFMT_LIN_X1R5G5B5 = 0x43002828,
	D3DFMT_A1R5G5B5 = 0x43012818,
	D3DFMT_LIN_A1R5G5B5 = 0x43002818,
	D3DFMT_A4R4G4B4 = 0x4F012818,
	D3DFMT_LIN_A4R4G4B4 = 0x4F002818,
	D3DFMT_X4R4G4B4 = 0x4F012828,
	D3DFMT_LIN_X4R4G4B4 = 0x4F002828,
	D3DFMT_Q4W4V4U4 = 0x4FAB201A,
	D3DFMT_LIN_Q4W4V4U4 = 0x4FAA201A,
	D3DFMT_A8L8 = 0x4A010008,
	D3DFMT_LIN_A8L8 = 0x4A000008,
	D3DFMT_G8R8 = 0x4A01202D,
	D3DFMT_LIN_G8R8 = 0x4A00202D,
	
	D3DFMT_A8R8G8B8 = 0x86012818,
	D3DFMT_LIN_A8R8G8B8 = 0x86002818,
	D3DFMT_X8R8G8B8 = 0x86012828,
	D3DFMT_LIN_X8R8G8B8 = 0x86002828,
	D3DFMT_A8B8G8R8 = 0x8601201A,
	D3DFMT_LIN_A8B8G8R8 = 0x8600201A,
};

static char* GetXenonTextureFormatName(int format) {
	char* formatName = new char[12];

	strcpy(formatName, "UNKNOWN");

	switch (format) {
	case D3DFMT_L8:
		strcpy(formatName, "L8");
		break;
	case D3DFMT_R6G5B5:
		strcpy(formatName, "RGB565");
		break;
	case D3DFMT_LIN_R6G5B5:
		strcpy(formatName, "Linear RGB565");
		break;
	case D3DFMT_A8L8:
		strcpy(formatName, "A8L8");
		break;
	case D3DFMT_LIN_A8L8:
		strcpy(formatName, "Linear A8L8");
		break;
	case D3DFMT_A4R4G4B4:
		strcpy(formatName, "ARGB4444");
		break;
	case D3DFMT_LIN_A4R4G4B4:
		strcpy(formatName, "Linear ARGB4444");
		break;
	case D3DFMT_DXT1:
		strcpy(formatName, "DXT1");
		break;
	case D3DFMT_LIN_DXT1:
		strcpy(formatName, "Linear DXT1");
		break;
	case D3DFMT_DXT3:
		strcpy(formatName, "DXT3");
		break;
	case D3DFMT_LIN_DXT3:
		strcpy(formatName, "Linear DXT3");
		break;
	case D3DFMT_DXT5:
		strcpy(formatName, "DXT5");
		break;
	case D3DFMT_LIN_DXT5:
		strcpy(formatName, "Linear DXT5");
		break;
	case D3DFMT_DXN:
		strcpy(formatName, "DXN");
		break;
	case D3DFMT_LIN_DXN:
		strcpy(formatName, "Linear DXN");
		break;
	case D3DFMT_A8R8G8B8:
		strcpy(formatName, "ARGB8888");
		break;
	case D3DFMT_LIN_A8R8G8B8:
		strcpy(formatName, "Linear ARGB8888");
		break;
	case D3DFMT_A8B8G8R8:
		strcpy(formatName, "ABGR8888");
		break;
	case D3DFMT_LIN_A8B8G8R8:
		strcpy(formatName, "Linear ABGR8888");
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
	int texDataSize = width * height;
	
	switch (texFormat)
	{
	case D3DFMT_A8L8:
	case D3DFMT_LIN_A8L8:	
		texDataSize = (width * height) * 2;
		break;
	case D3DFMT_L8:
	case D3DFMT_LIN_L8:
		texDataSize = width * height;
		break;
	case D3DFMT_DXT1:
	case D3DFMT_LIN_DXT1:
		texDataSize = (width * height) * 2;
		break;
	case D3DFMT_DXT3:
	case D3DFMT_LIN_DXT3:
	case D3DFMT_DXT5:
	case D3DFMT_LIN_DXT5:
	case D3DFMT_DXN:
	case D3DFMT_LIN_DXN:
		texDataSize = width * height;
		break;
	case D3DFMT_A8R8G8B8:
	case D3DFMT_A8B8G8R8:
	case D3DFMT_LIN_A8R8G8B8:
	case D3DFMT_LIN_A8B8G8R8:
		texDataSize = (width * height) * 4;
		break;
	case D3DFMT_A4R4G4B4:
	case D3DFMT_R5G6B5:
	case D3DFMT_R6G5B5:
	case D3DFMT_LIN_A4R4G4B4:
	case D3DFMT_LIN_R5G6B5:
	case D3DFMT_LIN_R6G5B5:
		texDataSize = (width * 2) * height;
		break;
	}

	return texDataSize;
}

/// Converts a texture to/from a linear format.
static unsigned char* ModifyLinearTexture(unsigned char* data, int width, int height, int texFormat, bool toLinear) {
	int dataSize = GetTextureDataSize(width, height, texFormat);
	unsigned char* destData = new unsigned char[dataSize];

	int blockSize = 1;
	int texelPitch = 1;
	
	switch (texFormat)
	{
	case D3DFMT_A8L8:
	case D3DFMT_LIN_A8L8:
		blockSize = 1;
		texelPitch = 2;
		break;
	case D3DFMT_L8:
	case D3DFMT_LIN_L8:
		blockSize = 1;
		texelPitch = 1;
		break;
	case D3DFMT_DXT1:
	case D3DFMT_LIN_DXT1:
		blockSize = 4;
		texelPitch = 8;
		break;
	case D3DFMT_DXT3:
	case D3DFMT_LIN_DXT3:
	case D3DFMT_DXT5:
	case D3DFMT_LIN_DXT5:
	case D3DFMT_DXN:
	case D3DFMT_LIN_DXN:
		blockSize = 4;
		texelPitch = 16;
		break;
	case D3DFMT_A8R8G8B8:
	case D3DFMT_A8B8G8R8:
	case D3DFMT_LIN_A8R8G8B8:
	case D3DFMT_LIN_A8B8G8R8:
		blockSize = 1;
		texelPitch = 4;
		break;
	case D3DFMT_A4R4G4B4:
	case D3DFMT_R5G6B5:
	case D3DFMT_R6G5B5:
	case D3DFMT_LIN_A4R4G4B4:
	case D3DFMT_LIN_R5G6B5:
	case D3DFMT_LIN_R6G5B5:
		blockSize = 1;
		texelPitch = 2;
		break;
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
			if (destOffset >= dataSize) continue;

			if (toLinear) {
				memcpy(&destData[destOffset], &data[srcOffset], texelPitch);
			}
			else {
				memcpy(&destData[srcOffset], &data[destOffset], texelPitch);
			}
		}
	}

	return destData;
}