#pragma once
#include "xenon_texture.h"

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

	enum TextureFormat : int {
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
		G8R8 = 14
	};
}