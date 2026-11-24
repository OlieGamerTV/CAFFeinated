#include "CommonReader.h"
#include "GhoulFileTypes.h"

namespace Ghoulies {
	void AssetInventory::ReadStandaloneFile(char* fileName) {
		if (fileName == nullptr) {
			printf("fileName == nullptr");
			return;
		}

		FILE* file = fopen(fileName, "rb");

		if (file == nullptr) {
			printf("file == nullptr");
			return;
		}

		fseek(file, 0L, SEEK_END);
		int32_t length = ftell(file);
		fseek(file, 0L, SEEK_SET);

		char* buffer = (char*)malloc(length);
		if (buffer == nullptr) {
			printf("buffer == nullptr");
			return;
		}
		fread(buffer, 1, length, file);

		fclose(file);

		ReadFromData(buffer);

		printf("AssetInventory Loaded Correctly.");
	}

	void AssetInventory::ReadFromData(char* data) {
		if (data == nullptr) {
			printf("data == nullptr");
			return;
		}

		fileData = data;

		memcpy(&version, fileData, 4);
		memcpy(&entryCount, fileData + 4, 4);
		memcpy(&unk1, fileData + 8, 4);

		if (unk1 != 1) return;

		int startPos = 12;

		int entryPos = startPos;

		printf("%d entries\n", entryCount);

		for (int i = 0; i < entryCount; i++) {
			entries.emplace_back();

			strncpy(entries[i].aid, fileData + entryPos, 0x80);
			strncpy(entries[i].filePath, fileData + entryPos + 0x80, 0x108);
			memcpy(&entries[i].timeStamp, fileData + entryPos + 0x188, 4);
			memcpy(&entries[i].aidVersion, fileData + entryPos + 0x18C, 4);
			memcpy(&entries[i].binaryFormatVersion, fileData + entryPos + 0x190, 4);

			entryPos += 0x194;

			printf(AssetInventoryTABLEENTRYFORMAT, entries[i].aid, entries[i].aidVersion, entries[i].timeStamp, entries[i].filePath, entries[i].binaryFormatVersion);
		}
	}

	void AssetInventory::ExportToFile(char* fileName) {
		if (fileName == nullptr) {
			printf("fileName == nullptr");
			return;
		}

		FILE* wStrm = fopen(fileName, "w");

		fwrite(AssetInventoryVERSION, strlen(AssetInventoryVERSION), 1, wStrm);
		fwrite("\n", 1, 1, wStrm);
		fwrite(AssetInventoryTABLEHEADER, strlen(AssetInventoryTABLEHEADER), 1, wStrm);
		fwrite("\n", 1, 1, wStrm);

		for (int i = 0; i < entryCount; i++) {
			char buffer[1024] = { 0 };
			sprintf(buffer, AssetInventoryTABLEENTRYFORMAT, entries[i].aid, entries[i].aidVersion, entries[i].timeStamp, entries[i].filePath, entries[i].binaryFormatVersion);
			fwrite(buffer, strlen(buffer), 1, wStrm);
		}

		fflush(wStrm);
		fclose(wStrm);
	}
}

void TextureFile::ParseTextureHeader(char* data) {
	if (data == nullptr) {
		printf("TextureFile::ParseTextureHeader() - The provided data is a null pointer.\n");
		return;
	}
	memcpy(&header.format, data, sizeof(int32_t));
	memcpy(&header.headerSize, data + 4, sizeof(int32_t));
	memcpy(&header.width, data + 8, sizeof(int16_t));
	memcpy(&header.height, data + 0xA, sizeof(int16_t));
	memcpy(&header.flags, data + 0xC, sizeof(int32_t));
	memcpy(&header.framerate, data + 0x10, sizeof(char));
	memcpy(&header.frameCount, data + 0x11, sizeof(char));
	memcpy(&header.sUnk1, data + 0x12, sizeof(char));
	memcpy(&header.tileCount, data + 0x14, sizeof(int32_t));
	memcpy(&header.chunkSize, data + 0x18, sizeof(int32_t));

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

	memcpy(&gpuHeader.size, data, sizeof(int32_t));
	memcpy(&gpuHeader.iunk2, data + 4, sizeof(int32_t));
	memcpy(&gpuHeader.offset, data + 8, sizeof(int32_t));
	memcpy(&gpuHeader.chunkSize, data + 0xC, sizeof(int32_t));

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