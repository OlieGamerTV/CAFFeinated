#include "CommonReader.h"
#include "GhoulBundle.h"

FileEntry::~FileEntry() {
	memset(fileName, 0, FILEENTRY_NAMESIZE);
}

bool GhoulBundle::ReadBundleFile(char* data, size_t dataSize) {
	isReady = false;

	if (data == nullptr) return false;
	dataPtr = data;

	try {
		memcpy_s(&entryCount, sizeof(int16_t), data, sizeof(int16_t));
		memcpy_s(&isCompressed, sizeof(int16_t), data + 2, sizeof(int16_t));
		memcpy_s(&iUnk1, sizeof(int32_t), data + 4, sizeof(int32_t));

		memcpy_s(&fileListOffset, sizeof(int32_t), data + 0x8, sizeof(int32_t));
		memcpy_s(&fileListDataSize, sizeof(int32_t), data + 0xC, sizeof(int32_t));
		memcpy_s(&gpuListOffset, sizeof(int32_t), data + 0x10, sizeof(int32_t));
		memcpy_s(&gpuListSize, sizeof(int32_t), data + 0x14, sizeof(int32_t));

		memcpy_s(&gpuSectOffset, sizeof(int32_t), data + 0x18, sizeof(int32_t));
		memcpy_s(&gpuSectSize, sizeof(int32_t), data + 0x1C, sizeof(int32_t));
		memcpy_s(&dataSectOffset, sizeof(int32_t), data + 0x20, sizeof(int32_t));
		memcpy_s(&dataSectSize, sizeof(int32_t), data + 0x24, sizeof(int32_t));

		int32_t totalSize = 0x28 + fileListDataSize + gpuListSize + gpuSectSize + dataSectSize;

		if (entryCount == 0 || iUnk1 != 0) return false;

		char* uncompedBaseData = (char*)malloc(totalSize);

		int32_t size = dataSize - 0x28;

		if (isCompressed == 1) {
			memcpy_s(uncompedBaseData, 0x28, data, 0x28);

			// Do the section table first
			char* in = (char*)malloc(size);

			memcpy_s(in, size, data + fileListOffset, size);

			char* out = InflateData(in, 9, size, fileListDataSize + gpuListSize + gpuSectSize + dataSectSize);

			memcpy_s(uncompedBaseData + 0x28, fileListDataSize + gpuListSize + gpuSectSize + dataSectSize, out, fileListDataSize + gpuListSize + gpuSectSize + dataSectSize);

			free(out);
		}
		else {
			memcpy_s(uncompedBaseData, totalSize, data, totalSize);
		}

		dataPtr = uncompedBaseData;

		fileEntries = new FileEntry[entryCount];

		if (fileEntries == nullptr)
			return false;

		for (int32_t i = 0; i < entryCount; i++) {
			strncpy(fileEntries[i].fileName, dataPtr + fileListOffset + (sizeof(FileEntry) * i), FILEENTRY_NAMESIZE);

			memcpy(&fileEntries[i].type, dataPtr + fileListOffset + (sizeof(FileEntry) * i) + FILEENTRY_NAMESIZE, sizeof(int32_t));
			memcpy(&fileEntries[i].timestamp, dataPtr + fileListOffset + (sizeof(FileEntry) * i) + FILEENTRY_NAMESIZE + 4, sizeof(int32_t));
			memcpy(&fileEntries[i].fUnk1, dataPtr + fileListOffset + (sizeof(FileEntry) * i) + FILEENTRY_NAMESIZE + 8, sizeof(int32_t));
			memcpy(&fileEntries[i].iUnk1, dataPtr + fileListOffset + (sizeof(FileEntry) * i) + FILEENTRY_NAMESIZE + 0xC, sizeof(int32_t));

			memcpy(&fileEntries[i].dataSectOffset, dataPtr + fileListOffset + (sizeof(FileEntry) * i) + FILEENTRY_NAMESIZE + 0x10, sizeof(int32_t));
			memcpy(&fileEntries[i].dataSectSize, dataPtr + fileListOffset + (sizeof(FileEntry) * i) + FILEENTRY_NAMESIZE + 0x14, sizeof(int32_t));
			memcpy(&fileEntries[i].gpuSectOffset, dataPtr + fileListOffset + (sizeof(FileEntry) * i) + FILEENTRY_NAMESIZE + 0x18, sizeof(int32_t));
			memcpy(&fileEntries[i].gpuSectSize, dataPtr + fileListOffset + (sizeof(FileEntry) * i) + FILEENTRY_NAMESIZE + 0x1C, sizeof(int32_t));
		}

		isReady = true;
	}
	catch (...) {
		printf("An error occured while reading a Ghoulies bundle file.\n");
		return false;
	}

	return true;
}

bool GhoulBundle::readStandaloneBundleFile(char* fileName) {
	FILE* currentFile = fopen(fileName, "rb");

	if (ferror(currentFile) != 0) {
		printf("Error occured while trying to open the file.\n");
		return false;
	}

	fseek(currentFile, 0L, SEEK_END);
	int32_t length = ftell(currentFile);
	fseek(currentFile, 0L, SEEK_SET);

	char* data = (char*)malloc(length);

	fread(data, sizeof(char), length, currentFile);

	fclose(currentFile);

	return ReadBundleFile(data, length);
}

void GhoulBundle::ClearBundleFileData() {
	isReady = false;

	if (fileEntries != nullptr) {
		delete[] fileEntries;
		fileEntries = nullptr;
	}

	if (dataPtr != nullptr) {
		free(dataPtr);
		dataPtr = nullptr;
	}
}