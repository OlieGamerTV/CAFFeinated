#include <cstdio>
#include <cstring>
#include <cstdint>
#include "PinataTex.h"
#include "ReaderUtils.h"

namespace Pinata
{
	void dbTexture_s::ReadTextureInfo(char* data, bool isBigEndian)
	{
		textureHeaderPtr = data;

		type = *(textureHeaderPtr + 0xC);
		flags = *(textureHeaderPtr + 0xD);
		userDefined = *(textureHeaderPtr + 0xE);
		maxLOD = *(textureHeaderPtr + 0xF);
		framesPerSecond = *(textureHeaderPtr + 0x10);
		numFrames = *(textureHeaderPtr + 0x11);
		currentFrameLoaded = *(textureHeaderPtr + 0x12);
		requiredFrame = *(textureHeaderPtr + 0x13);

		memcpy(&format, textureHeaderPtr + 0x0, sizeof(int32_t));
		memcpy(&d3dHeader, textureHeaderPtr + 0x4, sizeof(int32_t));
		memcpy(&width, textureHeaderPtr + 0x8, sizeof(int16_t));
		memcpy(&height, textureHeaderPtr + 0xA, sizeof(int16_t));
		memcpy(&imageDataStart, textureHeaderPtr + 0x14, sizeof(int32_t));
		memcpy(&sizeOfOneFrame, textureHeaderPtr + 0x18, sizeof(int32_t));

		if (isBigEndian)
		{
			format = flipEndian(format);
			d3dHeader = flipEndian(d3dHeader);
			width = flipEndian(width);
			height = flipEndian(height);
			imageDataStart = flipEndian(imageDataStart);
			sizeOfOneFrame = flipEndian(sizeOfOneFrame);
		}

		printf("Texture Type 0x%02x\n", format);
		printf("Width %d - Height %d\n", width, height);
	}
}
