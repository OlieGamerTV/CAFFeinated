
#include "CommonReader.h"
#include "DarkPackage.h"

void DarkPackageFile::ReadPackageFile(char* data, char byteswap) {
	if (data == nullptr) {
		return;
	}

	dbData = data;

	header.ReadPackageHeader(data, byteswap);

	fileTable.ReadPackageFileTable(data, header.numOfFiles, byteswap);
}

void DarkPackageHeader::ReadPackageHeader(char* data, char byteswap) {
	memcpy_s(headerString, 0x20, data, 0x20);

	int numOfFilesVar = 0;

	if (byteswap == 1) {
		memcpy_s(&numOfFilesVar, 4, data + 0x20, 4);

		numOfFiles = flipEndian(numOfFilesVar);
	}

	if (byteswap == 0) {
		memcpy_s(&numOfFiles, 4, data + 0x20, 4);
	}
}

void DarkPackageFileTable::ReadPackageFileTable(char* data, int fileCount, char byteswap) {

	int nameOffsetVar = 0;
	int dataOffsetVar = 0;
	int unk1Var = 0;
	int unk2Var = 0;
	int unk3Var = 0;
	int unk4Var = 0;
	int unk5Var = 0;
	int unk6Var = 0;

	int offset = 0x24;

	entries = new DarkPackageFileTableEntry[fileCount]();

	for (int i = 0; i < fileCount; i++) {
		if (byteswap == 1) {
			memcpy_s(&nameOffsetVar, 4, data + offset, 4);
			memcpy_s(&dataOffsetVar, 4, data + offset + 0x4, 4);
			memcpy_s(&unk1Var, 4, data + offset + 0x8, 4);
			memcpy_s(&unk2Var, 4, data + offset + 0xC, 4);
			memcpy_s(&unk3Var, 4, data + offset + 0x10, 4);
			memcpy_s(&unk4Var, 4, data + offset + 0x14, 4);
			memcpy_s(&unk5Var, 4, data + offset + 0x18, 4);
			memcpy_s(&unk6Var, 4, data + offset + 0x1C, 4);

			entries[i].nameOffset = flipEndian(nameOffsetVar);
			entries[i].dataOffset = flipEndian(dataOffsetVar);
			entries[i].unk1 = flipEndian(unk1Var);
			entries[i].unk2 = flipEndian(unk2Var);
			entries[i].unk3 = flipEndian(unk3Var);
			entries[i].unk4 = flipEndian(unk4Var);
			entries[i].unk5 = flipEndian(unk5Var);
			entries[i].unk6 = flipEndian(unk6Var);
		}

		if (byteswap == 0) {
			memcpy_s(&entries[i].nameOffset, 4, data + offset, 4);
			memcpy_s(&entries[i].dataOffset, 4, data + offset + 0x4, 4);
			memcpy_s(&entries[i].unk1, 4, data + offset + 0x8, 4);
			memcpy_s(&entries[i].unk2, 4, data + offset + 0xC, 4);
			memcpy_s(&entries[i].unk3, 4, data + offset + 0x10, 4);
			memcpy_s(&entries[i].unk4, 4, data + offset + 0x14, 4);
			memcpy_s(&entries[i].unk5, 4, data + offset + 0x18, 4);
			memcpy_s(&entries[i].unk6, 4, data + offset + 0x1C, 4);
		}

		offset += 0x20;
	}

	fileNames = new char* [fileCount]();

	for (int i = 0; i < fileCount; i++) {
		fileNames[i] = new char[0x80]();

		strcpy_s(fileNames[i], 0x80, data + entries[i].nameOffset);

		printf("FILE %d - %s\n", i, fileNames[i]);
	}
}