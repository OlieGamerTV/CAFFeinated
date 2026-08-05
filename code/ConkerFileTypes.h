#pragma once

struct ConkerTextureHeader {
	int format = 0;
	int unk1 = 0;
	int unk2 = 0;
	short width = 0, height = 0;
};

struct ConkerTextureFile {
	char* texFileData = nullptr;
	ConkerTextureHeader header;
	int framePos = 0;

	bool refresh = true;

	void ParseTextureHeader(char* data);
};