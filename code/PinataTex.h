#pragma once
#include "D3DTex.h"

namespace Pinata {
	static const char* dbTextureNameList[] = {
	"UNKNOWN",
	"DXT1",
	"DXT3",
	"DXT5",
	"A8R8G8B8",
	"X8R8G8B8",
	"LIN_A8R8G8B8",
	"LIN_X8R8G8B8",
	"L8",
	"A8L8",
	"R5G6B5",
	"A4R4G4B4",
	"DXN",
	"DXT3A",
	"G8R8"
	};

	enum TextureFormat : int32_t {
		UNKNOWN = 0,
		DXT1 = 1,
		DXT3 = 2,
		DXT5 = 3,
		A8R8G8B8 = 4,
		X8R8G8B8 = 5,
		LIN_A8R8G8B8 = 6,
		LIN_X8R8G8B8 = 7,
		L8 = 8,
		A8L8 = 9,
		R5G6B5 = 10,
		A4R4G4B4 = 11,
		DXN = 12,
		DXT3A = 13,
		G8R8 = 14,
		LE_LIN_YUY2 = 15,
		MAX = 16
	};
	
	enum dbTexture_Type_e : uint8_t
	{
		dbTexture_Type_Direct,
		dbTexture_Type_Cube,
		dbTexture_Type_Array,
		dbTexture_Type_MAX
	};
	
	int32_t ConvertVPFormatToXenonFormat(int32_t format)
	{
		if (format == DXT1) return D3DFMT_DXT1;
		if (format == DXT3) return D3DFMT_DXT3;
		if (format == DXT5) return D3DFMT_DXT5;
		if (format == A8R8G8B8) return D3DFMT_A8R8G8B8;
		if (format == X8R8G8B8) return D3DFMT_LIN_X8R8G8B8;
		if (format == LIN_A8R8G8B8) return D3DFMT_LIN_A8R8G8B8;
		if (format == LIN_X8R8G8B8) return D3DFMT_LIN_X8R8G8B8;
		if (format == L8) return D3DFMT_L8;
		if (format == A8L8) return D3DFMT_A8L8;
		if (format == R5G6B5) return D3DFMT_LIN_R5G6B5;
		if (format == A4R4G4B4) return D3DFMT_A4R4G4B4;
		if (format == DXN) return D3DFMT_DXN;
		if (format == DXT3A) return D3DFMT_DXT3A;
		if (format == G8R8) return D3DFMT_G8R8;
	}
	
	struct dbTexture_s
	{
		int32_t format = UNKNOWN;
		int32_t d3dHeader = 0;
		int16_t width = 0;
		int16_t height = 0;
		uint8_t type = 0;
		uint8_t flags = 0;
		uint8_t userDefined = 0;
		uint8_t maxLOD = 0;
		uint8_t framesPerSecond = 0;
		uint8_t numFrames = 0;
		uint8_t currentFrameLoaded = 0;
		uint8_t requiredFrame = 0;
		int32_t imageDataStart = 0;
		int32_t sizeOfOneFrame = 0;
		
		char* textureHeaderPtr;
		bool refresh = true;

		unsigned char* textureDataPtr;

		void ReadTextureInfo(char* data, bool isBigEndian);

		void SetTextureHeaderPtr(char* ptr) {
			textureHeaderPtr = ptr;
		}

		void SetTextureDataPtr(unsigned char* ptr) {
			textureDataPtr = ptr;
		}
	};
}