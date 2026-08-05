#include "CommonReader.h"
#include "GhoulBundle.h"

bool GhoulBundle::ReadBundleFile(char* data, int dataSize) {
	if (data == nullptr) return false;
	dataPtr = data;

	try {
		memcpy_s(&entryCount, sizeof(short), data, sizeof(short));
		memcpy_s(&isCompressed, sizeof(short), data + 2, sizeof(short));
		memcpy_s(&iUnk1, sizeof(int), data + 4, sizeof(int));

		memcpy_s(&fileListOffset, sizeof(int), data + 0x8, sizeof(int));
		memcpy_s(&fileListDataSize, sizeof(int), data + 0xC, sizeof(int));
		memcpy_s(&gpuListOffset, sizeof(int), data + 0x10, sizeof(int));
		memcpy_s(&gpuListSize, sizeof(int), data + 0x14, sizeof(int));

		memcpy_s(&gpuSectOffset, sizeof(int), data + 0x18, sizeof(int));
		memcpy_s(&gpuSectSize, sizeof(int), data + 0x1C, sizeof(int));
		memcpy_s(&dataSectOffset, sizeof(int), data + 0x20, sizeof(int));
		memcpy_s(&dataSectSize, sizeof(int), data + 0x24, sizeof(int));

		int totalSize = 0x28 + fileListDataSize + gpuListSize + gpuSectSize + dataSectSize;

		if (entryCount == 0 || iUnk1 != 0) return false;

		char* uncompedBaseData = (char*)malloc(totalSize);

		int size = dataSize - 0x28;

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
		for (int i = 0; i < entryCount; i++) {
			strncpy(fileEntries[i].fileName, dataPtr + fileListOffset + (sizeof(FileEntry) * i), FILEENTRY_NAMESIZE);

			memcpy(&fileEntries[i].type, dataPtr + fileListOffset + (sizeof(FileEntry) * i) + FILEENTRY_NAMESIZE, sizeof(int));
			memcpy(&fileEntries[i].timestamp, dataPtr + fileListOffset + (sizeof(FileEntry) * i) + FILEENTRY_NAMESIZE + 4, sizeof(int));
			memcpy(&fileEntries[i].fUnk1, dataPtr + fileListOffset + (sizeof(FileEntry) * i) + FILEENTRY_NAMESIZE + 8, sizeof(int));
			memcpy(&fileEntries[i].iUnk1, dataPtr + fileListOffset + (sizeof(FileEntry) * i) + FILEENTRY_NAMESIZE + 0xC, sizeof(int));

			memcpy(&fileEntries[i].dataSectOffset, dataPtr + fileListOffset + (sizeof(FileEntry) * i) + FILEENTRY_NAMESIZE + 0x10, sizeof(int));
			memcpy(&fileEntries[i].dataSectSize, dataPtr + fileListOffset + (sizeof(FileEntry) * i) + FILEENTRY_NAMESIZE + 0x14, sizeof(int));
			memcpy(&fileEntries[i].gpuSectOffset, dataPtr + fileListOffset + (sizeof(FileEntry) * i) + FILEENTRY_NAMESIZE + 0x18, sizeof(int));
			memcpy(&fileEntries[i].gpuSectSize, dataPtr + fileListOffset + (sizeof(FileEntry) * i) + FILEENTRY_NAMESIZE + 0x1C, sizeof(int));
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
	int length = ftell(currentFile);
	fseek(currentFile, 0L, SEEK_SET);

	char* data = (char*)malloc(length);

	fread(data, sizeof(char), length, currentFile);

	fclose(currentFile);

	return ReadBundleFile(data, length);
}