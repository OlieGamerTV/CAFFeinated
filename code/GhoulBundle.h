#pragma once

static const int FILEENTRY_NAMESIZE = 0x80;

struct FileEntry {
	char fileName[0x80];

	int type;
	unsigned int timestamp;
	float fUnk1;
	int iUnk1;

	// Actual Section Info
	int dataSectOffset;
	int dataSectSize;
	int gpuSectOffset;
	int gpuSectSize;
};

struct GhoulBundle {
	char* dataPtr;

	bool isReady;
	bool hasErrored = false;

	short entryCount;
	short isCompressed;
	int iUnk1;

	// Sections

	int fileListOffset;
	int fileListDataSize;
	int gpuListOffset;
	int gpuListSize;
	int gpuSectOffset;
	int gpuSectSize;
	int dataSectOffset;
	int dataSectSize;

	FileEntry* fileEntries;

	bool ReadBundleFile(char* data, int dataSize);
	bool readStandaloneBundleFile(char* fileName);

	char* GetDataSection() {
		if (dataPtr == nullptr) return nullptr;

		char* data = (char*)malloc(dataSectSize);
		memcpy(data, dataPtr + dataSectOffset, dataSectSize);

		return data;
	}

	char* GetGpuSection() {
		if (dataPtr == nullptr) return nullptr;
		if (gpuSectSize == 0) return nullptr;

		char* data = (char*)malloc(gpuSectSize);
		memcpy(data, dataPtr + gpuSectOffset, gpuSectSize);

		return data;
	}

	char* GetGPUListSection() {
		if (dataPtr == nullptr) return nullptr;
		if (gpuListSize == 0) return nullptr;

		char* data = (char*)malloc(gpuListSize);
		memcpy(data, dataPtr + gpuListOffset, gpuListSize);

		return data;
	}

	char* GetFileData(int idx) {
		if (idx < 0 || idx >= entryCount) return nullptr;

		char* data = GetDataSection();
		if (data == nullptr) return nullptr;

		char* fileData = (char*)malloc(fileEntries[idx].dataSectSize);
		memcpy(fileData, data + fileEntries[idx].dataSectOffset, fileEntries[idx].dataSectSize);

		free(data);
		return fileData;
	}

	char* GetFileGPUList(int idx) {
		if (idx < 0 || idx >= entryCount) return nullptr;

		char* listSect = GetGPUListSection();
		if (listSect == nullptr) return nullptr;

		int size = 0;

		memcpy(&size, listSect + fileEntries[idx].gpuSectOffset, 4);

		char* fileData = (char*)malloc(size);
		memcpy(fileData, listSect + fileEntries[idx].gpuSectOffset, size);

		free(listSect);
		return fileData;
	}

	char* GetFileGPU(int idx) {
		if (idx < 0 || idx >= entryCount) return nullptr;

		char* data = GetGpuSection();
		if (data == nullptr) return nullptr;

		char* fileData = (char*)malloc(fileEntries[idx].gpuSectSize);
		memcpy(fileData, data + fileEntries[idx].gpuSectOffset, fileEntries[idx].gpuSectSize);

		printf("%d %d %d\n", idx, fileEntries[idx].gpuSectOffset, fileEntries[idx].gpuSectSize);

		free(data);
		return fileData;
	}

	char* GetFileGPUForTexture(int idx) {
		if (idx < 0 || idx >= entryCount) return nullptr;

		char* listSect = GetGPUListSection();
		if (listSect == nullptr) return nullptr;

		int offset = 0;
		int chunkSize = 0;
		memcpy(&offset, listSect + fileEntries[idx].gpuSectOffset + 8, 4);
		memcpy(&chunkSize, listSect + fileEntries[idx].gpuSectOffset + 0xC, 4);

		char* data = GetGpuSection();
		if (data == nullptr) return nullptr;

		char* fileData = (char*)malloc(chunkSize);
		memcpy(fileData, data + offset, chunkSize);

		printf("%d %d %d\n", idx, offset, chunkSize);

		free(listSect);
		free(data);
		return fileData;
	}
};