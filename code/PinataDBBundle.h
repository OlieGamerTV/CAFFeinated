#pragma once
//#include "Bundle.h"

struct IndexEntry {
	char filename[256];
	unsigned int timestamp = 0;
	float version = 0;

	// Precached Hash
	unsigned int hash = 0;

	// Precached IDX to corresponding hash.
	int idx = 0;
};

struct PrecacheEntry {
	int hashIdx = 0;
	int indexIdx = 0;
};

struct DBHashFile {
	int fileCount = 0;
	unsigned int* hash32_Array = nullptr;
	unsigned long long* hash64_Array = nullptr;
	int* offsetArray = nullptr;
};

struct DBBundle {
public:
	char* savedFilePath = nullptr;

	char* hashFileData = nullptr;
	char* indexFileData = nullptr;

	DBHashFile hashFile;

	bool isTiPIndexFile = false;

	int indexCount = 0;
	IndexEntry* indexFile = nullptr;

	PrecacheEntry* precachedEntries = nullptr;

	bool isReady = false;
	bool hasErrored = false;

	bool readStandaloneDbBundleFiles(char* filePath);
	void readDbBundleFiles(char* hashData, char* indexData, int indexCount);

	// Allocates and returns the target file data.
	char* getFileData(int fileIdx, int* dataSize);
	char* getBundleFileData(int fileAid, int fileIdx);

	void ClearActiveBundleData();

	unsigned long long getHashFromArray(int idx) {
		if (isTiPIndexFile)
			return hashFile.hash64_Array[idx];

		if (!isTiPIndexFile)
			return hashFile.hash32_Array[idx];
	}

	// Gets the offset of the target file by adding the size of the files before it.
	int getOffsetOfFile(unsigned int aid) {
		int offset = 0;
		for (int i = 0; i < hashFile.fileCount; i++) {
			if (getHashFromArray(i) == aid) {
				return hashFile.offsetArray[i];
			}
		}
		return 0;
	}

	int getIdxOfFile(unsigned int aid) {
		int offset = 0;
		for (int i = 0; i < hashFile.fileCount; i++) {
			if (getHashFromArray(i) == aid) {
				return i;
			}
		}
		return -1;
	}

	char* getCorrespondingName(unsigned int aid) {
		int offset = 0;
		for (int i = 0; i < indexCount; i++) {
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