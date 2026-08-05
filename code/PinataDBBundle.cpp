#include "CommonReader.h"
#include "AssetCommons.h"
#include "PinataDBBundle.h"
#include "Bundle.h"

DBBundle::~DBBundle() {
	if (indexFile != nullptr) {
		delete[] indexFile;
		indexFile = nullptr;
	}

	if (hashFile.offsetArray != nullptr) {
		delete[] hashFile.offsetArray;
		hashFile.offsetArray = nullptr;
	}

	if (hashFile.hash32_Array != nullptr) {
		delete[] hashFile.hash32_Array;
		hashFile.hash32_Array = nullptr;
	}

	if (hashFile.hash64_Array != nullptr) {
		delete[] hashFile.hash64_Array;
		hashFile.hash64_Array = nullptr;
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

char* DBBundle::getFileData(int fileIdx, int* dataSize) {
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

	int baseSize = tmpBundle.getBaseSizeOfCompedBundle();

	char* baseFileData = (char*)realloc(fileData, baseSize);

	fseek(packFile, hashFile.offsetArray[fileIdx], SEEK_SET);

	fread(baseFileData, 1, baseSize, packFile);

	tmpBundle.readBundleSectionFileV0036(baseFileData);

	int fullCaffSize = tmpBundle.getTotalSizeOfCompedBundle();

	char* fullCaffData = (char*)realloc(baseFileData, fullCaffSize);

	fseek(packFile, hashFile.offsetArray[fileIdx], SEEK_SET);
	fread(fullCaffData, 1, fullCaffSize, packFile);

	fclose(packFile);

	*dataSize = fullCaffSize;

	return fullCaffData;
}

void DBBundle::readStandaloneDbBundleFiles(char* filePath) {
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

		fclose(idxFile);
		idxFile = fopen(indexFilePath, "rb");

		if (idxFile != 0) {
			isTiPIndexFile = true;
		}
	}

	// If both cases fail, print an error and return.
	if (idxFile == 0) {
		printf("%s - db_index does not exist in this directory.\n", __func__);
		hasErrored = true;
		return;
	}

	FILE* hashFile = fopen(hashFilePath, "rb");

	if (hashFile == 0) {
		printf("%s - debug_hash does not exist in this directory.\n", __func__);
		fclose(idxFile);
		fclose(hashFile);
		hasErrored = true;
		return;
	}

	fseek(idxFile, 0L, SEEK_END);
	int length = ftell(idxFile);
	fseek(idxFile, 0L, SEEK_SET);

	char* indexData = (char*)malloc(length);

	fread(indexData, sizeof(char), length, idxFile);

	fseek(idxFile, 0L, SEEK_SET);

	unsigned int number_of_lines = 0;
	int ch;
	while (EOF != (ch = getc(idxFile)))
		if ('\n' == ch)
			++number_of_lines;

	fclose(idxFile);

	printf("%s - idx %d.\n", __func__, length);

	char* inputIndexData = indexData;
	if (isTiPIndexFile) {
		int uncompedSize = 0;
		memcpy(&uncompedSize, indexData, 4);
		uncompedSize = flipEndian(uncompedSize);

		inputIndexData = InflateData(indexData + 4, 9, length - 4, uncompedSize);
	}

	fseek(hashFile, 0L, SEEK_END);
	length = ftell(hashFile);
	fseek(hashFile, 0L, SEEK_SET);

	char* hashData = (char*)malloc(length);

	fread(hashData, sizeof(char), length, hashFile);

	fclose(hashFile);

	printf("%s - hash %d.\n", __func__, length);

	readDbBundleFiles(hashData, inputIndexData, number_of_lines);
	
	return;
}

void DBBundle::readDbBundleFiles(char* hashData, char* indexData, int totalIndexCount) {
	if (hashData == nullptr || indexData == nullptr) {
		printf("%s - Either hashData or indexData is null.\n", __func__);
		hasErrored = true;
		return;
	}

	hashFileData = hashData;
	indexFileData = indexData;
	indexCount = totalIndexCount;

	int fileCount = 0;

	memcpy(&fileCount, hashData, 4);

	hashFile.fileCount = flipEndian(fileCount);

	printf("%s - %d.\n", __func__, hashFile.fileCount);

	// Parse the Hash File Data first.
	int hashBasePos = 4;
	int offsetBasePos = 4 + (hashFile.fileCount * 4);

	if (isTiPIndexFile) {
		hashFile.hash64_Array = new unsigned long long[hashFile.fileCount];
		offsetBasePos = 4 + (hashFile.fileCount * 8);
	}
	else {
		hashFile.hash32_Array = new unsigned int[hashFile.fileCount];
	}

	hashFile.offsetArray = new int[hashFile.fileCount];

	precachedEntries = new PrecacheEntry[hashFile.fileCount];

	indexFile = new IndexEntry[totalIndexCount];

	// Get the hash & offsets first...
	for (int i = 0; i < hashFile.fileCount; i++) {
		int offset = 0;

		if (isTiPIndexFile) {
			unsigned int hash = 0;
			memcpy(&hash, hashData + hashBasePos + (i * 8), 8);
			hashFile.hash64_Array[i] = flipEndian(hash);
		}
		else {
			unsigned int hash = 0;
			memcpy(&hash, hashData + hashBasePos + (i * 4), 4);
			hashFile.hash32_Array[i] = flipEndian(hash);
		}

		memcpy(&offset, hashData + offsetBasePos + (i * 4), 4);

		hashFile.offsetArray[i] = flipEndian(offset);

		printf("HASH ENTRY %d - [%08X %d]\n", i, hashFile.hash32_Array[i], hashFile.offsetArray[i]);
	}

	// ...and now we get the available names.
	int charRead = 0;
	for (int i = 0; i < totalIndexCount; i++) {
		memset(indexFile[i].filename, 0, 256);
		sscanf(indexData + charRead, "%s %d %f", indexFile[i].filename, &indexFile[i].timestamp, &indexFile[i].version);

		indexFile[i].hash = assetIdGetHash_Base(indexFile[i].filename);

		charRead += strlen(indexFile[i].filename) + 0x13;

		printf("%s %d %f\n", indexFile[i].filename, indexFile[i].timestamp, indexFile[i].version);

		int idx = getIdxOfFile(indexFile[i].hash);
		if (idx != -1) {

			precachedEntries[idx].hashIdx = idx;
			precachedEntries[idx].indexIdx = i;
			printf("HASH ENTRY %d - String %s correlates to known hash %08X.\n", i, indexFile[i].filename, indexFile[i].hash);
		}

		if (idx == -1) {
			precachedEntries[idx].hashIdx = -1;
			precachedEntries[idx].indexIdx = -1;
		}
	}

	isReady = true;
}