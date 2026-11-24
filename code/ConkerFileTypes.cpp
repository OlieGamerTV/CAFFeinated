#include "CommonReader.h"
#include "ConkerFileTypes.h"

void ConkerPackage::ParsePackageData(char* data, bool isBigEndian) {
	memcpy(&magic, data, 0x20);

	memcpy(&nameCount, data + 0x20, sizeof(int32_t));
	if (isBigEndian) {
		//header.format = flipEndian(header.format);
		nameCount = flipEndian(nameCount);
	}

	entries = new ConkerPackageEntry[nameCount];
	fileNames = new char*[nameCount];

	if (entries == nullptr || fileNames == nullptr) return;

	for (int i = 0; i < nameCount; i++) {
		memcpy(&entries[i].nameOffs, data + 0x24 + (8 * i), sizeof(int32_t));
		memcpy(&entries[i].unkOffs, data + 0x28 + (8 * i), sizeof(int32_t));

		if (isBigEndian) {
			//header.format = flipEndian(header.format);
			entries[i].nameOffs = flipEndian(entries[i].nameOffs);
			entries[i].unkOffs = flipEndian(entries[i].unkOffs);
		}

		fileNames[i] = (char*)malloc(strlen(data + entries[i].nameOffs) + 1);
		memset(fileNames[i], 0, strlen(data + entries[i].nameOffs) + 1);
		memcpy(fileNames[i], data + entries[i].nameOffs, strlen(data + entries[i].nameOffs));

		printf("%d %d -> %s\n", entries[i].nameOffs, entries[i].unkOffs, fileNames[i]);
	}
}

void ConkerTextureFile::ParseTextureHeader(char* data, bool isBigEndian) {
	if (data == nullptr) {
		printf("TextureFile::ParseTextureHeader() - The provided data is a null pointer.\n");
		return;
	}

	texFileData = data;

	memcpy(&header.format, data, sizeof(int32_t));
	memcpy(&header.unk1, data + 4, sizeof(int32_t));
	memcpy(&header.unk2, data + 8, sizeof(int32_t));
	memcpy(&header.width, data + 0xC, sizeof(int16_t));
	memcpy(&header.height, data + 0xE, sizeof(int16_t));

	if (isBigEndian) {
		//header.format = flipEndian(header.format);
		header.unk1 = flipEndian(header.unk1);
		header.unk2 = flipEndian(header.unk2);
		header.width = flipEndian(header.width);
		header.height = flipEndian(header.height);
	}

	printf("Format - %d | Width/Height - %d/%d\n", header.format, header.width, header.height);
}