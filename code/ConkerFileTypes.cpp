#include "CommonReader.h"
#include "ConkerFileTypes.h"

void ConkerTextureFile::ParseTextureHeader(char* data) {
	if (data == nullptr) {
		printf("TextureFile::ParseTextureHeader() - The provided data is a null pointer.\n");
		return;
	}

	texFileData = data;

	memcpy(&header.format, data, sizeof(int));
	memcpy(&header.unk1, data + 4, sizeof(int));
	memcpy(&header.unk2, data + 8, sizeof(int));
	memcpy(&header.width, data + 0xC, sizeof(short));
	memcpy(&header.height, data + 0xE, sizeof(short));

	printf("Format - %d | Width/Height - %d/%d\n", header.format, header.width, header.height);
}