#include "CommonReader.h"
#include "PinataPKG.h"
#include "LoadingProcess.h"

namespace Pinata {
	void PKGFile::ClearActiveData() {
		if (entries != nullptr) {
			delete[] entries;
			entries = nullptr;
		}

		if (caffEntries != nullptr) {
			for (int i = 0; i < header.entryCount; i++) {
				if (caffEntries[i].isDirty) {
					free(caffEntries[i].bundleData);
				}
				else {
					caffEntries[i].bundleData = nullptr;
				}
			}
			delete[] caffEntries;
			caffEntries = nullptr;
		}

		if (data != nullptr) {
			free(data);
			data = nullptr;
		}

		isDirty = false;
	}

	bool PKGFile::readStandalonePkgFile(char* filePath) {
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

		if (fileData == nullptr) {
			printf("%s - fileData is null.\n", __func__);
			return false;
		}

		isBigEndian = false;
		data = fileData;

		memcpy(&header.version, data, sizeof(int32_t));
		memcpy(&header.entryCount, data + 4, sizeof(int32_t));

		if (header.version != PKG_VER1 && header.version != PKG_VER2) {
			isBigEndian = true;
			header.version = flipEndian(header.version);
			header.entryCount = flipEndian(header.entryCount);
		}

		if (header.version != PKG_VER1 && header.version != PKG_VER2) {
			printf("%s - Version number doesn't match known ones.\n", __func__);
			return false;
		}

		entries = new PKGFileEntry[header.entryCount];

		int currentPos = 8;

		for (int i = 0; i < header.entryCount; i++) {
			memcpy(&entries[i].id, data + currentPos, sizeof(int32_t));
			memcpy(&entries[i].offset, data + currentPos + 4, sizeof(int32_t));
			memcpy(&entries[i].size, data + currentPos + 8, sizeof(int32_t));

			if (isBigEndian) {
				entries[i].id = flipEndian(entries[i].id);
				entries[i].offset = flipEndian(entries[i].offset);
				entries[i].size = flipEndian(entries[i].size);
			}

			currentPos += 0xC;
		}

		caffEntries = new BundleV36[header.entryCount];

		for (int i = 0; i < header.entryCount; i++) {
			caffEntries[i].readBundleFileV0036(data + entries[i].offset);
		}

		return true;
	}

	char* PKGFile::GetFileData(int32_t fileIdx) {
		if (fileIdx < 0 || fileIdx >= header.entryCount) return nullptr;

		char* fileData = (char*)malloc(entries[fileIdx].size);

		memcpy(fileData, data + entries[fileIdx].offset, entries[fileIdx].size);

		return fileData;
	}

	void PKGFile::writePinataPKG(const char* outPath) {
		if (outPath == nullptr || data == nullptr) return;

		FILE* writeStrm = fopen(outPath, "wb");

		if (writeStrm == nullptr) {
			printf("An error occured while trying to open the stream. Error Code 0x%08X.\n", errno);
			return;
		}

		// Buffer allocation.
		char emptyBuff[2048];
		memset(emptyBuff, 0, 2048);

		int eof = 0xFF;

		int version = header.version;
		int entryCount = header.entryCount;

		if (isBigEndian) {
			version = flipEndian(version);
			entryCount = flipEndian(entryCount);
		}

		fwrite(&version, sizeof(int32_t), 1, writeStrm);
		fwrite(&entryCount, sizeof(int32_t), 1, writeStrm);

		for (int i = 0; i < header.entryCount; i++) {
			int id = entries[i].id;
			int offset = entries[i].offset;
			int size = caffEntries[i].getTotalSizeOfCompedBundle();

			if (isBigEndian) {
				id = flipEndian(id);
				offset = flipEndian(offset);
				size = flipEndian(size);
			}

			fwrite(&id, sizeof(int32_t), 1, writeStrm);
			fwrite(&offset, sizeof(int32_t), 1, writeStrm);
			fwrite(&size, sizeof(int32_t), 1, writeStrm);
		}

		fwrite(&eof, sizeof(int32_t), 1, writeStrm);

		int remain = (0xC + (header.entryCount * 0xC)) % 2048;
		int paddSize = 2048 - remain;

		printf("2048 - %d = %d\n", 0xC + (header.entryCount * 0xC), paddSize);

		fwrite(&emptyBuff, 1, paddSize, writeStrm);

		for (int i = 0; i < header.entryCount; i++) {
			int size = caffEntries[i].getTotalSizeOfCompedBundle();
			remain = size % 2048;

			paddSize = (2048 - remain) - sizeof(int32_t);
			
			fwrite(caffEntries[i].bundleData, 1, size, writeStrm);
			fwrite(&eof, sizeof(int32_t), 1, writeStrm);
			fwrite(&emptyBuff, 1, paddSize, writeStrm);

			printf("2048 - %d = %d\n", remain, paddSize);
			IncreaseCurrentSavedOnLoadingWidget();
		}

		fflush(writeStrm);
		fclose(writeStrm);
	}
}