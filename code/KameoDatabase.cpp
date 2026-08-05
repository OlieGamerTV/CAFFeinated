
#include "CommonReader.h"
#include "KameoDatabase.h"

void KameoDBFile::ReadDatabaseFile(char* data, char byteswap) {
	if (data == nullptr) {
		return;
	}

	dbData = data;

	header.ReadDatabaseHeader(data, byteswap);

	fileTable.ReadDatabaseFileTable(data, header.fileTableOffset, byteswap);
}

void KameoDBHeader::ReadDatabaseHeader(char* data, char byteswap) {
	memcpy_s(headerString, 0x20, data, 0x20);

	int fileTableOffsetVar = 0;
	int unk1Var = 0;

	if (byteswap == 1) {
		memcpy_s(&fileTableOffsetVar, 4, data + 0x20, 4);
		memcpy_s(&unk1Var, 4, data + 0x24, 4);

		fileTableOffset = flipEndian(fileTableOffsetVar);
		unk1 = flipEndian(unk1Var);
	}

	if (byteswap == 0) {
		memcpy_s(&fileTableOffset, 4, data + 0x20, 4);
		memcpy_s(&unk1, 4, data + 0x24, 4);
	}
}

void KameoDBFileTable::ReadDatabaseFileTable(char* data, int offset, char byteswap) {

	int entryCountVar = 0;
	int offs1Var = 0;
	int offs2Var = 0;
	int offs3Var = 0;

	if (byteswap == 1) {
		memcpy_s(&entryCountVar, 4, data + offset, 4);
		memcpy_s(&offs1Var, 4, data + offset + 0x4, 4);
		memcpy_s(&offs2Var, 4, data + offset + 0x8, 4);
		memcpy_s(&offs3Var, 4, data + offset + 0xC, 4);

		header.entryCount = flipEndian(entryCountVar);
		header.nameTableOffs = flipEndian(offs1Var);
		header.indexTableOffs = flipEndian(offs2Var);
		header.hashTableOffs = flipEndian(offs3Var);
	}

	if (byteswap == 0) {
		memcpy_s(&header.entryCount, 4, data, 4);
		memcpy_s(&header.nameTableOffs, 4, data + 0x4, 4);
		memcpy_s(&header.indexTableOffs, 4, data + 0x8, 4);
		memcpy_s(&header.hashTableOffs, 4, data + 0xC, 4);
	}

	nameEntries = new KameoDBFileTableNameEntry[header.entryCount]();
	indexEntries = new KameoDBFileTableIndexEntry[header.entryCount]();
	hashEntries = new KameoDBFileTableHashEntry[header.entryCount]();

	int tmpVal1 = 0;
	int tmpVal2 = 0;
	int tmpVal3 = 0;

	// Name Entries
	int position = header.nameTableOffs;
	for (int i = 0; i < header.entryCount; i++) {
		if (byteswap == 1) {
			memcpy_s(&tmpVal1, 4, data + position, 4);
			memcpy_s(&tmpVal2, 4, data + position + 0x4, 4);
			memcpy_s(&tmpVal3, 4, data + position + 0x8, 4);

			nameEntries[i].nameOffset = flipEndian(tmpVal1);
			nameEntries[i].unk1 = flipEndian(tmpVal2);
			nameEntries[i].unk2 = flipEndian(tmpVal3);
		}

		if (byteswap == 0) {
			memcpy_s(&nameEntries[i].nameOffset, 4, data + position, 4);
			memcpy_s(&nameEntries[i].unk1, 4, data + position + 0x4, 4);
			memcpy_s(&nameEntries[i].unk2, 4, data + position + 0x8, 4);
		}

		printf("Entry %d - [%d %d %d]\n", i, nameEntries[i].nameOffset, nameEntries[i].unk1, nameEntries[i].unk2);
		position += 0xC;
	}

	// Index Table
	position = header.indexTableOffs;
	for (int i = 0; i < header.entryCount; i++) {
		if (byteswap == 1) {
			memcpy_s(&tmpVal1, 4, data + position, 4);
			memcpy_s(&tmpVal2, 4, data + position + 0x4, 4);
			memcpy_s(&tmpVal3, 4, data + position + 0x8, 4);

			indexEntries[i].unk1 = flipEndian(tmpVal1);
			indexEntries[i].index = flipEndian(tmpVal2);
			indexEntries[i].nameTableOffset = flipEndian(tmpVal3);
		}

		if (byteswap == 0) {
			memcpy_s(&indexEntries[i].unk1, 4, data + position, 4);
			memcpy_s(&indexEntries[i].index, 4, data + position + 0x4, 4);
			memcpy_s(&indexEntries[i].nameTableOffset, 4, data + position + 0x8, 4);
		}

		printf("Entry %d - [%d %d %d]\n", i, indexEntries[i].unk1, indexEntries[i].index, indexEntries[i].nameTableOffset);
		position += 0xC;
	}

	// Hash Entries
	position = header.hashTableOffs;
	for (int i = 0; i < header.entryCount; i++) {
		if (byteswap == 1) {
			memcpy_s(&tmpVal1, 4, data + position, 4);
			memcpy_s(&tmpVal2, 4, data + position + 0x4, 4);
			memcpy_s(&tmpVal3, 4, data + position + 0x8, 4);

			hashEntries[i].hash = flipEndian(tmpVal1);
			hashEntries[i].unk1 = flipEndian(tmpVal2);
			hashEntries[i].nameTableOffset = flipEndian(tmpVal3);
		}

		if (byteswap == 0) {
			memcpy_s(&hashEntries[i].hash, 4, data + position, 4);
			memcpy_s(&hashEntries[i].unk1, 4, data + position + 0x4, 4);
			memcpy_s(&hashEntries[i].nameTableOffset, 4, data + position + 0x8, 4);
		}

		printf("Entry %d - [%08X %d %d]\n", i, hashEntries[i].hash, hashEntries[i].unk1, hashEntries[i].nameTableOffset);
		position += 0xC;
	}

	fileNames = new char* [header.entryCount]();

	for (int i = 0; i < header.entryCount; i++) {
		fileNames[i] = new char[128];
		strcpy_s(fileNames[i], 128, data + nameEntries[i].nameOffset);

		printf("Entry %d - [%s]\n", i, fileNames[i]);
	}
}