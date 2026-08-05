#pragma once

struct KameoDBFileTableHeader {
public:
	int entryCount = 0;
	int nameTableOffs = 0;
	int indexTableOffs = 0;
	int hashTableOffs = 0;
};

struct KameoDBFileTableHashEntry {
public:
	unsigned int hash = 0;
	int unk1 = 0;
	int nameTableOffset = 0;
};

struct KameoDBFileTableIndexEntry {
public:
	int unk1 = 0;
	int index = 0;
	int nameTableOffset = 0;
};

struct KameoDBFileTableNameEntry {
public:
	int nameOffset = 0;
	int unk1 = 0;
	int unk2 = 0;
};

struct KameoDBFileTable {
public:
	KameoDBFileTableHeader header;
	KameoDBFileTableNameEntry* nameEntries;
	KameoDBFileTableIndexEntry* indexEntries;
	KameoDBFileTableHashEntry* hashEntries;
	char** fileNames = nullptr;

	void ReadDatabaseFileTable(char* data, int offset, char byteswap);
};

struct KameoDBHeader {
public:
	char headerString[0x20] = { 0 };
	int fileTableOffset = 0;
	int unk1 = 0;

	void ReadDatabaseHeader(char* data, char byteswap);
};

struct KameoDBFile {
public:
	KameoDBHeader header;
	KameoDBFileTable fileTable;

	char* dbData = nullptr;

	void ReadDatabaseFile(char* data, char byteswap);
};