#include "imgui_includes.h"
#include "CommonReader.h"
#include "StreamBundle.h"
#include "MessageHandle.h"

void StreamBundle::ClearActiveData() {
	if (fileData != nullptr) {
		free(fileData);
		fileData = nullptr;
	}

	bundleFiles.clear();
	header.referenceTable.clear();
	fileEntries.clear();
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

bool StreamBundle::readStreamBundleFileHeader(){
	if (fileData == nullptr) return false;

	uint32_t versionVar = 0;
	memcpy(&versionVar, fileData, sizeof(int32_t));
	header.version = flipEndian(versionVar);

	if (header.version == STREAMBUNDLE_IDENTIFIER_NOVVERSION) {
		int32_t offsetVar = 0xC;
		int32_t totalFileTotalVar = 0;
		uint32_t timestampVar = 0;
		int32_t referenceTableCountVar = 0;

		memcpy(&offsetVar, fileData + 4, sizeof(int32_t));
		memcpy(&totalFileTotalVar, fileData + 8, sizeof(int32_t));
		memcpy(&timestampVar, fileData + 0xC, sizeof(int32_t));
		memcpy(&referenceTableCountVar, fileData + 0x10, sizeof(int32_t));

		header.entrySize = flipEndian(offsetVar);
		header.numEntries = flipEndian(totalFileTotalVar);
		header.timestamp = flipEndian(timestampVar);
		header.numParents = flipEndian(referenceTableCountVar);
		return true;
	}
	if (header.version == STREAMBUNDLE_IDENTIFIER_SEPT18VERSION || header.version == STREAMBUNDLE_IDENTIFIER_SEPT20VERSION) {
		int32_t offsetVar = 0xC;
		int32_t totalFileTotalVar = 0;
		uint64_t timestampVar = 0;
		int32_t referenceTableCountVar = 0;

		memcpy(&offsetVar, fileData + 4, sizeof(int32_t));
		memcpy(&totalFileTotalVar, fileData + 8, sizeof(int32_t));
		memcpy(&timestampVar, fileData + 0x10, sizeof(int64_t));
		memcpy(&referenceTableCountVar, fileData + 0x18, sizeof(int32_t));

		header.entrySize = flipEndian(offsetVar);
		header.numEntries = flipEndian(totalFileTotalVar);
		header.timestamp = flipEndian(timestampVar);
		header.numParents = flipEndian(referenceTableCountVar);
		return true;
	}

	return false;
}

bool StreamBundle::readStreamBunfleFileEntries() {
	printf("Total References %d\n", header.numParents);

	int32_t pos = 0x14;
	if (header.version == STREAMBUNDLE_IDENTIFIER_NOVVERSION) {
		pos = 0x14;
	}
	if (header.version == STREAMBUNDLE_IDENTIFIER_SEPT18VERSION || header.version == STREAMBUNDLE_IDENTIFIER_SEPT20VERSION) {
		pos = 0x20;
	}

	for (int32_t i = 0; i < header.numParents; i++) {
		int32_t refAid = 0;
		memcpy(&refAid, fileData + pos, sizeof(int32_t));
		header.referenceTable.emplace_back(flipEndian(refAid));
		pos += 4;

		printf("Ref %d - %08x (Bundle\\%02x\\%02x%02x%02x)\n", i, header.referenceTable[i], (header.referenceTable[i] >> 24) & 0xFF, (header.referenceTable[i] >> 16) & 0xFF, (header.referenceTable[i] >> 8) & 0xFF, header.referenceTable[i] & 0xFF);
	}

	if (header.version == STREAMBUNDLE_IDENTIFIER_NOVVERSION) {
		pos = 0x14 + (header.numParents * 4);
	}
	if (header.version == STREAMBUNDLE_IDENTIFIER_SEPT18VERSION || header.version == STREAMBUNDLE_IDENTIFIER_SEPT20VERSION) {
		pos = 0x20 + (header.numParents * 4);
	}

	for (int32_t i = 0; i < header.numEntries; i++) {
		fileEntries.emplace_back();
		uint32_t aidVar;
		int32_t offsetVar;
		int32_t dataSizeVar;
		memcpy(&aidVar, fileData + pos, sizeof(int32_t));
		memcpy(&offsetVar, fileData + pos + 4, sizeof(int32_t));
		memcpy(&dataSizeVar, fileData + pos + 8, sizeof(int32_t));
		fileEntries[i].assetId = flipEndian(aidVar);
		fileEntries[i].startOffset = flipEndian(offsetVar);
		fileEntries[i].assetSize = flipEndian(dataSizeVar);
		pos += 0xC;

		printf("File %08x (%d - %d)\n", fileEntries[i].assetId, fileEntries[i].startOffset, fileEntries[i].assetSize);
	}

	uint32_t magic = 0;
	char magicChar[5];

	bundleFiles.reserve(header.numEntries);

	try {
		for (int32_t i = 0; i < header.numEntries; i++) {
			bundleFiles.emplace_back();
			memset(magicChar, 0, 5);

			memcpy(&magic, fileData + fileEntries[i].startOffset, sizeof(uint32_t));
			memcpy(&magicChar, fileData + fileEntries[i].startOffset, 4);

			printf("File %d Magic %08x (%s)\n", i, magic, magicChar);

			// Necessity due to now using std::vector.
			char* bundleData = (char*)malloc(fileEntries[i].assetSize);
			memcpy(bundleData, fileData + fileEntries[i].startOffset, fileEntries[i].assetSize);

			if (magic == 0x57424E44) {
				bundleFiles[i].entryType = ENTRY_DNBW;
				bundleFiles[i].waveBankFile.ReadBankFile(bundleData);
			}
			else if (magic == 0x46464143) {
				bundleFiles[i].entryType = ENTRY_BUNDLE;
				bundleFiles[i].bundleFile.ReadBundleFile(bundleData);
			}
			else {
				bundleFiles[i].entryType = ENTRY_NONE;
			}

			bundleData = nullptr;
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
}

bool StreamBundle::readStreamBundleFile(char* data) {
	if (data == nullptr) return false;
	fileData = data;

	if (!readStreamBundleFileHeader()) return false;
	if (!readStreamBunfleFileEntries()) return false;
	

	printf("Finished reading StreamBundle file.\n");

	isReady = true;
	return true;
}

char* StreamBundle::getFileData(int32_t fileIdx) {

	if (fileData == nullptr) {
		printf("No file data has been supplied.\n");
		return NULL;
	}

	char* data = (char*)malloc(fileEntries[fileIdx].assetSize);

	memcpy(data, fileData + fileEntries[fileIdx].startOffset, fileEntries[fileIdx].assetSize);

	return data;
}

bool StreamBundle::addStreamedReference(bool* open) {
	bool ret = false;
	if (ImGui::BeginPopupModal("Add Reference", open, ImGuiWindowFlags_NoCollapse)) {
		uint32_t aid = 0x50000000;

		ImGui::InputScalar("Reference Aid", ImGuiDataType_U32, &aid, 0, 0, "%08X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);

		if (ImGui::Button("Add")) {
			bool isAlreadyPresent = false;
			for (int i = 0; i < header.numParents; i++) {
				if (header.referenceTable[i] == aid) {
					isAlreadyPresent = true;
					break;
				}
			}

			if (!isAlreadyPresent) {
				header.referenceTable.emplace_back(aid);
				header.numParents++;

				*open = false;
				ImGui::CloseCurrentPopup();
				ret = true;
			}
			else {
				FireMessageF("The given hash %08X is already present as a reference.", ErrorType_Warn, aid);
			}
		}
		if (ImGui::Button("Remove")) {
			bool wasPresent = false;
			for (int i = 0; i < header.numParents; i++) {
				if (header.referenceTable[i] == aid) {
					header.referenceTable.erase(header.referenceTable.begin() + i);
					wasPresent = true;
					break;
				}
			}
			
			if (wasPresent) {
				*open = false;
				ImGui::CloseCurrentPopup();
				ret = true;
			}
			else {
				FireMessageF("The given hash %08X was not found as an existing reference.", ErrorType_Warn, aid);
			}
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