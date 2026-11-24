#include "IncubationFormats.h"
#include "ReaderUtils.h"
#include "MessageHandle.h"

namespace Incubation {
	void AssetInventory::ReadStandaloneFile(char* fileName) {
		if (fileName == nullptr) {
			printf("fileName == nullptr");
			return;
		}

		FILE* file = fopen(fileName, "rb");

		if (file == nullptr) {
			printf("file == nullptr");
			return;
		}

		fseek(file, 0L, SEEK_END);
		int32_t length = ftell(file);
		fseek(file, 0L, SEEK_SET);

		char* buffer = (char*)malloc(length);
		if (buffer == nullptr) {
			printf("buffer == nullptr");
			return;
		}
		fread(buffer, 1, length, file);

		fclose(file);

		ReadFromData(buffer);

		printf("AssetInventory Loaded Correctly.");
	}

	void AssetInventory::ReadFromData(char* data) {
		if (data == nullptr) {
			FireMessage("data == nullptr", ErrorType_Error);
			return;
		}

		fileData = data;

		memcpy(&entryCount, fileData, 4);

		int startPos = 4;

		if (entryCount == 52) {
			int textLen = strlen(fileData + 4);
			int textRen = textLen % 4;
			textLen += 4 - textRen;
			printf("%d\n", textLen);
			memcpy(&entryCount, fileData + 4 + textLen, 4);
			startPos = 8 + textLen;
		}

		entryCount = flipEndian(entryCount);

		int entryPos = startPos;
		int namePos = startPos + (entryCount * 24);

		printf("%d entries\n", entryCount);

		for (int i = 0; i < entryCount; i++) {
			entries.emplace_back();
			nameEntries.emplace_back();

			memcpy(&entries[i].amidOffs, fileData + entryPos, 4);
			memcpy(&entries[i].filePathOffs, fileData + entryPos + 4, 4);
			memcpy(&entries[i].amidVersion, fileData + entryPos + 8, 4);
			memcpy(&entries[i].binaryFormatVersion, fileData + entryPos + 0xC, 4);
			memcpy(&entries[i].timeStamp, fileData + entryPos + 0x10, 8);

			entries[i].amidOffs = flipEndian(entries[i].amidOffs);
			entries[i].filePathOffs = flipEndian(entries[i].filePathOffs);
			entries[i].amidVersion = flipEndian(entries[i].amidVersion);
			entries[i].binaryFormatVersion = flipEndian_f32((char*)&entries[i].binaryFormatVersion);
			entries[i].timeStamp = flipEndian(entries[i].timeStamp);

			int amidLen = strlen(fileData + namePos + entries[i].amidOffs);
			int filePathLen = strlen(fileData + namePos + entries[i].filePathOffs);

			nameEntries[i].amid = (char*)malloc(amidLen + 1);
			nameEntries[i].filePath = (char*)malloc(filePathLen + 1);

			strcpy(nameEntries[i].amid, fileData + namePos + entries[i].amidOffs);
			strcpy(nameEntries[i].filePath, fileData + namePos + entries[i].filePathOffs);

			entryPos += 24;

			printf(AssetInventoryTABLEENTRYFORMAT, nameEntries[i].amid, entries[i].amidVersion, entries[i].timeStamp, nameEntries[i].filePath, entries[i].binaryFormatVersion);
		}
	}

	void AssetInventory::ExportToFile(char* fileName) {
		if (fileName == nullptr) {
			printf("fileName == nullptr");
			return;
		}

		FILE* wStrm = fopen(fileName, "w");

		fwrite(AssetInventoryVERSION, strlen(AssetInventoryVERSION), 1, wStrm);
		fwrite("\n", 1, 1, wStrm);
		fwrite(AssetInventoryDEVROOT, strlen(AssetInventoryDEVROOT), 1, wStrm);
		fwrite("\n", 1, 1, wStrm);
		fwrite(AssetInventoryTABLEHEADER, strlen(AssetInventoryTABLEHEADER), 1, wStrm);
		fwrite("\n", 1, 1, wStrm);

		for (int i = 0; i < entryCount; i++) {
			char buffer[1024] = { 0 };
			sprintf(buffer, AssetInventoryTABLEENTRYFORMAT, nameEntries[i].amid, entries[i].amidVersion, entries[i].timeStamp, nameEntries[i].filePath, entries[i].binaryFormatVersion);
			fwrite(buffer, strlen(buffer), 1, wStrm);
		}

		fflush(wStrm);
		fclose(wStrm);
	}
}