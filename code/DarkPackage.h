#pragma once

struct DarkPackageFileTableEntry {
public:
	int nameOffset = 0;
	int dataOffset = 0;
	int unk1 = 0;
	int unk2 = 0;
	int unk3 = 0;
	int unk4 = 0;
	int unk5 = 0;
	int unk6 = 0;
};

struct DarkPackageFileTable {
public:
	DarkPackageFileTableEntry* entries;
	char** fileNames = nullptr;

	void ReadPackageFileTable(char* data, int fileCount, char byteswap);
};

struct DarkPackageHeader {
public:
	char headerString[0x20] = { 0 };
	int numOfFiles = 0;

	void ReadPackageHeader(char* data, char byteswap);
};

struct DarkPackageFile {
public:
	DarkPackageHeader header;
	DarkPackageFileTable fileTable;

	char* dbData = nullptr;

	void ReadPackageFile(char* data, char byteswap);
};