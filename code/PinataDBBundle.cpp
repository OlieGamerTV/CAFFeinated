#include "CommonReader.h"
#include "AssetCommons.h"
#include "PinataDBBundle.h"
#include "LoadingProcess.h"
#include "Bundle.h"

char* DBBundle::getFileData(int32_t fileIdx, int32_t* dataSize) {
	if (savedFilePath == nullptr) {
		return nullptr;
	}

	char packFilePath[1024];
	packFilePath[0] = 0;

	sprintf(packFilePath, "%s\\debug_pack.bin", savedFilePath);

	FILE* packFile = fopen(packFilePath, "rb");

	if (packFile == nullptr) {
		printf("DBBundle::getFileData - debug_pack does not exist in this directory.\n");
		return nullptr;
	}

	BundleV36 tmpBundle;

	char* fileData = (char*)malloc(0x78);

	fseek(packFile, hashFile.offsetArray[fileIdx], SEEK_SET);

	fread(fileData, 1, 0x78, packFile);

	tmpBundle.readBundleHeaderV0036(fileData);

	int32_t baseSize = tmpBundle.getBaseSizeOfCompedBundle();

	char* baseFileData = (char*)realloc(fileData, baseSize);

	fseek(packFile, hashFile.offsetArray[fileIdx], SEEK_SET);

	fread(baseFileData, 1, baseSize, packFile);

	tmpBundle.readBundleSectionFileV0036(baseFileData);

	int32_t fullCaffSize = tmpBundle.getTotalSizeOfCompedBundle();

	char* fullCaffData = (char*)realloc(baseFileData, fullCaffSize);

	fseek(packFile, hashFile.offsetArray[fileIdx], SEEK_SET);
	fread(fullCaffData, 1, fullCaffSize, packFile);

	fclose(packFile);

	if (dataSize != nullptr) {
		*dataSize = fullCaffSize;
	}

	return fullCaffData;
}

bool DBBundle::readStandaloneDbBundleFiles(char* filePath) {
	isTiPIndexFile = false;
	hasErrored = false;
	isReady = false;

	savedFilePath = filePath;

	char indexFilePath[1024];
	indexFilePath[0] = 0;

	char hashFilePath[1024];
	hashFilePath[0] = 0;

	sprintf(indexFilePath, "%s\\db_index.txt", filePath);
	sprintf(hashFilePath, "%s\\debug_hash.bin", filePath);

	FILE* idxFile = fopen(indexFilePath, "rb");

	if (idxFile == 0) {
		sprintf(indexFilePath, "%s\\db_index.bin", filePath); // If "db_index.txt" doesn't exist, check instead for "db_index.bin", which is what Trouble in Paradise uses.

		//fclose(idxFile);
		idxFile = fopen(indexFilePath, "rb");

		if (idxFile != 0) {
			isTiPIndexFile = true;
		}
	}

	// If both cases fail, print an error and return.
	if (idxFile == 0) {
		printf("%s - db_index does not exist in this directory.\n", __func__);
		hasErrored = true;
		return false;
	}

	FILE* hashFile = fopen(hashFilePath, "rb");

	if (hashFile == 0) {
		printf("%s - debug_hash does not exist in this directory.\n", __func__);
		fclose(idxFile);
		fclose(hashFile);
		hasErrored = true;
		return false;
	}

	fseek(idxFile, 0L, SEEK_END);
	int32_t length = ftell(idxFile);
	fseek(idxFile, 0L, SEEK_SET);

	char* indexData = (char*)malloc(length);

	fread(indexData, sizeof(char), length, idxFile);

	fseek(idxFile, 0L, SEEK_SET);

	uint32_t number_of_lines = 0;
	if (!isTiPIndexFile) {
		char ch[256];
		while (!feof(idxFile)) {
			fgets(ch, 256, idxFile);
			++number_of_lines;
		}

		printf("%s - idx %d.\n", __func__, length);
	}

	fclose(idxFile);

	char* inputIndexData = indexData;
	if (isTiPIndexFile) {
		int32_t uncompedSize = 0;
		memcpy(&uncompedSize, indexData, 4);
		uncompedSize = flipEndian(uncompedSize);

		inputIndexData = InflateData(indexData + 4, 9, length - 4, uncompedSize);

		free(indexData);
	}

	if (isTiPIndexFile) {
		int32_t charRead = 0;
		char* offs = inputIndexData;

		int count = 0;

		number_of_lines++;
		while (offs = strchr(offs + 1, '\n'), offs != 0) {
			++number_of_lines;
		}

		printf("%s - idx %d.\n", __func__, number_of_lines);
	}

	fseek(hashFile, 0L, SEEK_END);
	length = ftell(hashFile);
	fseek(hashFile, 0L, SEEK_SET);

	char* hashData = (char*)malloc(length);

	fread(hashData, sizeof(char), length, hashFile);

	fclose(hashFile);

	printf("%s - hash %d.\n", __func__, length);

	readDbBundleFiles(hashData, inputIndexData, number_of_lines);
	
	return true;
}

void DBBundle::readDbBundleFiles(char* hashData, char* indexData, int32_t totalIndexCount) {
	if (hashData == nullptr || indexData == nullptr) {
		printf("%s - Either hashData or indexData is null.\n", __func__);
		hasErrored = true;
		return;
	}

	hashFileData = hashData;
	indexFileData = indexData;
	indexCount = totalIndexCount;

	int32_t fileCount = 0;

	memcpy(&fileCount, hashData, 4);

	hashFile.fileCount = flipEndian(fileCount);

	printf("%s - %d.\n", __func__, hashFile.fileCount);

	// Parse the Hash File Data first.
	int32_t hashBasePos = 4;
	int32_t offsetBasePos = 4 + (hashFile.fileCount * 4);

	hashFile.hashArray = new uint64_t[hashFile.fileCount];
	if (isTiPIndexFile) {
		offsetBasePos = 4 + (hashFile.fileCount * 8);
	}

	hashFile.offsetArray = new int32_t[hashFile.fileCount];

	precachedEntries = new PrecacheEntry[hashFile.fileCount];

	memset(precachedEntries, 0xFF, sizeof(PrecacheEntry) * hashFile.fileCount);

	indexFile = new IndexEntry[totalIndexCount];
	memset(indexFile, 0, sizeof(IndexEntry) * totalIndexCount);

	CloseLoadingPromptWidget();
	SetupLoadingBarPromptWidget("Loading contents of debug_hash.bin", hashFile.fileCount);

	// Get the hash & offsets first...
	for (int32_t i = 0; i < hashFile.fileCount; i++) {
		int32_t offset = 0;

		if (isTiPIndexFile) {
			uint64_t hash = 0;
			memcpy(&hash, hashData + hashBasePos + (i * 8), 8);
			hashFile.hashArray[i] = flipEndian(hash);
		}
		else {
			uint32_t hash = 0;
			memcpy(&hash, hashData + hashBasePos + (i * 4), 4);
			hashFile.hashArray[i] = flipEndian(hash);
		}

		memcpy(&offset, hashData + offsetBasePos + (i * 4), 4);

		hashFile.offsetArray[i] = flipEndian(offset);

		if (isTiPIndexFile) {
			printf("HASH ENTRY %d - [%016llX %d]\n", i, hashFile.hashArray[i], hashFile.offsetArray[i]);
		}
		else {
			printf("HASH ENTRY %d - [%08X %d]\n", i, hashFile.hashArray[i], hashFile.offsetArray[i]);
		}

		IncreaseCurrentSavedOnLoadingWidget();
	}

	if (isTiPIndexFile) {
		SetupLoadingBarPromptWidget("Pre-caching the contents of db_index.bin", totalIndexCount);
	}
	else {
		SetupLoadingBarPromptWidget("Pre-caching the contents of db_index.txt", totalIndexCount);
	}
	
	// ...and now we get the available names.
	int32_t charRead = 0;
	for (int32_t i = 0; i < totalIndexCount; i++) {
		int count = 0;
		memset(indexFile[i].filename, 0, 256);
		sscanf(indexData + charRead, "%s %d %f%n", indexFile[i].filename, &indexFile[i].timestamp, &indexFile[i].version, &count);

		if (isTiPIndexFile) {
			indexFile[i].hash = assetIdGetHash_BaseExt(indexFile[i].filename);
		}
		else {
			indexFile[i].hash = assetIdGetHash_Base(indexFile[i].filename);
		}

		
		charRead += count;

		printf("%s %d %f\n", indexFile[i].filename, indexFile[i].timestamp, indexFile[i].version);

		int32_t idx = getIdxOfFile(indexFile[i].hash);
		if (idx != -1) {

			precachedEntries[idx].hashIdx = idx;
			precachedEntries[idx].indexIdx = i;
			if (isTiPIndexFile) {
				printf("ENTRY %d - String %s correlates to known hash 0x%016I64x.\n", i, indexFile[i].filename, indexFile[i].hash);
			}
			else {
				printf("ENTRY %d - String %s correlates to known hash %08X.\n", i, indexFile[i].filename, indexFile[i].hash);
			}
		}

		if (idx == -1) {
			if (isTiPIndexFile) {
				printf("ENTRY %d - Hash of string %s (0x%016I64x) isn't present in index_hash.\n", i, indexFile[i].filename, indexFile[i].hash);
			}
			else {
				printf("ENTRY %d - Hash of string %s (%08X) isn't present in index_hash.\n", i, indexFile[i].filename, indexFile[i].hash);
			}
		}
		IncreaseCurrentSavedOnLoadingWidget();
	}

	CloseLoadingBarPromptWidget();

	isReady = true;
}

void DBBundle::ClearActiveBundleData() {
	try {
		if (precachedEntries != nullptr) {
			delete[] precachedEntries;
			precachedEntries = nullptr;
		}

		if (indexFile != nullptr) {
			delete[] indexFile;
			indexFile = nullptr;
		}

		hashFile.fileCount = 0;
		if (hashFile.hashArray != nullptr) {
			delete[] hashFile.hashArray;
			hashFile.hashArray = nullptr;
		}

		if (hashFile.offsetArray != nullptr) {
			delete[] hashFile.offsetArray;
			hashFile.offsetArray = nullptr;
		}

		if (hashFileData != nullptr) {
			free(hashFileData);
			hashFileData = nullptr;
		}

		if (indexFileData != nullptr) {
			free(indexFileData);
			indexFileData = nullptr;
		}
	}
	catch (std::exception e) {
		printf("%s\n", e.what());
	}
}