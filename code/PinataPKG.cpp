#include "PinataPKG.h"
#include "Bundle.h"

namespace Pinata {
	bool PKGFile::readStandaloneDbBundleFiles(char* filePath) {
		FILE* currentFile = fopen(filePath, "rb");

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

		return readDbBundleFiles(data);
	}

	bool PKGFile::readDbBundleFiles(char* fileData) {
		isReady = false;

		if (fileData == nullptr) {
			printf("%s - fileData is null.\n", __func__);
			return false;
		}

		isReady = true;

		return true;
	}
}