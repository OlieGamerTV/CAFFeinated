#pragma once

static const size_t FILEENTRY_NAMESIZE = 0x80;

struct FileEntry {
	char fileName[0x80];

	int32_t type;
	uint32_t timestamp;
	float fUnk1;
	int32_t iUnk1;

	// Actual Section Info
	int32_t dataSectOffset;
	int32_t dataSectSize;
	int32_t gpuSectOffset;
	int32_t gpuSectSize;

	~FileEntry();
};

struct GhoulBundle {
	char* dataPtr;

	bool isReady;
	bool hasErrored = false;

	int16_t entryCount;
	int16_t isCompressed;
	int32_t iUnk1;

	// Sections

	int32_t fileListOffset;
	int32_t fileListDataSize;
	int32_t gpuListOffset;
	int32_t gpuListSize;
	int32_t gpuSectOffset;
	int32_t gpuSectSize;
	int32_t dataSectOffset;
	int32_t dataSectSize;

	FileEntry* fileEntries;

	bool ReadBundleFile(char* data, size_t dataSize);
	bool readStandaloneBundleFile(char* fileName);

	void ClearBundleFileData();

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

	char* GetFileData(int32_t idx) {
		if (idx < 0 || idx >= entryCount) return nullptr;

		char* data = GetDataSection();
		if (data == nullptr) return nullptr;

		char* fileData = (char*)malloc(fileEntries[idx].dataSectSize);
		memcpy(fileData, data + fileEntries[idx].dataSectOffset, fileEntries[idx].dataSectSize);

		free(data);
		return fileData;
	}

	char* GetFileGPUList(int32_t idx) {
		if (idx < 0 || idx >= entryCount) return nullptr;

		char* listSect = GetGPUListSection();
		if (listSect == nullptr) return nullptr;

		int32_t size = 0;

		memcpy(&size, listSect + fileEntries[idx].gpuSectOffset, 4);

		char* fileData = (char*)malloc(size);
		memcpy(fileData, listSect + fileEntries[idx].gpuSectOffset, size);

		free(listSect);
		return fileData;
	}

	char* GetFileGPU(int32_t idx) {
		if (idx < 0 || idx >= entryCount) return nullptr;

		char* data = GetGpuSection();
		if (data == nullptr) return nullptr;

		char* fileData = (char*)malloc(fileEntries[idx].gpuSectSize);
		memcpy(fileData, data + fileEntries[idx].gpuSectOffset, fileEntries[idx].gpuSectSize);

		printf("%d %d %d\n", idx, fileEntries[idx].gpuSectOffset, fileEntries[idx].gpuSectSize);

		free(data);
		return fileData;
	}

	char* GetFileGPUForTexture(int32_t idx) {
		if (idx < 0 || idx >= entryCount) return nullptr;

		char* listSect = GetGPUListSection();
		if (listSect == nullptr) return nullptr;

		int32_t offset = 0;
		int32_t chunkSize = 0;
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