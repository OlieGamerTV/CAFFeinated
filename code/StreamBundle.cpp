#include "imgui_includes.h"
#include "CommonReader.h"
#include "StreamBundle.h"

void StreamBundle::ClearActiveData() {
	if (bundleFiles != nullptr) {
		for (int i = 0; i < header.totalFileTotal; i++) {
			if (bundleFiles[i].entryType == ENTRY_BUNDLE) {
				bundleFiles[i].bundleFile.bundleData = nullptr;
			}
		}
		delete[] bundleFiles;
	}

	if (header.referenceTable != nullptr) {
		delete[] header.referenceTable;
	}

	if (fileEntries != nullptr) {
		delete[] fileEntries;
	}

	if (fileData != nullptr) {
		free(fileData);
		fileData = nullptr;
	}
}

bool StreamBundle::readStandaloneStreamBundleFile(char* fileName) {

	FILE* currentFile = fopen(fileName, "rb");

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

	return readStreamBundleFile(data);
}

bool StreamBundle::readStreamBundleFile(char* data) {
	if (data == nullptr) return false;

	fileData = data;
	int32_t offsetVar = 0xC;
	int32_t totalFileTotalVar = 0;
	uint32_t timestampVar = 0;

	int32_t referenceTableCountVar = 0;

	memcpy(&offsetVar, data + 4, sizeof(int32_t));
	memcpy(&totalFileTotalVar, data + 8, sizeof(int32_t));
	memcpy(&timestampVar, data + 0xC, sizeof(int32_t));
	memcpy(&referenceTableCountVar, data + 0x10, sizeof(int32_t));

	header.offset = flipEndian(offsetVar);
	header.totalFileTotal = flipEndian(totalFileTotalVar);
	header.timestamp = flipEndian(timestampVar);
	header.referenceTableCount = flipEndian(referenceTableCountVar);

	header.referenceTable = new uint32_t[header.referenceTableCount];

	printf("Total References %d\n", header.referenceTableCount);

	int32_t pos = 0x14;
	for (int32_t i = 0; i < header.referenceTableCount; i++) {
		int32_t refAid = 0;
		memcpy(&refAid, data + pos, sizeof(int32_t));
		header.referenceTable[i] = flipEndian(refAid);
		pos += 4;

		printf("Ref %d - %08x (Bundle\\%02x\\%02x%02x%02x)\n", i, header.referenceTable[i], (header.referenceTable[i] >> 24) & 0xFF, (header.referenceTable[i] >> 16) & 0xFF, (header.referenceTable[i] >> 8) & 0xFF, header.referenceTable[i] & 0xFF);
	}

	fileEntries = new StreamFileEntry[header.totalFileTotal];

	pos = 0x14 + (header.referenceTableCount * 4);
	for (int32_t i = 0; i < header.totalFileTotal; i++) {
		uint32_t aidVar;
		int32_t offsetVar;
		int32_t dataSizeVar;
		memcpy(&aidVar, data + pos, sizeof(int32_t));
		memcpy(&offsetVar, data + pos + 4, sizeof(int32_t));
		memcpy(&dataSizeVar, data + pos + 8, sizeof(int32_t));
		fileEntries[i].aid = flipEndian(aidVar);
		fileEntries[i].offset = flipEndian(offsetVar);
		fileEntries[i].dataSize = flipEndian(dataSizeVar);
		pos += 0xC;

		printf("File %08x (%d - %d)\n", fileEntries[i].aid, fileEntries[i].offset, fileEntries[i].dataSize);
	}

	bundleFiles = new StreamEntry[header.totalFileTotal];
	uint32_t magic = 0;
	char magicChar[5];

	try {
		for (int32_t i = 0; i < header.totalFileTotal; i++) {
			//char* bundleData = (char*)malloc(fileEntries[i].dataSize);

			memset(magicChar, 0, 5);

			memcpy(&magic, data + fileEntries[i].offset, sizeof(uint32_t));
			memcpy(&magicChar, data + fileEntries[i].offset, 4);

			printf("File %d Magic %08x (%s)\n", i, magic, magicChar);

			//memcpy(bundleData, data + fileEntries[i].offset, fileEntries[i].dataSize);
			if (magic == 0x57424E44) {
				bundleFiles[i].entryType = ENTRY_DNBW;
				bundleFiles[i].waveBankFile.ReadBankFile(data + fileEntries[i].offset);
			}
			else if (magic == 0x46464143) {
				bundleFiles[i].entryType = ENTRY_BUNDLE;
				bundleFiles[i].bundleFile.readBundleFileV0036(data + fileEntries[i].offset);
				//bundleData = nullptr;
			}
			else {
				bundleFiles[i].entryType = ENTRY_NONE;
			}

			/*if (bundleData != nullptr) {
				free(bundleData);
			}*/
		}
	}
	catch (std::exception e) {
		printf("A problem occured while reading the data. Error %s\n", e.what());
		return false;
	}
	catch (...) {
		printf("A problem occured while reading the data. Error %d\n", errno);
		return false;
	}
	

	printf("Finished reading StreamBundle file.\n");

	isReady = true;
	return true;
}

char* StreamBundle::getFileData(int32_t fileIdx) {

	if (fileData == nullptr) {
		printf("No file data has been supplied.\n");
		return NULL;
	}

	char* data = (char*)malloc(fileEntries[fileIdx].dataSize);

	memcpy(data, fileData + fileEntries[fileIdx].offset, fileEntries[fileIdx].dataSize);

	return data;
}

bool StreamBundle::addStreamedReference(bool* open) {
	bool ret = false;
	if (ImGui::BeginPopupModal("Add Reference", open, ImGuiWindowFlags_NoCollapse)) {
		uint32_t aid = 0x50000000;

		ImGui::InputScalar("Reference Aid", ImGuiDataType_U32, &aid, 0, 0, "%08X", ImGuiInputTextFlags_CharsHexadecimal);

		if (ImGui::Button("Add")) {
			uint32_t* tempTable = new uint32_t[header.referenceTableCount];
			for (int32_t i = 0; i < header.referenceTableCount; i++) {
				tempTable[i] = header.referenceTable[i];
			}
			header.referenceTableCount++;
			header.referenceTable = new uint32_t[header.referenceTableCount];
			for (int32_t i = 0; i < header.referenceTableCount - 1; i++) {
				header.referenceTable[i] = tempTable[i];
			}
			header.referenceTable[header.referenceTableCount - 1] = aid;
			*open = false;
			ImGui::CloseCurrentPopup();
			ret = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			*open = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	return ret;
}