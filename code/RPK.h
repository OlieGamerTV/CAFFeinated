#pragma once

static const int RPK_DATASTART = 0xC;
static const int RPK_FILEDATASIZE = 0x110;

struct RPKFileEntry {
public:
	char fileName[256];
	int dataOffs;
	int unk2;
	int dataSize;
	int unk4;
};

struct RPKFile {
public:
	char* rpkData;
	bool isReady = false;
	bool hasErrored = false;
	unsigned int unk1;
	int unk2;
	int fileCount;

	RPKFileEntry* fileEntries;

	~RPKFile();

	bool readStandaloneRPKFile(char* fileName);
	bool readRPKFile(char* data);

	char* getFileData(int fileIdx);
};