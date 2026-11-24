#ifndef _CLRFILETYPES
#define _CLRFILETYPES

#ifdef _WIN32
#pragma once
#endif
#include <cstdint>

struct ConkerPackageEntry {
	int32_t nameOffs;
	int32_t unkOffs;
};

struct ConkerPackage {
	char magic[0x20];
	int32_t nameCount;
	ConkerPackageEntry* entries;
	char** fileNames;

	void ParsePackageData(char* data, bool isBigEndian);
};

struct ConkerTextureHeader {
	int32_t format = 0;
	int32_t unk1 = 0;
	int32_t unk2 = 0;
	int16_t width = 0, height = 0;
};

struct ConkerTextureFile {
	char* texFileData = nullptr;
	ConkerTextureHeader header;
	int32_t framePos = 0;

	bool refresh = true;

	void ParseTextureHeader(char* data, bool isBigEndian);
};

#endif