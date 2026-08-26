#pragma once
#ifndef _PINATADBBUNDLE
#define _PINATADBBUNDLE

struct IndexEntry {
	char filename[256];
	uint32_t timestamp = 0;
	float version = 0;

	// Precached Hash
	uint64_t hash = 0;

	// Precached IDX to corresponding hash.
	int32_t idx = 0;
};

struct PrecacheEntry {
	int32_t hashIdx = 0;
	int32_t indexIdx = 0;
};

struct DBHashFile {
	int32_t fileCount = 0;
	uint64_t* hashArray = nullptr;
	int32_t* offsetArray = nullptr;
};

struct DBBundle {
public:
	char* savedFilePath = nullptr;

	char* hashFileData = nullptr;
	char* indexFileData = nullptr;

	DBHashFile hashFile;

	bool isTiPIndexFile = false;

	int32_t indexCount = 0;
	IndexEntry* indexFile = nullptr;

	PrecacheEntry* precachedEntries = nullptr;

	bool isReady = false;
	bool hasErrored = false;

	bool readStandaloneDbBundleFiles(char* filePath);
	void readDbBundleFiles(char* hashData, char* indexData, int32_t indexCount);

	// Allocates and returns the target file data.
	char* getFileData(int32_t fileIdx, int32_t* dataSize);
	char* getBundleFileData(int32_t fileAid, int32_t fileIdx);

	void ClearActiveBundleData();

	uint64_t getHashFromArray(int32_t idx) {
		return hashFile.hashArray[idx];
	}

	// Gets the offset of the target file by adding the size of the files before it.
	int32_t getOffsetOfFile(uint64_t aid) {
		int32_t offset = 0;
		for (int32_t i = 0; i < hashFile.fileCount; i++) {
			if (hashFile.hashArray[i] == aid) {
				return hashFile.offsetArray[i];
			}
		}
		return 0;
	}

	int32_t getIdxOfFile(uint64_t aid) {
		int32_t offset = 0;
		for (int32_t i = 0; i < hashFile.fileCount; i++) {
			if (hashFile.hashArray[i] == aid) {
				return i;
			}
		}
		return -1;
	}

	char* getCorrespondingName(uint64_t aid) {
		int32_t offset = 0;
		for (int32_t i = 0; i < indexCount; i++) {
			if (indexFile[i].hash == aid) {
				//printf("getCorrespondingName - %s tied to hash %08X.\n", indexFile[i].filename, aid);
				return indexFile[i].filename;
			}
		}
		//printf("getCorrespondingName - No filename corresponds to hash %08X.\n", aid);
		return 0;
	}

	//IMGUI Popups
	//bool addStreamedReference(bool* open);
};
#endif