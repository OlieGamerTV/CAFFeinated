#include "CommonReader.h"
#include "GhoulFileTypes.h"

void TextureFile::ParseTextureHeader(char* data) {
	if (data == nullptr) {
		printf("TextureFile::ParseTextureHeader() - The provided data is a null pointer.\n");
		return;
	}
	memcpy(&header.format, data, sizeof(int));
	memcpy(&header.headerSize, data + 4, sizeof(int));
	memcpy(&header.width, data + 8, sizeof(short));
	memcpy(&header.height, data + 0xA, sizeof(short));
	memcpy(&header.flags, data + 0xC, sizeof(int));
	memcpy(&header.framerate, data + 0x10, sizeof(char));
	memcpy(&header.frameCount, data + 0x11, sizeof(char));
	memcpy(&header.sUnk1, data + 0x12, sizeof(char));
	memcpy(&header.tileCount, data + 0x14, sizeof(int));
	memcpy(&header.chunkSize, data + 0x18, sizeof(int));

	printf("Format - %d | Header Size - %d | Width/Height - %d/%d\n", header.format, header.headerSize, header.width, header.height);

	entries = new TextureGPUEntry[1];
}

void TextureFile::ParseTextureEntries(char* data) {
	if (data == nullptr) {
		printf("TextureFile::ParseTextureEntries() - The provided data is a null pointer.\n");
		return;
	}
	if (entries == nullptr) {
		printf("TextureFile::ParseTextureEntries() - The entries section has not been initialized. Please run TextureFile::ParseTextureHeader() first.\n");
		return;
	}

	memcpy(&gpuHeader.size, data, sizeof(int));
	memcpy(&gpuHeader.iunk2, data + 4, sizeof(int));
	memcpy(&gpuHeader.offset, data + 8, sizeof(int));
	memcpy(&gpuHeader.chunkSize, data + 0xC, sizeof(int));

	printf("Header Size - %d | UNK - %d | Data Offset - %d | Chunk Size - %d\n", gpuHeader.size, gpuHeader.iunk2, gpuHeader.offset, gpuHeader.chunkSize);
}

void TextureFile::FreeTextureData() {
	if (texFileData != nullptr) {
		free(texFileData);
		texFileData = nullptr;
	}

	if (entries != nullptr) {
		delete[] entries;
		entries = nullptr;
	}

	refresh = true;
	framePos = 0;
}

TextureGPUEntry::~TextureGPUEntry() {
	if (textureData != nullptr) {
		free(textureData);
		textureData = nullptr;
	}
}