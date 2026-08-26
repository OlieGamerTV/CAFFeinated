#pragma once

#include "AssetCommons.h"
#include "CommonReader.h"
#include "Bundle.h"

bool BundleFile::ReadBundleFile(char* data) {
	if (data == nullptr) {
		printf("Passed data array is null.\n");
		return false;
	}

	char caffMagic[4] = { 0 };
	char caffVer[0x10] = { 0 };

	memset(caffMagic, 0, 4);
	memset(caffVer, 0, 0x10);

	if (memcpy(caffMagic, data, 0x4) == nullptr) {
		printf("Error occured while trying to read the file.\n");
		return false;
	}

	if (memcpy(caffVer, data + 4, 0x10) == nullptr) {
		printf("Error occured while trying to read the file.\n");
		return false;
	}

	if (strncmp(caffMagic, "CAFF", 4) != 0) {
		printf("The file supplied is not a valid bundle file.\n");
		return false;
	}


	if (strncmp(caffVer, "24.09.03.0026", 0x10) == 0) {
		if (V26Bundle == nullptr) {
			V26Bundle = new BundleV26();
		}
		
		return V26Bundle->readBundleFileV0026(data);
	}

	if (strncmp(caffVer, "28.01.05.0031", 0x10) == 0) {
		if (V31Bundle == nullptr) {
			V31Bundle = new BundleV31();
		}

		return V31Bundle->readBundleFileV0031(data);
	}

	if (strncmp(caffVer, "07.08.06.0036", 0x10) == 0) {
		if (V36Bundle == nullptr) {
			V36Bundle = new BundleV36();
		}

		return V36Bundle->readBundleFileV0036(data);
	}

	if (strncmp(caffVer, "08.11.07.0040", 0x10) == 0) {
		if (V40Bundle == nullptr) {
			V40Bundle = new BundleV40();
		}

		return V40Bundle->readBundleFileV0040(data);
	}

	printf("The file supplied is not a supported bundle file.\n");
	return false;
}

void BundleFile::ClearActiveBundleFile() {
	if (V40Bundle != nullptr) {
		for (int i = 0; i < V40Bundle->header.numAssets; i++) {
			if (V40Bundle->sectionTable.fileLabelTable.fileLabels[i] != nullptr) {
				free(V40Bundle->sectionTable.fileLabelTable.fileLabels[i]);
				V40Bundle->sectionTable.fileLabelTable.fileLabels[i] = nullptr;
			}
		}
		delete V40Bundle;
		V40Bundle = nullptr;
	}

	if (V36Bundle != nullptr) {
		for (int i = 0; i < V36Bundle->header.numAssets; i++) {
			if (V36Bundle->sectionTable.fileLabelTable.fileLabels[i] != nullptr) {
				free(V36Bundle->sectionTable.fileLabelTable.fileLabels[i]);
				V36Bundle->sectionTable.fileLabelTable.fileLabels[i] = nullptr;
			}
		}
		delete V36Bundle;
		V36Bundle = nullptr;
	}

	if (V31Bundle != nullptr) {
		delete V31Bundle;
		V31Bundle = nullptr;
	}

	if (V26Bundle != nullptr) {
		delete V26Bundle;
		V26Bundle = nullptr;
	}
}

SectionTable::~SectionTable() {
	if (fileLabelTable.fileLabels != nullptr) {
		free(fileLabelTable.fileLabels);
		fileLabelTable.fileLabels = nullptr;
	}
}

bool BundleV40::readBundleHeaderV0040(char* data) {
	if (data == nullptr) {
		printf("Passed data array is null.\n");
		return false;
	}

	char caffVer[0x10];

	int32_t offset = 0;
	bool isValidVersion = false;

	if (memcpy(caffVer, data + 4, 0x10) == nullptr) {
		printf("Error occured while trying to read the file.\n");
		return false;
	}

	//printf("Bundle Version: %s\n", caffVer);
	strcpy(header.versionString, caffVer);

	if (strcmp(header.versionString, "08.11.07.0040") == 0) {
		printf("The bundle file provided is a Kinect Sports bundle.\n");
		isValidVersion = true;
	}

	if (!isValidVersion) {
		printf("The given file is not a valid or supported bundle file.\n");
		return false;
	}

	try {
		printf("CAFF Header - {\n");
		// Now we read the important information.

		memcpy(&header.timestamp, data + 0x18, sizeof(int32_t));
		memcpy(&header.headerSize, data + 0x1C, sizeof(int32_t));
		memcpy(&header.headerHash, data + 0x20, sizeof(int32_t));
		memcpy(&header.numAssets, data + 0x24, sizeof(int32_t));
		memcpy(&header.numSections, data + 0x28, sizeof(int32_t));

		memcpy(&header.numRelocationGroups, data + 0x2C, sizeof(int32_t));
		memcpy(&header.numByteswaps, data + 0x30, sizeof(int32_t));
		memcpy(&header.numRelocationGroups, data + 0x34, sizeof(int32_t));
		memcpy(&header.numRelocations, data + 0x38, sizeof(int32_t));
		memcpy(&header.numMappedByteswapGroups, data + 0x3C, sizeof(int32_t));
		memcpy(&header.numMappedByteswaps, data + 0x40, sizeof(int32_t));
		memcpy(&header.numMappedRelocationGroups, data + 0x44, sizeof(int32_t));
		memcpy(&header.numMappedRelocations, data + 0x48, sizeof(int32_t));
		memcpy(&header.numPoolItems, data + 0x4C, sizeof(int32_t));

		memcpy(&header.byteswapFlags, data + 0x50, sizeof(char));
		memcpy(&header.numSectionTypes, data + 0x51, sizeof(char));
		memcpy(&header.compression, data + 0x52, sizeof(char));
		memcpy(&header.numPools, data + 0x53, sizeof(char));
		memcpy(&header.sectionTypeNamesBufferLen, data + 0x54, sizeof(int32_t)); // 0x4C

		memcpy(&header.sectTable.size, data + 0x58, sizeof(int32_t));
		memcpy(&header.sectTable.base, data + 0x5C, sizeof(int32_t));
		memcpy(&header.sectTable.overlap, data + 0x60, sizeof(int32_t));
		memcpy(&header.sectTable.loadSize, data + 0x64, sizeof(int32_t));
		memcpy(&header.sectTable.compressedSize, data + 0x68, sizeof(int32_t));

		memcpy(&header.fileTable.size, data + 0x6C, sizeof(int32_t));
		memcpy(&header.fileTable.base, data + 0x70, sizeof(int32_t));
		memcpy(&header.fileTable.overlap, data + 0x74, sizeof(int32_t));
		memcpy(&header.fileTable.loadSize, data + 0x78, sizeof(int32_t));
		memcpy(&header.fileTable.compressedSize, data + 0x7C, sizeof(int32_t));

		printf("\tCRC - %08X, NUM SYMBOLS & NUM FILES[%d %d -> %d %d]\n", flipEndian(header.headerHash), header.numAssets, header.numSections, flipEndian(header.numAssets), flipEndian(header.numSections));

		// For Big Endian Files, this will almost always be set to 1.
		if (header.byteswapFlags == 1) {
			header.sectionTypeNamesBufferLen = flipEndian(header.sectionTypeNamesBufferLen);

			header.timestamp = flipEndian(header.timestamp); // Flip from big endian to little endian.
			header.headerSize = flipEndian(header.headerSize); // Flip from big endian to little endian.
			header.headerHash = flipEndian(header.headerHash); // Flip from big endian to little endian.
			header.numAssets = flipEndian(header.numAssets); // Flip from big endian to little endian.
			header.numSections = flipEndian(header.numSections); // Flip from big endian to little endian.

			// Flip all applicable values from big endian to little endian.
			header.sectTable.size = flipEndian(header.sectTable.size);
			header.sectTable.compressedSize = flipEndian(header.sectTable.compressedSize);
			header.fileTable.size = flipEndian(header.fileTable.size);
			header.fileTable.compressedSize = flipEndian(header.fileTable.compressedSize);
		}
		printf("}\n");
	}
	catch (int32_t ex) {
	}

	return true;
}

bool BundleV40::readBundleSectionFileV0040(char* data) {
	if (data == nullptr) {
		printf("Passed data array is null.\n");
		return false;
	}

	try {
		printf("CAFF Sections - {\n");

		// Initialize section table arrays.
		sectionTable.fileLabelOffsets = new int32_t[header.numAssets];
		sectionTable.fileInfos = new FileInfoEntry[header.numSections];
		sectionTable.fileLabelTable.fileLabels = new char* [header.numAssets];

		char* uncompedBaseData = (char*)malloc(header.headerSize + header.sectTable.size + header.fileTable.size);
		memset(uncompedBaseData, 0, header.headerSize + header.sectTable.size + header.fileTable.size);

		if (header.compression == 1) {
			memcpy(uncompedBaseData, data, header.headerSize);

			// Do the section table first
			char* in = (char*)malloc(header.sectTable.compressedSize);

			memcpy(in, data + header.headerSize, header.sectTable.compressedSize);

			char* out = InflateData(in, 6, header.sectTable.compressedSize, header.sectTable.size);

			memcpy(uncompedBaseData + header.headerSize, out, header.sectTable.size);

			free(out);

			// And now we do the file table.
			in = (char*)malloc(header.fileTable.compressedSize);

			memcpy(in, data + header.headerSize + header.sectTable.compressedSize, header.fileTable.compressedSize);

			out = InflateData(in, 6, header.fileTable.compressedSize, header.fileTable.size);

			memcpy(uncompedBaseData + header.headerSize + header.sectTable.size, out, header.fileTable.size);

			free(out);
		}
		else {
			memcpy(uncompedBaseData, data, header.headerSize + header.sectTable.size + header.fileTable.size);
		}

		// Section Time
		int32_t sectPos = header.headerSize;
		for (int32_t i = 0; i < header.numSectionTypes; i++) {
			int32_t strOffset = 0;
			int32_t unk2 = 0;
			int32_t uncompressedSize = 0;
			int32_t compressedSize = 0;

			memcpy(&sectionTable.entries[i].alignment, uncompedBaseData + (sectPos + 4), sizeof(char)); // 0x4

			if (header.byteswapFlags == 0) {
				memcpy(&sectionTable.entries[i].strOffset, uncompedBaseData + (sectPos), sizeof(int32_t)); // 0x0
				memcpy(&sectionTable.entries[i].poolSection, uncompedBaseData + (sectPos + 5), sizeof(int32_t)); // 0x5
				memcpy(&sectionTable.entries[i].size, uncompedBaseData + (sectPos + 9), sizeof(int32_t)); // 0x9
				memcpy(&sectionTable.entries[i].compressedSize, uncompedBaseData + (sectPos + 0x1D), sizeof(int32_t)); // 0x1D
			}
			else if (header.byteswapFlags == 1) {
				memcpy(&strOffset, uncompedBaseData + (sectPos), sizeof(int32_t)); // 0x0
				memcpy(&unk2, uncompedBaseData + (sectPos + 5), sizeof(int32_t)); // 0x5
				memcpy(&uncompressedSize, uncompedBaseData + (sectPos + 9), sizeof(int32_t)); // 0x9
				memcpy(&compressedSize, uncompedBaseData + (sectPos + 0x1D), sizeof(int32_t)); // 0x1D

				// Flip all applicable values from big endian to little endian.
				sectionTable.entries[i].strOffset = flipEndian(strOffset);
				sectionTable.entries[i].poolSection = flipEndian(unk2);
				sectionTable.entries[i].size = flipEndian(uncompressedSize);
				sectionTable.entries[i].compressedSize = flipEndian(compressedSize);
			}

			sectPos += 0x21;

			printf("\tSECT INFO %u [%u %u UNCOMPED SIZE - %u, COMPEDSIZE - %u]\n", i, sectionTable.entries[i].strOffset, sectionTable.entries[i].poolSection, sectionTable.entries[i].size, sectionTable.entries[i].compressedSize);
		}

		// Section Labels
		int32_t baseOffsetForSectLabels = header.headerSize + (0x21 * header.numSectionTypes);
		sectionTable.sectionLabelOffset = baseOffsetForSectLabels;
		for (int32_t i = 0; i < header.numSectionTypes; i++) {
			strcpy(sectionTable.sectionLabels[i].label, uncompedBaseData + (baseOffsetForSectLabels + sectionTable.entries[i].strOffset));

			//printf("SECT LABEL %u [%s]\n", i, sectionTable.sectionLabels[i].label);
		}

		int32_t baseOffsetForFileLabelOffsets = header.headerSize + (0x21 * header.numSectionTypes) + header.sectionTypeNamesBufferLen;
		sectionTable.sectionFileLabelOffset = baseOffsetForFileLabelOffsets;


		// Symbol time
		int32_t totalTableSize = 0;
		if (header.byteswapFlags == 0) {
			memcpy(&sectionTable.fileLabelTable.totalLabelTableSize, uncompedBaseData + baseOffsetForFileLabelOffsets, sizeof(int32_t)); // 0x00
		}
		else if (header.byteswapFlags == 1) {
			memcpy(&totalTableSize, uncompedBaseData + baseOffsetForFileLabelOffsets, sizeof(int32_t)); // 0x00
			sectionTable.fileLabelTable.totalLabelTableSize = flipEndian(totalTableSize);
		}

		for (int32_t i = 0; i < header.numAssets; i++) {
			int32_t offset = 0;

			if (header.byteswapFlags == 0) {
				memcpy(&sectionTable.fileLabelOffsets[i], uncompedBaseData + (baseOffsetForFileLabelOffsets + (i * 4)) + 4, sizeof(int32_t));
			}
			else if (header.byteswapFlags == 1) {
				memcpy(&offset, uncompedBaseData + (baseOffsetForFileLabelOffsets + (i * 4)) + 4, sizeof(int32_t));
				sectionTable.fileLabelOffsets[i] = flipEndian(offset);
			}

			//printf("Symbol %d offset %d\n", i, sectionTable.fileLabelOffsets[i]);
		}

		int32_t baseOffsetForFileLabels = baseOffsetForFileLabelOffsets + 4 + (header.numAssets * 4);


		for (int32_t i = 0; i < header.numAssets; i++) {
			size_t strLen = strlen(uncompedBaseData + (baseOffsetForFileLabels + sectionTable.fileLabelOffsets[i])) + 1;
			sectionTable.fileLabelTable.fileLabels[i] = (char*)malloc(128);
			memset(sectionTable.fileLabelTable.fileLabels[i], 0, 128);

			strcpy(sectionTable.fileLabelTable.fileLabels[i], uncompedBaseData + (baseOffsetForFileLabels + sectionTable.fileLabelOffsets[i]));
			//printf("Symbol %d - %s\n", i, sectionTable.fileLabelTable.fileLabels[i].label);
		}

		int32_t adbLen = 0;
		if (header.byteswapFlags == 0) {
			memcpy(&sectionTable.adbStringLen, uncompedBaseData + baseOffsetForFileLabels + sectionTable.fileLabelTable.totalLabelTableSize, sizeof(int32_t)); // 0x0
		}
		else if (header.byteswapFlags == 1) {
			memcpy(&adbLen, uncompedBaseData + baseOffsetForFileLabels + sectionTable.fileLabelTable.totalLabelTableSize, sizeof(int32_t)); // 0x0
			sectionTable.adbStringLen = flipEndian(adbLen);
		}

		if (sectionTable.adbStringLen != 0) {
			sectionTable.adbString = new char[sectionTable.adbStringLen];
			memset(sectionTable.adbString, 0, sectionTable.adbStringLen);

			strncpy(sectionTable.adbString, uncompedBaseData + baseOffsetForFileLabels + sectionTable.fileLabelTable.totalLabelTableSize + 4, sectionTable.adbStringLen);
			printf("\tADB %d %s\n", sectionTable.adbStringLen, sectionTable.adbString);
		}

		int32_t baseOffsetForFileInfos = baseOffsetForFileLabels + sectionTable.fileLabelTable.totalLabelTableSize + 4 + sectionTable.adbStringLen;
		sectionTable.fileInfosOffset = baseOffsetForFileInfos;

		int fileInfoDataSize = 0xE;

		uint8_t bit = 0;
		memcpy(&bit, uncompedBaseData + baseOffsetForFileInfos + 0xC, sizeof(char)); // 0xC

		// Check for KS:S2 junk
		if (bit > 0x5)
			fileInfoDataSize = 0x12;

		//printf("File Info Offset - %d\n", sectionTable.fileInfosOffset);
		for (int32_t i = 0; i < header.numSections; i++) {
			int32_t infoID = 0;
			int32_t infoDataOffset = 0;
			int32_t infoDataSize = 0;

			if (header.byteswapFlags == 0) {
				memcpy(&sectionTable.fileInfos[i].asset, uncompedBaseData + baseOffsetForFileInfos, sizeof(int32_t)); // 0x0
				memcpy(&sectionTable.fileInfos[i].start, uncompedBaseData + baseOffsetForFileInfos + 4, sizeof(int32_t)); // 0x4
				memcpy(&sectionTable.fileInfos[i].size, uncompedBaseData + baseOffsetForFileInfos + 8, sizeof(int32_t)); // 0x8
			}
			else if (header.byteswapFlags == 1) {
				memcpy(&infoID, uncompedBaseData + baseOffsetForFileInfos, sizeof(int32_t)); // 0x0
				memcpy(&infoDataOffset, uncompedBaseData + baseOffsetForFileInfos + 4, sizeof(int32_t)); // 0x4
				memcpy(&infoDataSize, uncompedBaseData + baseOffsetForFileInfos + 8, sizeof(int32_t)); // 0x8
				sectionTable.fileInfos[i].asset = flipEndian(infoID);
				sectionTable.fileInfos[i].start = flipEndian(infoDataOffset);
				sectionTable.fileInfos[i].size = flipEndian(infoDataSize);
			}

			if (bit > 0x5) {
				memcpy(&sectionTable.fileInfos[i].sectionType, uncompedBaseData + baseOffsetForFileInfos + 0x10, sizeof(char)); // 0xC
				memcpy(&sectionTable.fileInfos[i].startAlignment, uncompedBaseData + baseOffsetForFileInfos + 0x11, sizeof(char)); // 0xD
			}
			else {
				memcpy(&sectionTable.fileInfos[i].sectionType, uncompedBaseData + baseOffsetForFileInfos + 0xC, sizeof(char)); // 0xC
				memcpy(&sectionTable.fileInfos[i].startAlignment, uncompedBaseData + baseOffsetForFileInfos + 0xD, sizeof(char)); // 0xD
			}

			baseOffsetForFileInfos += fileInfoDataSize;

			printf("\tFILE INFO %u [%u %u %u %u]\n", sectionTable.fileInfos[i].asset, sectionTable.fileInfos[i].start, sectionTable.fileInfos[i].size, sectionTable.fileInfos[i].sectionType, sectionTable.fileInfos[i].startAlignment);
		}

		printf("}\n");

		free(uncompedBaseData);
		uncompedBaseData = nullptr;
	}
	catch (...) {
		printf("An error occured while reading the provided bundle file. Error Code %d\n", errno);
		return false;
	}

	return true;
}

bool BundleV40::readBundleFileV0040(char* data) {

	int32_t end = SRC_ENDIANLITTLE;

	if (data == nullptr) {
		printf("Passed data array is null.\n");
		return false;
	}

	bundleData = data;

	try
	{
		if (!readBundleHeaderV0040(data)) {
			printf("A problem occurred while parsing the bundle header.\n");

			return false;
		}

		if (!readBundleSectionFileV0040(data)) {
			printf("A problem occurred while parsing the bundle section and file tables.\n");
			return false;
		}

		isReady = true;
		printf("The bundle file has finished reading.\n");
	}
	catch (int32_t e) {
		printf("An error occured while reading the provided bundle file.\n");
	}

	//free(data);

	return true;
}

BundleV40::~BundleV40() {
	if (bundleData != nullptr) {
		free(bundleData);
		bundleData = nullptr;
	}
}

bool BundleV40::readStandaloneBundleFile(char* fileName) {
	isReady = false;

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

	readBundleFileV0040(data);
	return true;
}

char* BundleV40::getFileData(char* fileName, int32_t fileInfoIdx) {
	if (bundleData == nullptr) {
		printf("Error occured while trying to open the file.\n");
		return NULL;
	}

	int32_t sectionOffset = getOffsetOfSection(sectionTable.fileInfos[fileInfoIdx].sectionType - 1);

	int32_t totalOffsetToDataSect = header.headerSize + header.sectTable.size + header.fileTable.size;

	//Initialize our new section.
	char* sect = (char*)malloc(sectionTable.fileInfos[fileInfoIdx].size);
	if (sect == nullptr) {
		throw("Memory Allocation Error for the section.");
		return NULL;
	}

	memset(sect, 0, sectionTable.fileInfos[fileInfoIdx].size);

	printf("Bundle::getFileData() - Allocating file info %d in section %s of size %d.\n", fileInfoIdx, sectionTable.sectionLabels[sectionTable.fileInfos[fileInfoIdx].sectionType - 1].label, sectionTable.fileInfos[fileInfoIdx].size);

	// Just do this in a completely different way
	if (header.compression) {
		totalOffsetToDataSect = header.headerSize + header.sectTable.compressedSize + header.fileTable.compressedSize;

		size_t sectCompedSize = getCompressedSizeofSection(sectionTable.fileInfos[fileInfoIdx].sectionType - 1);
		size_t sectUncompedSize = getUncompressedSizeofSection(sectionTable.fileInfos[fileInfoIdx].sectionType - 1);
		char* data = (char*)malloc(sectCompedSize);
		if (data == nullptr) {
			throw("Memory Allocation Error for the compression code with the compressed memory.");
			return NULL;
		}

		memcpy_s(data, sectCompedSize, bundleData + totalOffsetToDataSect + sectionOffset, sectCompedSize);

		char* outputData = InflateData(data, 6, sectCompedSize, sectUncompedSize);

		memcpy_s(sect, sectionTable.fileInfos[fileInfoIdx].size, outputData + sectionTable.fileInfos[fileInfoIdx].start, sectionTable.fileInfos[fileInfoIdx].size);

		return sect;
	}

	memcpy_s(sect, sectionTable.fileInfos[fileInfoIdx].size, (bundleData + totalOffsetToDataSect + sectionOffset + sectionTable.fileInfos[fileInfoIdx].start), sectionTable.fileInfos[fileInfoIdx].size);

	printf("Bundle::getFileData() - File Data for entry %d successfully obtained.\n", fileInfoIdx);

	return sect;
}

bool BundleV36::readBundleHeaderV0036(char* data) {
	if (data == nullptr) {
		printf("Passed data array is null.\n");
		return false;
	}

	char caffVer[0x10];

	int32_t offset = 0;
	bool isValidVersion = false;

	if (memcpy(caffVer, data + 4, 0x10) == nullptr) {
		printf("Error occured while trying to read the file.\n");
		return false;
	}

	//printf("Bundle Version: %s\n", caffVer);
	strcpy(header.versionString, caffVer);

	if (strcmp(header.versionString, "07.08.06.0036") == 0) {
		printf("The bundle file provided is a Viva Pinata or Nuts & Bolts file.\n");
		isValidVersion = true;
	}

	if (!isValidVersion) {
		printf("The given file is not a valid or supported bundle file.\n");
		return false;
	}

	try {
		printf("CAFF Header - {\n");
		// Now we read the important information.

		memcpy(&header.headerSize, data + 0x14, sizeof(int32_t)); // 0x14
		memcpy(&header.headerHash, data + 0x18, sizeof(int32_t)); // 0x18
		memcpy(&header.numAssets, data + 0x1C, sizeof(int32_t)); // 0x1C
		memcpy(&header.numSections, data + 0x20, sizeof(int32_t)); // 0x20

		memcpy(&header.byteswapFlags, data + 0x48, sizeof(char));
		memcpy(&header.numSectionTypes, data + 0x49, sizeof(char));
		memcpy(&header.compression, data + 0x4A, sizeof(char));
		memcpy(&header.numPools, data + 0x4B, sizeof(char));

		memcpy(&header.sectionTypeNamesBufferLen, data + 0x4C, sizeof(int32_t)); // 0x4C

		memcpy(&header.sectTable.size, data + 0x50, sizeof(int32_t));
		memcpy(&header.sectTable.base, data + 0x54, sizeof(int32_t));
		memcpy(&header.sectTable.overlap, data + 0x58, sizeof(int32_t));
		memcpy(&header.sectTable.loadSize, data + 0x5C, sizeof(int32_t));
		memcpy(&header.sectTable.compressedSize, data + 0x60, sizeof(int32_t));

		memcpy(&header.fileTable.size, data + 0x64, sizeof(int32_t));
		memcpy(&header.fileTable.base, data + 0x68, sizeof(int32_t));
		memcpy(&header.fileTable.overlap, data + 0x6C, sizeof(int32_t));
		memcpy(&header.fileTable.loadSize, data + 0x70, sizeof(int32_t));
		memcpy(&header.fileTable.compressedSize, data + 0x74, sizeof(int32_t));

		printf("\tCRC - %08X, NUM SYMBOLS & NUM FILES[%d %d -> %d %d]\n", flipEndian(header.headerHash), header.numAssets, header.numSections, flipEndian(header.numAssets), flipEndian(header.numSections));

		// For Big Endian Files, this will almost always be set to 1.
		if (header.byteswapFlags == 1) {
			header.sectionTypeNamesBufferLen = flipEndian(header.sectionTypeNamesBufferLen);

			header.headerSize = flipEndian(header.headerSize); // Flip from big endian to little endian.
			header.headerHash = flipEndian(header.headerHash); // Flip from big endian to little endian.
			header.numAssets = flipEndian(header.numAssets); // Flip from big endian to little endian.
			header.numSections = flipEndian(header.numSections); // Flip from big endian to little endian.

			// Flip all applicable values from big endian to little endian.
			header.sectTable.size = flipEndian(header.sectTable.size);
			header.sectTable.compressedSize = flipEndian(header.sectTable.compressedSize);
			header.fileTable.size = flipEndian(header.fileTable.size);
			header.fileTable.compressedSize = flipEndian(header.fileTable.compressedSize);
		}

		printf("}\n");
	}
	catch (int32_t ex) {
	}

	return true;
}

bool BundleV36::readBundleSectionFileV0036(char* data) {
	if (data == nullptr) {
		printf("Passed data array is null.\n");
		return false;
	}

	try{
		printf("CAFF Sections - {\n");

		// Initialize section table arrays.
		sectionTable.fileLabelOffsets = new int32_t[header.numAssets];
		sectionTable.fileInfos = new FileInfoEntry[header.numSections];
		sectionTable.fileLabelTable.fileLabels = new char*[header.numAssets];

		char* uncompedBaseData = (char*)malloc(header.headerSize + header.sectTable.size + header.fileTable.size);
		memset(uncompedBaseData, 0, header.headerSize + header.sectTable.size + header.fileTable.size);

		if (header.compression == 1) {
			memcpy(uncompedBaseData, data, header.headerSize);

			// Do the section table first
			char* in = (char*)malloc(header.sectTable.compressedSize);

			memcpy(in, data + header.headerSize, header.sectTable.compressedSize);

			char* out = InflateData(in, 6, header.sectTable.compressedSize, header.sectTable.size);

			memcpy(uncompedBaseData + header.headerSize, out, header.sectTable.size);

			free(out);

			// And now we do the file table.
			in = (char*)malloc(header.fileTable.compressedSize);

			memcpy(in, data + header.headerSize + header.sectTable.compressedSize, header.fileTable.compressedSize);

			out = InflateData(in, 6, header.fileTable.compressedSize, header.fileTable.size);

			memcpy(uncompedBaseData + header.headerSize + header.sectTable.size, out, header.fileTable.size);

			free(out);
		}
		else {
			memcpy(uncompedBaseData, data, header.headerSize + header.sectTable.size + header.fileTable.size);
		}

		// Section Time
		int32_t sectPos = header.headerSize;
		for (int32_t i = 0; i < header.numSectionTypes; i++) {
			int32_t strOffset = 0;
			int32_t unk2 = 0;
			int32_t uncompressedSize = 0;
			int32_t compressedSize = 0;

			memcpy(&sectionTable.entries[i].alignment, uncompedBaseData + (sectPos + 4), sizeof(char)); // 0x4

			if (header.byteswapFlags == 0) {
				memcpy(&sectionTable.entries[i].strOffset, uncompedBaseData + (sectPos), sizeof(int32_t)); // 0x0
				memcpy(&sectionTable.entries[i].poolSection, uncompedBaseData + (sectPos + 5), sizeof(int32_t)); // 0x5
				memcpy(&sectionTable.entries[i].size, uncompedBaseData + (sectPos + 9), sizeof(int32_t)); // 0x9
				memcpy(&sectionTable.entries[i].compressedSize, uncompedBaseData + (sectPos + 0x1D), sizeof(int32_t)); // 0x1D
			}
			else if (header.byteswapFlags == 1) {
				memcpy(&strOffset, uncompedBaseData + (sectPos), sizeof(int32_t)); // 0x0
				memcpy(&unk2, uncompedBaseData + (sectPos + 5), sizeof(int32_t)); // 0x5
				memcpy(&uncompressedSize, uncompedBaseData + (sectPos + 9), sizeof(int32_t)); // 0x9
				memcpy(&compressedSize, uncompedBaseData + (sectPos + 0x1D), sizeof(int32_t)); // 0x1D

				// Flip all applicable values from big endian to little endian.
				sectionTable.entries[i].strOffset = flipEndian(strOffset);
				sectionTable.entries[i].poolSection = flipEndian(unk2);
				sectionTable.entries[i].size = flipEndian(uncompressedSize);
				sectionTable.entries[i].compressedSize = flipEndian(compressedSize);
			}

			sectPos += 0x21;

			printf("\tSECT INFO %u [%u %u UNCOMPED SIZE - %u, COMPEDSIZE - %u]\n", i, sectionTable.entries[i].strOffset, sectionTable.entries[i].poolSection, sectionTable.entries[i].size, sectionTable.entries[i].compressedSize);
		}

		// Section Labels
		int32_t baseOffsetForSectLabels = header.headerSize + (0x21 * header.numSectionTypes);
		sectionTable.sectionLabelOffset = baseOffsetForSectLabels;
		for (int32_t i = 0; i < header.numSectionTypes; i++) {
			strcpy(sectionTable.sectionLabels[i].label, uncompedBaseData + (baseOffsetForSectLabels + sectionTable.entries[i].strOffset));

			//printf("SECT LABEL %u [%s]\n", i, sectionTable.sectionLabels[i].label);
		}

		int32_t baseOffsetForFileLabelOffsets = header.headerSize + (0x21 * header.numSectionTypes) + header.sectionTypeNamesBufferLen;
		sectionTable.sectionFileLabelOffset = baseOffsetForFileLabelOffsets;


		// Symbol time
		int32_t totalTableSize = 0;
		if (header.byteswapFlags == 0) {
			memcpy(&sectionTable.fileLabelTable.totalLabelTableSize, uncompedBaseData + baseOffsetForFileLabelOffsets, sizeof(int32_t)); // 0x00
		}
		else if (header.byteswapFlags == 1) {
			memcpy(&totalTableSize, uncompedBaseData + baseOffsetForFileLabelOffsets, sizeof(int32_t)); // 0x00
			sectionTable.fileLabelTable.totalLabelTableSize = flipEndian(totalTableSize);
		}

		for (int32_t i = 0; i < header.numAssets; i++) {
			int32_t offset = 0;

			if (header.byteswapFlags == 0) {
				memcpy(&sectionTable.fileLabelOffsets[i], uncompedBaseData + (baseOffsetForFileLabelOffsets + (i * 4)) + 4, sizeof(int32_t));
			}
			else if (header.byteswapFlags == 1) {
				memcpy(&offset, uncompedBaseData + (baseOffsetForFileLabelOffsets + (i * 4)) + 4, sizeof(int32_t));
				sectionTable.fileLabelOffsets[i] = flipEndian(offset);
			}

			//printf("Symbol %d offset %d\n", i, sectionTable.fileLabelOffsets[i]);
		}

		int32_t baseOffsetForFileLabels = baseOffsetForFileLabelOffsets + 4 + (header.numAssets * 4);


		for (int32_t i = 0; i < header.numAssets; i++) {
			size_t strLen = strlen(uncompedBaseData + (baseOffsetForFileLabels + sectionTable.fileLabelOffsets[i])) + 1;
			sectionTable.fileLabelTable.fileLabels[i] = (char*)malloc(128);
			memset(sectionTable.fileLabelTable.fileLabels[i], 0, 128);

			strcpy(sectionTable.fileLabelTable.fileLabels[i], uncompedBaseData + (baseOffsetForFileLabels + sectionTable.fileLabelOffsets[i]));
			//printf("Symbol %d - %s\n", i, sectionTable.fileLabelTable.fileLabels[i].label);
		}

		int32_t adbLen = 0;
		if (header.byteswapFlags == 0) {
			memcpy(&sectionTable.adbStringLen, uncompedBaseData + baseOffsetForFileLabels + sectionTable.fileLabelTable.totalLabelTableSize, sizeof(int32_t)); // 0x0
		}
		else if (header.byteswapFlags == 1) {
			memcpy(&adbLen, uncompedBaseData + baseOffsetForFileLabels + sectionTable.fileLabelTable.totalLabelTableSize, sizeof(int32_t)); // 0x0
			sectionTable.adbStringLen = flipEndian(adbLen);
		}

		if (sectionTable.adbStringLen != 0) {
			sectionTable.adbString = new char[sectionTable.adbStringLen];
			memset(sectionTable.adbString, 0, sectionTable.adbStringLen);

			strncpy(sectionTable.adbString, uncompedBaseData + baseOffsetForFileLabels + sectionTable.fileLabelTable.totalLabelTableSize + 4, sectionTable.adbStringLen);
			printf("\tADB %d %s\n", sectionTable.adbStringLen, sectionTable.adbString);
		}

		int32_t baseOffsetForFileInfos = baseOffsetForFileLabels + sectionTable.fileLabelTable.totalLabelTableSize + 4 + sectionTable.adbStringLen;
		sectionTable.fileInfosOffset = baseOffsetForFileInfos;

		//printf("File Info Offset - %d\n", sectionTable.fileInfosOffset);
		for (int32_t i = 0; i < header.numSections; i++) {
			int32_t infoID = 0;
			int32_t infoDataOffset = 0;
			int32_t infoDataSize = 0;

			if (header.byteswapFlags == 0) {
				memcpy(&sectionTable.fileInfos[i].asset, uncompedBaseData + baseOffsetForFileInfos, sizeof(int32_t)); // 0x0
				memcpy(&sectionTable.fileInfos[i].start, uncompedBaseData + baseOffsetForFileInfos + 4, sizeof(int32_t)); // 0x4
				memcpy(&sectionTable.fileInfos[i].size, uncompedBaseData + baseOffsetForFileInfos + 8, sizeof(int32_t)); // 0x8
			}
			else if (header.byteswapFlags == 1) {
				memcpy(&infoID, uncompedBaseData + baseOffsetForFileInfos, sizeof(int32_t)); // 0x0
				memcpy(&infoDataOffset, uncompedBaseData + baseOffsetForFileInfos + 4, sizeof(int32_t)); // 0x4
				memcpy(&infoDataSize, uncompedBaseData + baseOffsetForFileInfos + 8, sizeof(int32_t)); // 0x8
				sectionTable.fileInfos[i].asset = flipEndian(infoID);
				sectionTable.fileInfos[i].start = flipEndian(infoDataOffset);
				sectionTable.fileInfos[i].size = flipEndian(infoDataSize);
			}

			memcpy(&sectionTable.fileInfos[i].sectionType, uncompedBaseData + baseOffsetForFileInfos + 0xC, sizeof(char)); // 0xC
			memcpy(&sectionTable.fileInfos[i].startAlignment, uncompedBaseData + baseOffsetForFileInfos + 0xD, sizeof(char)); // 0xD

			baseOffsetForFileInfos += 0xE;

			printf("\tFILE INFO %u [%u %u %u %u]\n", sectionTable.fileInfos[i].asset, sectionTable.fileInfos[i].start, sectionTable.fileInfos[i].size, sectionTable.fileInfos[i].sectionType, sectionTable.fileInfos[i].startAlignment);
		}

		printf("}\n");

		free(uncompedBaseData);
		uncompedBaseData = nullptr;
	}
	catch (...) {
		printf("An error occured while reading the provided bundle file. Error Code %d\n", errno);
		return false;
	}

	return true;
}

bool BundleV36::readBundleFileV0036(char* data) {

	int32_t end = SRC_ENDIANLITTLE;

	if (data == nullptr) {
		printf("Passed data array is null.\n");
		return false;
	}

	bundleData = data;

	try
	{
		if (!readBundleHeaderV0036(data)) {
			printf("A problem occurred while parsing the bundle header.\n");
			
			return false;
		}

		if (!readBundleSectionFileV0036(data)) {
			printf("A problem occurred while parsing the bundle section and file tables.\n");
			return false;
		}
		printf("The bundle file has finished reading.\n");
	}
	catch (int32_t e) {
		printf("An error occured while reading the provided bundle file.\n");
	}

	//free(data);

	return true;
}

BundleV36::~BundleV36() {
	if (bundleData != nullptr) {
		free(bundleData);
		bundleData = nullptr;
	}
}

bool BundleV36::readStandaloneBundleFile(char* fileName) {

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

	readBundleFileV0036(data);
	return true;
}

void BundleV36::writeStandaloneBundleFile(char* fileName) {

	FILE* currentFile = fopen(fileName, "rb");

	if (ferror(currentFile) != 0) {
		printf("Error occured while trying to open the file.\n");
		return;
	}

	fseek(currentFile, 0L, SEEK_END);
	int32_t length = ftell(currentFile);
	fseek(currentFile, 0L, SEEK_SET);

	char* data = (char*)malloc(length);

	fread(data, sizeof(char), length, currentFile);

	fclose(currentFile);

	readBundleFileV0036(data);

	free(data);
	return;
}

char* BundleV36::getFileData(char* fileName, int32_t fileInfoIdx) {
	if (bundleData == nullptr) {
		printf("Error occured while trying to open the file.\n");
		return NULL;
	}

	int32_t sectionOffset = getOffsetOfSection(sectionTable.fileInfos[fileInfoIdx].sectionType - 1);

	int32_t totalOffsetToDataSect = header.headerSize + header.sectTable.size + header.fileTable.size;

	//Initialize our new section.
	char* sect = (char*)malloc(sectionTable.fileInfos[fileInfoIdx].size);
	if (sect == nullptr) {
		throw("Memory Allocation Error for the section.");
		return NULL;
	}

	memset(sect, 0, sectionTable.fileInfos[fileInfoIdx].size);

	printf("Bundle::getFileData() - Allocating file info %d in section %s of size %d.\n", fileInfoIdx, sectionTable.sectionLabels[sectionTable.fileInfos[fileInfoIdx].sectionType - 1].label, sectionTable.fileInfos[fileInfoIdx].size);

	// Just do this in a completely different way
	if (header.compression) {
		totalOffsetToDataSect = header.headerSize + header.sectTable.compressedSize + header.fileTable.compressedSize;

		size_t sectCompedSize = getCompressedSizeofSection(sectionTable.fileInfos[fileInfoIdx].sectionType - 1);
		size_t sectUncompedSize = getUncompressedSizeofSection(sectionTable.fileInfos[fileInfoIdx].sectionType - 1);
		char* data = (char*)malloc(sectCompedSize);
		if (data == nullptr) {
			throw("Memory Allocation Error for the compression code with the compressed memory.");
			return NULL;
		}

		memcpy_s(data, sectCompedSize, bundleData + totalOffsetToDataSect + sectionOffset, sectCompedSize);

		char* outputData = InflateData(data, 6, sectCompedSize, sectUncompedSize);

		memcpy_s(sect, sectionTable.fileInfos[fileInfoIdx].size, outputData + sectionTable.fileInfos[fileInfoIdx].start, sectionTable.fileInfos[fileInfoIdx].size);

		return sect;
	}

	memcpy_s(sect, sectionTable.fileInfos[fileInfoIdx].size, (bundleData + totalOffsetToDataSect + sectionOffset + sectionTable.fileInfos[fileInfoIdx].start), sectionTable.fileInfos[fileInfoIdx].size);

	printf("Bundle::getFileData() - File Data for entry %d successfully obtained.\n", fileInfoIdx);

	return sect;
}

bool BundleV31::readBundleFileV0031(char* data) {
	if (data == nullptr) {
		printf("Passed data array is null.\n");
		return false;
	}

	isReady = false;

	bundleData = data;

	char caffVer[0x10];

	int32_t offset = 0;
	bool isValidVersion = false;

	if (memcpy(caffVer, data + 4, 0x10) == nullptr) {
		printf("Error occured while trying to read the file.\n");
		return false;
	}

	//printf("Bundle Version: %s\n", caffVer);
	strcpy(header.versionString, caffVer);

	if (strcmp(header.versionString, "28.01.05.0031") == 0) {
		printf("The bundle file provided is indeed the January 28th, 2005 version of the bundle format.\n");
		isValidVersion = true;
	}

	if (!isValidVersion) {
		printf("The given file is not a valid or is a bundle file with a different version.\n");
		return false;
	}

	try {
		printf("CAFF Header - {\n");
		// Now we read the important information.

		memcpy(&header.headerHash, data + 0x14 + offset, sizeof(int32_t));
		memcpy(&header.numAssets, data + 0x18 + offset, sizeof(int32_t));
		memcpy(&header.numSections, data + 0x1C + offset, sizeof(int32_t));
		memcpy(&header.numByteswapGroups, data + 0x20 + offset, sizeof(int32_t));
		memcpy(&header.numByteswaps, data + 0x24 + offset, sizeof(int32_t));
		memcpy(&header.numRelocationGroups, data + 0x28 + offset, sizeof(int32_t));
		memcpy(&header.numRelocations, data + 0x2C + offset, sizeof(int32_t));
		memcpy(&header.numPoolItems, data + 0x30 + offset, sizeof(int32_t));
		memcpy(&header.tableSectionOffset, data + 0x34 + offset, sizeof(int32_t));

		memcpy(&header.headerSize, data + 0x38 + offset, sizeof(int32_t));

		memcpy(&header.byteswapFlags, data + 0x3C + offset, sizeof(int8_t));
		memcpy(&header.numSectionTypes, data + 0x3D + offset, sizeof(int8_t));
		memcpy(&header.compression, data + 0x3E + offset, sizeof(int8_t));
		memcpy(&header.numPools, data + 0x3F + offset, sizeof(int8_t));

		if (header.byteswapFlags == 1) {
			header.headerHash = flipEndian(header.headerHash); // Flip from big endian to little endian.
			header.headerSize = flipEndian(header.headerSize); // Flip from big endian to little endian.

			header.numAssets = flipEndian(header.numAssets); // Flip from big endian to little endian.
			header.numSections = flipEndian(header.numSections); // Flip from big endian to little endian.
			header.numByteswapGroups = flipEndian(header.numByteswapGroups); // Flip from big endian to little endian.
			header.numByteswaps = flipEndian(header.numByteswaps); // Flip from big endian to little endian.
			header.numRelocationGroups = flipEndian(header.numRelocationGroups); // Flip from big endian to little endian.
			header.numRelocations = flipEndian(header.numRelocations); // Flip from big endian to little endian.
			header.numPoolItems = flipEndian(header.numPoolItems); // Flip from big endian to little endian.
			header.tableSectionOffset = flipEndian(header.tableSectionOffset); // Flip from big endian to little endian.
		}

		printf("\tCRC - %08X\n", header.headerHash);
		printf("}\n");

		int32_t position = 0x40;
		for (int32_t i = 0; i < header.numSectionTypes; i++) {
			memcpy_s(sectionEntries[i].sectionType, 11, data + position, 11);
			memcpy(&sectionEntries[i].alignment, data + position + 0xB, sizeof(int8_t));
			memcpy(&sectionEntries[i].poolSection, data + position + 0xC, sizeof(int32_t));
			memcpy(&sectionEntries[i].size, data + position + 0x10, sizeof(int32_t));
			memcpy(&sectionEntries[i].base, data + position + 0x14, sizeof(int32_t));
			memcpy(&sectionEntries[i].offset, data + position + 0x18, sizeof(int32_t));
			memcpy(&sectionEntries[i].overlap, data + position + 0x1C, sizeof(int32_t));
			memcpy(&sectionEntries[i].loadSize, data + position + 0x20, sizeof(int32_t));
			memcpy(&sectionEntries[i].compressedSize, data + position + 0x24, sizeof(int32_t));

			if (header.byteswapFlags == 1) {
				sectionEntries[i].poolSection = flipEndian(sectionEntries[i].poolSection); // Flip from big endian to little endian.
				sectionEntries[i].size = flipEndian(sectionEntries[i].size); // Flip from big endian to little endian.
				sectionEntries[i].base = flipEndian(sectionEntries[i].base); // Flip from big endian to little endian.
				sectionEntries[i].offset = flipEndian(sectionEntries[i].offset); // Flip from big endian to little endian.
				sectionEntries[i].overlap = flipEndian(sectionEntries[i].overlap); // Flip from big endian to little endian.
				sectionEntries[i].loadSize = flipEndian(sectionEntries[i].loadSize); // Flip from big endian to little endian.
				sectionEntries[i].compressedSize = flipEndian(sectionEntries[i].compressedSize); // Flip from big endian to little endian.
			}

			position += 0x28;
		}

		// Run through the file info table next.
		char* dataSect = getSectionData(0);

		memcpy(&fileInfoTable.debugTable.sizeOfStringTable, dataSect + header.tableSectionOffset, sizeof(int32_t));
		if (header.byteswapFlags == 1) {
			fileInfoTable.debugTable.sizeOfStringTable = flipEndian(fileInfoTable.debugTable.sizeOfStringTable); // Flip from big endian to little endian.
		}

		int32_t offsToTable = header.tableSectionOffset + 4;
		if (fileInfoTable.debugTable.sizeOfStringTable != 0) {
			offsToTable += (0x4 * header.numAssets) + fileInfoTable.debugTable.sizeOfStringTable;

			fileInfoTable.debugTable.stringTableOffsets = new int32_t[header.numAssets]();
			fileInfoTable.debugTable.fileNames = new char*[header.numAssets]();

			int32_t strTablePos = header.tableSectionOffset + 4 + (4 * header.numAssets);

			for (int32_t i = 0; i < header.numAssets; i++) {
				memcpy(&fileInfoTable.debugTable.stringTableOffsets[i], dataSect + header.tableSectionOffset + 4 + (4 * i), sizeof(int32_t));

				if (header.byteswapFlags == 1) {
					fileInfoTable.debugTable.stringTableOffsets[i] = flipEndian(fileInfoTable.debugTable.stringTableOffsets[i]); // Flip from big endian to little endian.
				}
			}

			for (int32_t i = 0; i < header.numAssets; i++) {
				int32_t length = strlen(dataSect + strTablePos + fileInfoTable.debugTable.stringTableOffsets[i]);
				fileInfoTable.debugTable.fileNames[i] = new char[length + 1]();
				strcpy_s(fileInfoTable.debugTable.fileNames[i], length + 1, dataSect + strTablePos + fileInfoTable.debugTable.stringTableOffsets[i]);

				printf("FILE %d - %s\n", i, fileInfoTable.debugTable.fileNames[i]);
			}
		}

		fileInfoTable.fileInfoEntries = new FileInfoEntry[header.numSections]();

		for (int32_t i = 0; i < header.numSections; i++) {
			memcpy(&fileInfoTable.fileInfoEntries[i].asset, dataSect + offsToTable + (0xE * i), sizeof(int32_t));
			memcpy(&fileInfoTable.fileInfoEntries[i].start, dataSect + offsToTable + (0xE * i) + 4, sizeof(int32_t));
			memcpy(&fileInfoTable.fileInfoEntries[i].size, dataSect + offsToTable + (0xE * i) + 8, sizeof(int32_t));
			memcpy(&fileInfoTable.fileInfoEntries[i].sectionType, dataSect + offsToTable + (0xE * i) + 0xC, sizeof(char));
			memcpy(&fileInfoTable.fileInfoEntries[i].startAlignment, dataSect + offsToTable + (0xE * i) + 0xD, sizeof(char));

			if (header.byteswapFlags == 1) {
				fileInfoTable.fileInfoEntries[i].asset = flipEndian(fileInfoTable.fileInfoEntries[i].asset); // Flip from big endian to little endian.
				fileInfoTable.fileInfoEntries[i].start = flipEndian(fileInfoTable.fileInfoEntries[i].start); // Flip from big endian to little endian.
				fileInfoTable.fileInfoEntries[i].size = flipEndian(fileInfoTable.fileInfoEntries[i].size); // Flip from big endian to little endian.
			}

			printf("FILE INFO %u [%u %u %u %u]\n", fileInfoTable.fileInfoEntries[i].asset, fileInfoTable.fileInfoEntries[i].start, fileInfoTable.fileInfoEntries[i].size, fileInfoTable.fileInfoEntries[i].sectionType, fileInfoTable.fileInfoEntries[i].startAlignment);
		}

		// Legacy check.
		char* base = new char[0x20];

		strcpy_s(base, 0x20, dataSect);

		type = V31_Unknown;

		if (strcmp(base, "darkpackage") == 0) {
			type = V31_PDZPackage;
		}

		if (strcmp(base, "kameoDatabase") == 0) {
			type = V31_KameoDB;
		}

		if (strcmp(base, "texture") == 0) {
			type = V31_Texture;
		}

		if (strcmp(base, "text") == 0) {
			strcpy_s(base, 0x20, dataSect + 5);

			if (strcmp(base, "04.05.05.0032") == 0) {
				type = V31_Text;
			}

			if (strcmp(base, "04.05.05.0032") != 0) {
				type = V31_ConkerText;
			}
		}

		delete[] base;
		delete[] dataSect;

		isReady = true;
		printf("V31 Bundle Finished Reading.\n");
	}
	catch (int32_t ex) {
	}

	return true;
}

BundleV31::~BundleV31() {
	if (bundleData != nullptr) {
		free(bundleData);
		bundleData = nullptr;
	}

	if (fileInfoTable.fileInfoEntries != nullptr) {
		delete[] fileInfoTable.fileInfoEntries;
		fileInfoTable.fileInfoEntries = nullptr;
	}

	if (fileInfoTable.debugTable.fileNames != nullptr) {
		delete[] fileInfoTable.debugTable.fileNames;
		fileInfoTable.debugTable.fileNames = nullptr;
	}

	if (fileInfoTable.debugTable.stringTableOffsets != nullptr) {
		delete[] fileInfoTable.debugTable.stringTableOffsets;
		fileInfoTable.debugTable.stringTableOffsets = nullptr;
	}
}

char* BundleV31::getSectionData(int32_t section) {
	if (bundleData == nullptr) {
		printf("We somehow don't have bundle data for this. This is bad.\n");
		return nullptr;
	}

	if (section < 0 || section >= header.numSectionTypes) {
		printf("Invalid range for the section index.\n");
		return nullptr;
	}

	char* sectData = new char[sectionEntries[section].size];

	if (header.compression) {
		char* data = (char*)malloc(sectionEntries[section].compressedSize);

		size_t sectCompedSize = sectionEntries[section].compressedSize;
		size_t sectUncompedSize = sectionEntries[section].size;

		if (data == nullptr) {
			throw("Memory Allocation Error for the compression code with the compressed memory.");
			return NULL;
		}

		printf("Offset - %d.\n", getOffsetOfSection(section));

		memcpy_s(data, sectCompedSize, bundleData + header.headerSize + getOffsetOfSection(section), sectCompedSize);

		char* outData = InflateData(data, 6, sectCompedSize, sectUncompedSize);

		memcpy_s(sectData, sectUncompedSize, outData, sectUncompedSize);

		free(outData);
		free(data);

		return sectData;
	}

	memcpy_s(sectData, sectionEntries[section].size, bundleData + header.headerSize + getOffsetOfSection(section), sectionEntries[section].size);

	return sectData;
}

char* BundleV31::getFileData(char* fileName, int32_t fileInfoIdx) {
	if (bundleData == nullptr) {
		printf("Error occured while trying to open the file.\n");
		return NULL;
	}

	int32_t sectionOffset = getOffsetOfSection(fileInfoTable.fileInfoEntries[fileInfoIdx].sectionType - 1);

	int32_t totalOffsetToDataSect = header.headerSize;

	//Initialize our new section.
	char* sect = (char*)malloc(fileInfoTable.fileInfoEntries[fileInfoIdx].size);
	if (sect == nullptr) {
		throw("Memory Allocation Error for the section.");
		return NULL;
	}

	memset(sect, 0, fileInfoTable.fileInfoEntries[fileInfoIdx].size);

	printf("Bundle::getFileData() - Allocating file info %d in section %s of size %d.\n", fileInfoIdx, sectionEntries[fileInfoTable.fileInfoEntries[fileInfoIdx].sectionType - 1].sectionType, fileInfoTable.fileInfoEntries[fileInfoIdx].size);

	// Just do this in a completely different way
	if (header.compression) {
		totalOffsetToDataSect = header.headerSize;

		size_t sectCompedSize = sectionEntries[fileInfoTable.fileInfoEntries[fileInfoIdx].sectionType - 1].compressedSize;
		size_t sectUncompedSize = sectionEntries[fileInfoTable.fileInfoEntries[fileInfoIdx].sectionType - 1].size;
		char* data = (char*)malloc(sectCompedSize);
		if (data == nullptr) {
			throw("Memory Allocation Error for the compression code with the compressed memory.");
			return NULL;
		}

		memcpy_s(data, sectCompedSize, bundleData + totalOffsetToDataSect + sectionOffset, sectCompedSize);

		char* outputData = InflateData(data, 6, sectCompedSize, sectUncompedSize);

		memcpy_s(sect, fileInfoTable.fileInfoEntries[fileInfoIdx].size, outputData + fileInfoTable.fileInfoEntries[fileInfoIdx].start, fileInfoTable.fileInfoEntries[fileInfoIdx].size);

		return sect;
	}

	memcpy_s(sect, fileInfoTable.fileInfoEntries[fileInfoIdx].size, (bundleData + totalOffsetToDataSect + sectionOffset + fileInfoTable.fileInfoEntries[fileInfoIdx].start), fileInfoTable.fileInfoEntries[fileInfoIdx].size);

	printf("Bundle::getFileData() - File Data for entry %d successfully obtained.\n", fileInfoIdx);

	return sect;
}

bool BundleV26::readBundleFileV0026(char* data) {
	if (data == nullptr) {
		printf("Passed data array is null.\n");
		return false;
	}

	isReady = false;

	bundleData = data;

	char caffVer[0x10];

	int32_t offset = 0;
	bool isValidVersion = false;

	if (memcpy(caffVer, data + 4, 0x10) == nullptr) {
		printf("Error occured while trying to read the file.\n");
		return false;
	}

	//printf("Bundle Version: %s\n", caffVer);
	strcpy(header.versionString, caffVer);

	if (strcmp(header.versionString, "24.09.03.0026") == 0) {
		printf("The bundle file provided is indeed the September 24, 2003 version of the bundle format.\n");
		isValidVersion = true;
	}

	if (!isValidVersion) {
		printf("The given file is not a valid or is a bundle file with a different version.\n");
		return false;
	}

	try {
		// Now we read the important information.

		memcpy(&header.numOfFiles, data + 0x14 + offset, sizeof(int32_t));
		memcpy(&header.numOfFileEntries, data + 0x18 + offset, sizeof(int32_t));
		memcpy(&header.unk3, data + 0x1C + offset, sizeof(int32_t));
		memcpy(&header.unk4, data + 0x20 + offset, sizeof(int32_t));
		memcpy(&header.unk5, data + 0x24 + offset, sizeof(int32_t));
		memcpy(&header.unk6, data + 0x28 + offset, sizeof(int32_t));
		memcpy(&header.fileInfoTableOffset, data + 0x2C + offset, sizeof(int32_t));

		memcpy(&header.headerSize, data + 0x30 + offset, sizeof(int32_t));

		memcpy(&header.unk8, data + 0x34 + offset, sizeof(char));
		memcpy(&header.numSectionTypes, data + 0x35 + offset, sizeof(char));
		memcpy(&header.compression, data + 0x36 + offset, sizeof(char));
		memcpy(&header.unk9, data + 0x37 + offset, sizeof(char));

		int32_t position = 0x38;
		for (int32_t i = 0; i < header.numSectionTypes; i++) {
			memcpy_s(sectionEntries[i].sectionName, 8, data + position, 8);
			memcpy(&sectionEntries[i].size, data + position + 0x8, sizeof(int32_t));
			memcpy(&sectionEntries[i].unk1, data + position + 0xC, sizeof(int32_t));
			memcpy(&sectionEntries[i].compressedSize, data + position + 0x20, sizeof(int32_t));

			position += 0x24;
		}

		// Run a few checks
		char* dataSect = getSectionData(0);

		memcpy(&fileInfoTable.debugTable.sizeOfStringTable, dataSect + header.fileInfoTableOffset, sizeof(int32_t));

		int32_t offsToTable = header.fileInfoTableOffset + 4;
		if (fileInfoTable.debugTable.sizeOfStringTable != 0) {
			offsToTable += (0x4 * header.numOfFiles) + fileInfoTable.debugTable.sizeOfStringTable;

			fileInfoTable.debugTable.stringTableOffsets = new int32_t[header.numOfFiles]();
			fileInfoTable.debugTable.fileNames = new char* [header.numOfFiles]();

			int32_t strTablePos = header.fileInfoTableOffset + 4 + (4 * header.numOfFiles);

			for (int32_t i = 0; i < header.numOfFiles; i++) {
				memcpy(&fileInfoTable.debugTable.stringTableOffsets[i], dataSect + header.fileInfoTableOffset + 4 + (4 * i), sizeof(int32_t));
			}

			for (int32_t i = 0; i < header.numOfFiles; i++) {
				int32_t length = strlen(dataSect + strTablePos + fileInfoTable.debugTable.stringTableOffsets[i]);
				fileInfoTable.debugTable.fileNames[i] = new char[length + 1]();
				strcpy_s(fileInfoTable.debugTable.fileNames[i], length + 1, dataSect + strTablePos + fileInfoTable.debugTable.stringTableOffsets[i]);

				printf("FILE %d - %s\n", i, fileInfoTable.debugTable.fileNames[i]);
			}
		}

		fileInfoTable.fileInfoEntries = new FileInfoEntry[header.numOfFileEntries]();

		for (int32_t i = 0; i < header.numOfFileEntries; i++) {
			memcpy(&fileInfoTable.fileInfoEntries[i].asset, dataSect + offsToTable + (0xE * i), sizeof(int32_t));
			memcpy(&fileInfoTable.fileInfoEntries[i].start, dataSect + offsToTable + (0xE * i) + 4, sizeof(int32_t));
			memcpy(&fileInfoTable.fileInfoEntries[i].size, dataSect + offsToTable + (0xE * i) + 8, sizeof(int32_t));
			memcpy(&fileInfoTable.fileInfoEntries[i].sectionType, dataSect + offsToTable + (0xE * i) + 0xC, sizeof(char));
			memcpy(&fileInfoTable.fileInfoEntries[i].startAlignment, dataSect + offsToTable + (0xE * i) + 0xD, sizeof(char));

			printf("FILE INFO %u [%u %u %u %u]\n", fileInfoTable.fileInfoEntries[i].asset, fileInfoTable.fileInfoEntries[i].start, fileInfoTable.fileInfoEntries[i].size, fileInfoTable.fileInfoEntries[i].sectionType, fileInfoTable.fileInfoEntries[i].startAlignment);
		}

		char* base = new char[0x20];

		strcpy_s(base, 0x20, dataSect);

		type = V31_Unknown;

		if (strcmp(base, "darkpackage") == 0) {
			type = V31_PDZPackage;
		}

		if (strcmp(base, "kameoDatabase") == 0) {
			type = V31_KameoDB;
		}

		if (strcmp(base, "texture") == 0) {
			type = V31_Texture;
		}

		if (strcmp(base, "text") == 0) {
			strcpy_s(base, 0x20, dataSect + 5);

			if (strcmp(base, "04.05.05.0032") == 0) {
				type = V31_Text;
			}

			if (strcmp(base, "04.05.05.0032") != 0) {
				type = V31_ConkerText;
			}
		}

		printf("%s\n", base);

		delete[] base;
		delete[] dataSect;

		isReady = true;
		printf("V26 Bundle Finished Reading.\n");
	}
	catch (int32_t ex) {
	}

	return true;
}

BundleV26::~BundleV26() {
	if (bundleData != nullptr) {
		free(bundleData);
		bundleData = nullptr;
	}
}

char* BundleV26::getSectionData(int32_t section) {
	if (bundleData == nullptr) {
		printf("We somehow don't have bundle data for this. This is bad.\n");
		return nullptr;
	}

	if (section < 0 || section >= header.numSectionTypes) {
		printf("Invalid range for the section index.\n");
		return nullptr;
	}

	char* sectData = new char[sectionEntries[section].size];

	if (header.compression) {
		char* data = (char*)malloc(sectionEntries[section].compressedSize);

		size_t sectCompedSize = sectionEntries[section].compressedSize;
		size_t sectUncompedSize = sectionEntries[section].size;

		if (data == nullptr) {
			throw("Memory Allocation Error for the compression code with the compressed memory.");
			return NULL;
		}

		printf("Offset - %d.\n", getOffsetOfSection(section));

		memcpy_s(data, sectCompedSize, bundleData + header.headerSize + getOffsetOfSection(section), sectCompedSize);

		char* outData = InflateData(data, 6, sectCompedSize, sectUncompedSize);

		memcpy_s(sectData, sectUncompedSize, outData, sectUncompedSize);

		free(outData);
		free(data);

		return sectData;
	}

	memcpy_s(sectData, sectionEntries[section].size, bundleData + header.headerSize + getOffsetOfSection(section), sectionEntries[section].size);

	return sectData;
}

char* BundleV26::getFileData(char* fileName, int32_t fileInfoIdx) {
	if (bundleData == nullptr) {
		printf("Error occured while trying to open the file.\n");
		return NULL;
	}

	int32_t sectionOffset = getOffsetOfSection(fileInfoTable.fileInfoEntries[fileInfoIdx].sectionType - 1);

	int32_t totalOffsetToDataSect = header.headerSize;

	//Initialize our new section.
	char* sect = (char*)malloc(fileInfoTable.fileInfoEntries[fileInfoIdx].size);
	if (sect == nullptr) {
		throw("Memory Allocation Error for the section.");
		return NULL;
	}

	memset(sect, 0, fileInfoTable.fileInfoEntries[fileInfoIdx].size);

	printf("Bundle::getFileData() - Allocating file info %d in section %s of size %d.\n", fileInfoIdx, sectionEntries[fileInfoTable.fileInfoEntries[fileInfoIdx].sectionType - 1].sectionName, fileInfoTable.fileInfoEntries[fileInfoIdx].size);

	// Just do this in a completely different way
	if (header.compression) {
		totalOffsetToDataSect = header.headerSize;

		size_t sectCompedSize = sectionEntries[fileInfoTable.fileInfoEntries[fileInfoIdx].sectionType - 1].compressedSize;
		size_t sectUncompedSize = sectionEntries[fileInfoTable.fileInfoEntries[fileInfoIdx].sectionType - 1].size;
		char* data = (char*)malloc(sectCompedSize);
		if (data == nullptr) {
			throw("Memory Allocation Error for the compression code with the compressed memory.");
			return NULL;
		}

		memcpy_s(data, sectCompedSize, bundleData + totalOffsetToDataSect + sectionOffset, sectCompedSize);

		char* outputData = InflateData(data, 6, sectCompedSize, sectUncompedSize);

		memcpy_s(sect, fileInfoTable.fileInfoEntries[fileInfoIdx].size, outputData + fileInfoTable.fileInfoEntries[fileInfoIdx].start, fileInfoTable.fileInfoEntries[fileInfoIdx].size);

		return sect;
	}

	memcpy_s(sect, fileInfoTable.fileInfoEntries[fileInfoIdx].size, (bundleData + totalOffsetToDataSect + sectionOffset + fileInfoTable.fileInfoEntries[fileInfoIdx].start), fileInfoTable.fileInfoEntries[fileInfoIdx].size);

	printf("Bundle::getFileData() - File Data for entry %d successfully obtained.\n", fileInfoIdx);

	return sect;
}