#include "CommonReader.h"
#include "RPK.h"

void RPKFile::ClearActiveData() {
	if (rpkData != nullptr) {
		free(rpkData);
		rpkData = nullptr;
	}

	if (fileEntries != nullptr) {
		delete[] fileEntries;
		fileEntries = nullptr;
	}
}

bool RPKFile::readStandaloneRPKFile(char* fileName) {
	isReady = false;
	hasErrored = false;

	char* data = nullptr;
	char* gzCheck = strstr(fileName, ".gz");
	bool isGZ = gzCheck != nullptr;

	if (!isGZ) {
		FILE* currentFile = fopen(fileName, "rb");

		if (ferror(currentFile) != 0) {
			printf("Error occured while trying to open the file.\n");
			hasErrored = true;
			return false;
		}

		fseek(currentFile, 0L, SEEK_END);
		int length = ftell(currentFile);

		fseek(currentFile, 0L, SEEK_SET);

		data = (char*)malloc(length);

		fread(data, sizeof(char), length, currentFile);

		fclose(currentFile);
	}

	if(isGZ) {
		// First we need to get the uncompressed size.
		FILE* currentFile = fopen(fileName, "rb");

		int uncompedSize = 0;
		fseek(currentFile, -4L, SEEK_END);
		fread(&uncompedSize, sizeof(int), 1, currentFile);

		printf("%d\n", uncompedSize);

		fclose(currentFile);

		// ...now we can uncompress our file.
		gzFile currGzFile = gzopen(fileName, "rb");

		if (currGzFile == NULL) {
			printf("Error occured while trying to open the file.\n");
			hasErrored = true;
			return false;
		}

		data = (char*)malloc(uncompedSize);

		int sizeRead = gzread(currGzFile, data, uncompedSize);

		if (sizeRead == -1) {
			printf("Error occured while trying to read the file.\n");
			hasErrored = true;
			return false;
		}

		gzclose(currGzFile);
		printf("No errors were hit while decompressing the file.\n");
	}

	bool error = readRPKFile(data);

	isReady = true;
	hasErrored = !error;
	return true;
}

bool RPKFile::readRPKFile(char* data) {
	if (data == nullptr) {
		printf("Passed data array is null.\n");
		return false;
	}

	rpkData = data;

	memcpy(&unk1, rpkData, 4);
	memcpy(&unk2, rpkData + 4, 4);
	memcpy(&fileCount, rpkData + 8, 4);

	if (unk1 != 0x7C9C23C4) {
		printf("The magic does not match the expected value. Got %08X, expected %08X.\n", unk1, 0x7C9C23C4);
		return false;
	}

	fileEntries = new RPKFileEntry[fileCount];

	int currentPos = RPK_DATASTART;
	printf("FILE ENTRIES - {\n");
	for (int i = 0; i < fileCount; i++) {
		memcpy(fileEntries[i].fileName, rpkData + currentPos, 0x100);
		memcpy(&fileEntries[i].dataOffs, rpkData + (currentPos + 0x100), 4);
		memcpy(&fileEntries[i].unk2, rpkData + (currentPos + 0x104), 4);
		memcpy(&fileEntries[i].dataSize, rpkData + (currentPos + 0x108), 4);
		memcpy(&fileEntries[i].unk4, rpkData + (currentPos + 0x10C), 4);

		printf("\t%03d -> %s\n", i, fileEntries[i].fileName);
		currentPos += RPK_FILEDATASIZE;
	}
	printf("}\n");

	return true;
}

char* RPKFile::getFileData(int fileIdx) {
	if (rpkData == nullptr) {
		printf("No file data has been supplied.\n");
		return nullptr;
	}

	char* fileData = (char*)malloc(fileEntries[fileIdx].dataSize);

	memcpy(fileData, rpkData + fileEntries[fileIdx].dataOffs, fileEntries[fileIdx].dataSize);

	return fileData;
}