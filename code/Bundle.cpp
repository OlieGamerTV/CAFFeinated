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

	printf("The file supplied is not a supported bundle file.\n");
	return false;
}

void BundleFile::ClearActiveBundleFile() {
	if (V36Bundle != nullptr) {
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
}

bool BundleV36::readBundleHeaderV0036(char* data) {
	if (data == nullptr) {
		printf("Passed data array is null.\n");
		return false;
	}

	char caffVer[0x10];

	int offset = 0;
	bool isValidVersion = false;

	if (memcpy(caffVer, data + 4, 0x10) == nullptr) {
		printf("Error occured while trying to read the file.\n");
		return false;
	}

	//printf("Bundle Version: %s\n", caffVer);
	strcpy(header.versionString, caffVer);

	if (strcmp(header.versionString, "08.11.07.0040") == 0) {
		printf("The bundle file provided is a Kinect Sports bundle.\n");
		offset = 8;
		isValidVersion = true;
	}

	if (strcmp(header.versionString, "28.01.05.0031") == 0) {
		printf("The bundle file provided is either a Conker: Live & Reloaded, Kameo: Elements of Power or Perfect Dark Zero bundle, which we do not support.\n");
		return false;
	}

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

		memcpy(&header.headerSize, data + 0x14 + offset, sizeof(int)); // 0x14
		memcpy(&header.headerHash, data + 0x18 + offset, sizeof(int)); // 0x18
		memcpy(&header.numAssets, data + 0x1C + offset, sizeof(int)); // 0x1C
		memcpy(&header.numSections, data + 0x20 + offset, sizeof(int)); // 0x20

		memcpy(&header.byteswapFlags, data + 0x48 + offset, sizeof(char));
		memcpy(&header.numSectionTypes, data + 0x49 + offset, sizeof(char));
		memcpy(&header.compression, data + 0x4A + offset, sizeof(char));
		memcpy(&header.numPools, data + 0x4B + offset, sizeof(char));

		memcpy(&header.sectionTypeNamesBufferLen, data + 0x4C + offset, sizeof(int)); // 0x4C

		memcpy(&header.sectionTableUncompedSize, data + 0x50 + offset, sizeof(int)); // 0x50
		memcpy(&header.sectionTableCompedSize, data + 0x60 + offset, sizeof(int)); // 0x60
		memcpy(&header.fileTableUncompedSize, data + 0x64 + offset, sizeof(int)); // 0x64
		memcpy(&header.fileTableCompedSize, data + 0x74 + offset, sizeof(int)); // 0x74

		printf("\tCRC - %08X, NUM SYMBOLS & NUM FILES[%d %d -> %d %d]\n", flipEndian(header.headerHash), header.numAssets, header.numSections, flipEndian(header.numAssets), flipEndian(header.numSections));

		if (header.byteswapFlags == 1) {
			header.sectionTypeNamesBufferLen = flipEndian(header.sectionTypeNamesBufferLen);

			header.headerSize = flipEndian(header.headerSize); // Flip from big endian to little endian.
			header.headerHash = flipEndian(header.headerHash); // Flip from big endian to little endian.
			header.numAssets = flipEndian(header.numAssets); // Flip from big endian to little endian.
			header.numSections = flipEndian(header.numSections); // Flip from big endian to little endian.

			// Flip all applicable values from big endian to little endian.
			header.sectionTableUncompedSize = flipEndian(header.sectionTableUncompedSize);
			header.sectionTableCompedSize = flipEndian(header.sectionTableCompedSize);
			header.fileTableUncompedSize = flipEndian(header.fileTableUncompedSize);
			header.fileTableCompedSize = flipEndian(header.fileTableCompedSize);
		}
		printf("}\n");
	}
	catch (int ex) {
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
		sectionTable.fileLabelOffsets = new int[header.numAssets];
		sectionTable.fileInfos = new FileInfoEntry[header.numSections];
		sectionTable.fileLabelTable.fileLabels = new FileLabel[header.numAssets];

		char* uncompedBaseData = (char*)malloc(header.headerSize + header.sectionTableUncompedSize + header.fileTableUncompedSize);
		memset(uncompedBaseData, 0, header.headerSize + header.sectionTableUncompedSize + header.fileTableUncompedSize);

		if (header.compression == 1) {
			memcpy(uncompedBaseData, data, header.headerSize);

			// Do the section table first
			char* in = (char*)malloc(header.sectionTableCompedSize);

			memcpy(in, data + header.headerSize, header.sectionTableCompedSize);

			char* out = InflateData(in, 6, header.sectionTableCompedSize, header.sectionTableUncompedSize);

			memcpy(uncompedBaseData + header.headerSize, out, header.sectionTableUncompedSize);

			free(out);

			// And now we do the file table.
			in = (char*)malloc(header.fileTableCompedSize);

			memcpy(in, data + header.headerSize + header.sectionTableCompedSize, header.fileTableCompedSize);

			out = InflateData(in, 6, header.fileTableCompedSize, header.fileTableUncompedSize);

			memcpy(uncompedBaseData + header.headerSize + header.sectionTableUncompedSize, out, header.fileTableUncompedSize);

			free(out);
		}
		else {
			memcpy(uncompedBaseData, data, header.headerSize + header.sectionTableUncompedSize + header.fileTableUncompedSize);
		}

		// Section Time
		int sectPos = header.headerSize;
		for (int i = 0; i < header.numSectionTypes; i++) {
			int strOffset = 0;
			int unk2 = 0;
			int uncompressedSize = 0;
			int compressedSize = 0;

			memcpy(&sectionTable.entries[i].unk1, uncompedBaseData + (sectPos + 4), sizeof(char)); // 0x4

			if (header.byteswapFlags == 0) {
				memcpy(&sectionTable.entries[i].strOffset, uncompedBaseData + (sectPos), sizeof(int)); // 0x0
				memcpy(&sectionTable.entries[i].unk2, uncompedBaseData + (sectPos + 5), sizeof(int)); // 0x5
				memcpy(&sectionTable.entries[i].uncompressedSize, uncompedBaseData + (sectPos + 9), sizeof(int)); // 0x9
				memcpy(&sectionTable.entries[i].compressedSize, uncompedBaseData + (sectPos + 0x1D), sizeof(int)); // 0x1D
			}
			else if (header.byteswapFlags == 1) {
				memcpy(&strOffset, uncompedBaseData + (sectPos), sizeof(int)); // 0x0
				memcpy(&unk2, uncompedBaseData + (sectPos + 5), sizeof(int)); // 0x5
				memcpy(&uncompressedSize, uncompedBaseData + (sectPos + 9), sizeof(int)); // 0x9
				memcpy(&compressedSize, uncompedBaseData + (sectPos + 0x1D), sizeof(int)); // 0x1D

				// Flip all applicable values from big endian to little endian.
				sectionTable.entries[i].strOffset = flipEndian(strOffset);
				sectionTable.entries[i].unk2 = flipEndian(unk2);
				sectionTable.entries[i].uncompressedSize = flipEndian(uncompressedSize);
				sectionTable.entries[i].compressedSize = flipEndian(compressedSize);
			}

			sectPos += 0x21;

			printf("\tSECT INFO %u [%u %u UNCOMPED SIZE - %u, COMPEDSIZE - %u]\n", i, sectionTable.entries[i].strOffset, sectionTable.entries[i].unk2, sectionTable.entries[i].uncompressedSize, sectionTable.entries[i].compressedSize);
		}

		// Section Labels
		int baseOffsetForSectLabels = header.headerSize + (0x21 * header.numSectionTypes);
		sectionTable.sectionLabelOffset = baseOffsetForSectLabels;
		for (int i = 0; i < header.numSectionTypes; i++) {
			strcpy(sectionTable.sectionLabels[i].label, uncompedBaseData + (baseOffsetForSectLabels + sectionTable.entries[i].strOffset));

			//printf("SECT LABEL %u [%s]\n", i, sectionTable.sectionLabels[i].label);
		}

		int baseOffsetForFileLabelOffsets = header.headerSize + (0x21 * header.numSectionTypes) + header.sectionTypeNamesBufferLen;
		sectionTable.sectionFileLabelOffset = baseOffsetForFileLabelOffsets;


		// Symbol time
		int totalTableSize = 0;
		if (header.byteswapFlags == 0) {
			memcpy(&sectionTable.fileLabelTable.totalLabelTableSize, uncompedBaseData + baseOffsetForFileLabelOffsets, sizeof(int)); // 0x00
		}
		else if (header.byteswapFlags == 1) {
			memcpy(&totalTableSize, uncompedBaseData + baseOffsetForFileLabelOffsets, sizeof(int)); // 0x00
			sectionTable.fileLabelTable.totalLabelTableSize = flipEndian(totalTableSize);
		}

		for (int i = 0; i < header.numAssets; i++) {
			int offset = 0;

			if (header.byteswapFlags == 0) {
				memcpy(&sectionTable.fileLabelOffsets[i], uncompedBaseData + (baseOffsetForFileLabelOffsets + (i * 4)) + 4, sizeof(int));
			}
			else if (header.byteswapFlags == 1) {
				memcpy(&offset, uncompedBaseData + (baseOffsetForFileLabelOffsets + (i * 4)) + 4, sizeof(int));
				sectionTable.fileLabelOffsets[i] = flipEndian(offset);
			}

			//printf("Symbol %d offset %d\n", i, sectionTable.fileLabelOffsets[i]);
		}

		int baseOffsetForFileLabels = baseOffsetForFileLabelOffsets + 4 + (header.numAssets * 4);


		for (int i = 0; i < header.numAssets; i++) {
			strcpy(sectionTable.fileLabelTable.fileLabels[i].label, uncompedBaseData + (baseOffsetForFileLabels + sectionTable.fileLabelOffsets[i]));
			//printf("Symbol %d - %s\n", i, sectionTable.fileLabelTable.fileLabels[i].label);
		}

		int adbLen = 0;
		if (header.byteswapFlags == 0) {
			memcpy(&sectionTable.adbStringLen, uncompedBaseData + baseOffsetForFileLabels + sectionTable.fileLabelTable.totalLabelTableSize, sizeof(int)); // 0x0
		}
		else if (header.byteswapFlags == 1) {
			memcpy(&adbLen, uncompedBaseData + baseOffsetForFileLabels + sectionTable.fileLabelTable.totalLabelTableSize, sizeof(int)); // 0x0
			sectionTable.adbStringLen = flipEndian(adbLen);
		}

		if (sectionTable.adbStringLen != 0) {
			sectionTable.adbString = new char[sectionTable.adbStringLen];
			memset(sectionTable.adbString, 0, sectionTable.adbStringLen);

			strncpy(sectionTable.adbString, uncompedBaseData + baseOffsetForFileLabels + sectionTable.fileLabelTable.totalLabelTableSize + 4, sectionTable.adbStringLen);
			printf("\tADB %d %s\n", sectionTable.adbStringLen, sectionTable.adbString);
		}

		int baseOffsetForFileInfos = baseOffsetForFileLabels + sectionTable.fileLabelTable.totalLabelTableSize + 4 + sectionTable.adbStringLen;
		sectionTable.fileInfosOffset = baseOffsetForFileInfos;

		//printf("File Info Offset - %d\n", sectionTable.fileInfosOffset);
		for (int i = 0; i < header.numSections; i++) {
			int infoID = 0;
			int infoDataOffset = 0;
			int infoDataSize = 0;

			if (header.byteswapFlags == 0) {
				memcpy(&sectionTable.fileInfos[i].ID, uncompedBaseData + baseOffsetForFileInfos, sizeof(int)); // 0x0
				memcpy(&sectionTable.fileInfos[i].dataOffset, uncompedBaseData + baseOffsetForFileInfos + 4, sizeof(int)); // 0x4
				memcpy(&sectionTable.fileInfos[i].dataSize, uncompedBaseData + baseOffsetForFileInfos + 8, sizeof(int)); // 0x8
			}
			else if (header.byteswapFlags == 1) {
				memcpy(&infoID, uncompedBaseData + baseOffsetForFileInfos, sizeof(int)); // 0x0
				memcpy(&infoDataOffset, uncompedBaseData + baseOffsetForFileInfos + 4, sizeof(int)); // 0x4
				memcpy(&infoDataSize, uncompedBaseData + baseOffsetForFileInfos + 8, sizeof(int)); // 0x8
				sectionTable.fileInfos[i].ID = flipEndian(infoID);
				sectionTable.fileInfos[i].dataOffset = flipEndian(infoDataOffset);
				sectionTable.fileInfos[i].dataSize = flipEndian(infoDataSize);
			}

			memcpy(&sectionTable.fileInfos[i].section, uncompedBaseData + baseOffsetForFileInfos + 0xC, sizeof(char)); // 0xC
			memcpy(&sectionTable.fileInfos[i].unk1, uncompedBaseData + baseOffsetForFileInfos + 0xD, sizeof(char)); // 0xD

			baseOffsetForFileInfos += 0xE;

			printf("\tFILE INFO %u [%u %u %u %u]\n", sectionTable.fileInfos[i].ID, sectionTable.fileInfos[i].dataOffset, sectionTable.fileInfos[i].dataSize, sectionTable.fileInfos[i].section, sectionTable.fileInfos[i].unk1);
		}

		printf("}\n");

		free(uncompedBaseData);
		uncompedBaseData = nullptr;
	}
	catch (int e) {
		printf("An error occured while reading the provided bundle file.\n");
	}

	return true;
}

bool BundleV36::readBundleFileV0036(char* data) {

	int end = SRC_ENDIANLITTLE;

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

		hashTable = new unsigned int[header.numAssets]();

		
		/*
		for (int i = 0; i < header.numAssets; i++) {
			char* lbl = new char[128];
			memset(lbl, 0, 128);

			char* pos = strchr(sectionTable.fileLabelTable.fileLabels[i].label, ',');
			int size = pos - sectionTable.fileLabelTable.fileLabels[i].label;
			strncpy(lbl, sectionTable.fileLabelTable.fileLabels[i].label, size);
			hashTable[i] = assetIdGetHash_Base(lbl);
			printf("%s\n", lbl);

			delete[] lbl;
		}*/

		isReady = true;
		printf("The bundle file has finished reading.\n");
	}
	catch (int e) {
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
	isReady = false;

	FILE* currentFile = fopen(fileName, "rb");

	if (ferror(currentFile) != 0) {
		printf("Error occured while trying to open the file.\n");
		return false;
	}

	fseek(currentFile, 0L, SEEK_END);
	int length = ftell(currentFile);
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
	int length = ftell(currentFile);
	fseek(currentFile, 0L, SEEK_SET);

	char* data = (char*)malloc(length);

	fread(data, sizeof(char), length, currentFile);

	fclose(currentFile);

	readBundleFileV0036(data);

	free(data);
	return;
}

char* BundleV36::getFileData(char* fileName, int fileInfoIdx) {
	if (bundleData == nullptr) {
		printf("Error occured while trying to open the file.\n");
		return NULL;
	}

	int sectionOffset = getOffsetOfSection(sectionTable.fileInfos[fileInfoIdx].section - 1);

	int totalOffsetToDataSect = header.headerSize + header.sectionTableUncompedSize + header.fileTableUncompedSize;

	//Initialize our new section.
	char* sect = (char*)malloc(sectionTable.fileInfos[fileInfoIdx].dataSize);
	if (sect == nullptr) {
		throw("Memory Allocation Error for the section.");
		return NULL;
	}

	memset(sect, 0, sectionTable.fileInfos[fileInfoIdx].dataSize);

	printf("Bundle::getFileData() - Allocating file info %d in section %s of size %d.\n", fileInfoIdx, sectionTable.sectionLabels[sectionTable.fileInfos[fileInfoIdx].section - 1].label, sectionTable.fileInfos[fileInfoIdx].dataSize);

	// Just do this in a completely different way
	if (header.compression) {
		totalOffsetToDataSect = header.headerSize + header.sectionTableCompedSize + header.fileTableCompedSize;

		size_t sectCompedSize = getCompressedSizeofSection(sectionTable.fileInfos[fileInfoIdx].section - 1);
		size_t sectUncompedSize = getUncompressedSizeofSection(sectionTable.fileInfos[fileInfoIdx].section - 1);
		char* data = (char*)malloc(sectCompedSize);
		if (data == nullptr) {
			throw("Memory Allocation Error for the compression code with the compressed memory.");
			return NULL;
		}

		memcpy_s(data, sectCompedSize, bundleData + totalOffsetToDataSect + sectionOffset, sectCompedSize);

		char* outputData = InflateData(data, 6, sectCompedSize, sectUncompedSize);

		memcpy_s(sect, sectionTable.fileInfos[fileInfoIdx].dataSize, outputData + sectionTable.fileInfos[fileInfoIdx].dataOffset, sectionTable.fileInfos[fileInfoIdx].dataSize);

		return sect;
	}

	memcpy_s(sect, sectionTable.fileInfos[fileInfoIdx].dataSize, (bundleData + totalOffsetToDataSect + sectionOffset + sectionTable.fileInfos[fileInfoIdx].dataOffset), sectionTable.fileInfos[fileInfoIdx].dataSize);

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

	int offset = 0;
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

		memcpy(&header.headerHash, data + 0x14 + offset, sizeof(int));
		memcpy(&header.numOfFiles, data + 0x18 + offset, sizeof(int));
		memcpy(&header.numOfFileEntries, data + 0x1C + offset, sizeof(int));
		memcpy(&header.unk3, data + 0x20 + offset, sizeof(int));
		memcpy(&header.unk4, data + 0x24 + offset, sizeof(int));
		memcpy(&header.unk5, data + 0x28 + offset, sizeof(int));
		memcpy(&header.unk6, data + 0x2C + offset, sizeof(int));
		memcpy(&header.unk7, data + 0x30 + offset, sizeof(int));
		memcpy(&header.fileInfoTableOffset, data + 0x34 + offset, sizeof(int));

		memcpy(&header.headerSize, data + 0x38 + offset, sizeof(int));

		memcpy(&header.byteswapFlags, data + 0x3C + offset, sizeof(char));
		memcpy(&header.numSectionTypes, data + 0x3D + offset, sizeof(char));
		memcpy(&header.compression, data + 0x3E + offset, sizeof(char));
		memcpy(&header.isDebug, data + 0x3F + offset, sizeof(char));

		if (header.byteswapFlags == 1) {
			header.headerHash = flipEndian(header.headerHash); // Flip from big endian to little endian.
			header.headerSize = flipEndian(header.headerSize); // Flip from big endian to little endian.

			header.numOfFiles = flipEndian(header.numOfFiles); // Flip from big endian to little endian.
			header.numOfFileEntries = flipEndian(header.numOfFileEntries); // Flip from big endian to little endian.
			header.unk3 = flipEndian(header.unk3); // Flip from big endian to little endian.
			header.unk4 = flipEndian(header.unk4); // Flip from big endian to little endian.
			header.unk5 = flipEndian(header.unk5); // Flip from big endian to little endian.
			header.unk6 = flipEndian(header.unk6); // Flip from big endian to little endian.
			header.unk7 = flipEndian(header.unk7); // Flip from big endian to little endian.
			header.fileInfoTableOffset = flipEndian(header.fileInfoTableOffset); // Flip from big endian to little endian.
		}

		printf("\tCRC - %08X\n", header.headerHash);
		printf("}\n");

		int position = 0x40;
		for (int i = 0; i < header.numSectionTypes; i++) {
			memcpy_s(sectionEntries[i].sectionName, 8, data + position, 8);
			memcpy(&sectionEntries[i].unk1, data + position + 0x8, sizeof(int));
			memcpy(&sectionEntries[i].unk2, data + position + 0xC, sizeof(int));
			memcpy(&sectionEntries[i].uncompressedSize, data + position + 0x10, sizeof(int));
			memcpy(&sectionEntries[i].compressedSize, data + position + 0x24, sizeof(int));

			if (header.byteswapFlags == 1) {
				sectionEntries[i].uncompressedSize = flipEndian(sectionEntries[i].uncompressedSize); // Flip from big endian to little endian.
				sectionEntries[i].compressedSize = flipEndian(sectionEntries[i].compressedSize); // Flip from big endian to little endian.
			}

			position += 0x28;
		}

		// Run through the file info table next.
		char* dataSect = getSectionData(0);

		memcpy(&fileInfoTable.debugTable.sizeOfStringTable, dataSect + header.fileInfoTableOffset, sizeof(int));
		if (header.byteswapFlags == 1) {
			fileInfoTable.debugTable.sizeOfStringTable = flipEndian(fileInfoTable.debugTable.sizeOfStringTable); // Flip from big endian to little endian.
		}

		int offsToTable = header.fileInfoTableOffset + 4;
		if (fileInfoTable.debugTable.sizeOfStringTable != 0) {
			offsToTable += (0x4 * header.numOfFiles) + fileInfoTable.debugTable.sizeOfStringTable;

			fileInfoTable.debugTable.stringTableOffsets = new int[header.numOfFiles]();
			fileInfoTable.debugTable.fileNames = new char*[header.numOfFiles]();

			int strTablePos = header.fileInfoTableOffset + 4 + (4 * header.numOfFiles);

			for (int i = 0; i < header.numOfFiles; i++) {
				memcpy(&fileInfoTable.debugTable.stringTableOffsets[i], dataSect + header.fileInfoTableOffset + 4 + (4 * i), sizeof(int));

				if (header.byteswapFlags == 1) {
					fileInfoTable.debugTable.stringTableOffsets[i] = flipEndian(fileInfoTable.debugTable.stringTableOffsets[i]); // Flip from big endian to little endian.
				}
			}

			for (int i = 0; i < header.numOfFiles; i++) {
				int length = strlen(dataSect + strTablePos + fileInfoTable.debugTable.stringTableOffsets[i]);
				fileInfoTable.debugTable.fileNames[i] = new char[length + 1]();
				strcpy_s(fileInfoTable.debugTable.fileNames[i], length + 1, dataSect + strTablePos + fileInfoTable.debugTable.stringTableOffsets[i]);

				printf("FILE %d - %s\n", i, fileInfoTable.debugTable.fileNames[i]);
			}
		}

		fileInfoTable.fileInfoEntries = new FileInfoEntry[header.numOfFileEntries]();

		for (int i = 0; i < header.numOfFileEntries; i++) {
			memcpy(&fileInfoTable.fileInfoEntries[i].ID, dataSect + offsToTable + (0xE * i), sizeof(int));
			memcpy(&fileInfoTable.fileInfoEntries[i].dataOffset, dataSect + offsToTable + (0xE * i) + 4, sizeof(int));
			memcpy(&fileInfoTable.fileInfoEntries[i].dataSize, dataSect + offsToTable + (0xE * i) + 8, sizeof(int));
			memcpy(&fileInfoTable.fileInfoEntries[i].section, dataSect + offsToTable + (0xE * i) + 0xC, sizeof(char));
			memcpy(&fileInfoTable.fileInfoEntries[i].unk1, dataSect + offsToTable + (0xE * i) + 0xD, sizeof(char));

			if (header.byteswapFlags == 1) {
				fileInfoTable.fileInfoEntries[i].ID = flipEndian(fileInfoTable.fileInfoEntries[i].ID); // Flip from big endian to little endian.
				fileInfoTable.fileInfoEntries[i].dataOffset = flipEndian(fileInfoTable.fileInfoEntries[i].dataOffset); // Flip from big endian to little endian.
				fileInfoTable.fileInfoEntries[i].dataSize = flipEndian(fileInfoTable.fileInfoEntries[i].dataSize); // Flip from big endian to little endian.
			}

			printf("FILE INFO %u [%u %u %u %u]\n", fileInfoTable.fileInfoEntries[i].ID, fileInfoTable.fileInfoEntries[i].dataOffset, fileInfoTable.fileInfoEntries[i].dataSize, fileInfoTable.fileInfoEntries[i].section, fileInfoTable.fileInfoEntries[i].unk1);
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
	catch (int ex) {
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

char* BundleV31::getSectionData(int section) {
	if (bundleData == nullptr) {
		printf("We somehow don't have bundle data for this. This is bad.\n");
		return nullptr;
	}

	if (section < 0 || section >= header.numSectionTypes) {
		printf("Invalid range for the section index.\n");
		return nullptr;
	}

	char* sectData = new char[sectionEntries[section].uncompressedSize];

	if (header.compression) {
		char* data = (char*)malloc(sectionEntries[section].compressedSize);

		size_t sectCompedSize = sectionEntries[section].compressedSize;
		size_t sectUncompedSize = sectionEntries[section].uncompressedSize;

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

	memcpy_s(sectData, sectionEntries[section].uncompressedSize, bundleData + header.headerSize + getOffsetOfSection(section), sectionEntries[section].uncompressedSize);

	return sectData;
}

char* BundleV31::getFileData(char* fileName, int fileInfoIdx) {
	if (bundleData == nullptr) {
		printf("Error occured while trying to open the file.\n");
		return NULL;
	}

	int sectionOffset = getOffsetOfSection(fileInfoTable.fileInfoEntries[fileInfoIdx].section - 1);

	int totalOffsetToDataSect = header.headerSize;

	//Initialize our new section.
	char* sect = (char*)malloc(fileInfoTable.fileInfoEntries[fileInfoIdx].dataSize);
	if (sect == nullptr) {
		throw("Memory Allocation Error for the section.");
		return NULL;
	}

	memset(sect, 0, fileInfoTable.fileInfoEntries[fileInfoIdx].dataSize);

	printf("Bundle::getFileData() - Allocating file info %d in section %s of size %d.\n", fileInfoIdx, sectionEntries[fileInfoTable.fileInfoEntries[fileInfoIdx].section - 1].sectionName, fileInfoTable.fileInfoEntries[fileInfoIdx].dataSize);

	// Just do this in a completely different way
	if (header.compression) {
		totalOffsetToDataSect = header.headerSize;

		size_t sectCompedSize = sectionEntries[fileInfoTable.fileInfoEntries[fileInfoIdx].section - 1].compressedSize;
		size_t sectUncompedSize = sectionEntries[fileInfoTable.fileInfoEntries[fileInfoIdx].section - 1].uncompressedSize;
		char* data = (char*)malloc(sectCompedSize);
		if (data == nullptr) {
			throw("Memory Allocation Error for the compression code with the compressed memory.");
			return NULL;
		}

		memcpy_s(data, sectCompedSize, bundleData + totalOffsetToDataSect + sectionOffset, sectCompedSize);

		char* outputData = InflateData(data, 6, sectCompedSize, sectUncompedSize);

		memcpy_s(sect, fileInfoTable.fileInfoEntries[fileInfoIdx].dataSize, outputData + fileInfoTable.fileInfoEntries[fileInfoIdx].dataOffset, fileInfoTable.fileInfoEntries[fileInfoIdx].dataSize);

		return sect;
	}

	memcpy_s(sect, fileInfoTable.fileInfoEntries[fileInfoIdx].dataSize, (bundleData + totalOffsetToDataSect + sectionOffset + fileInfoTable.fileInfoEntries[fileInfoIdx].dataOffset), fileInfoTable.fileInfoEntries[fileInfoIdx].dataSize);

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

	int offset = 0;
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

		memcpy(&header.numOfFiles, data + 0x14 + offset, sizeof(int));
		memcpy(&header.numOfFileEntries, data + 0x18 + offset, sizeof(int));
		memcpy(&header.unk3, data + 0x1C + offset, sizeof(int));
		memcpy(&header.unk4, data + 0x20 + offset, sizeof(int));
		memcpy(&header.unk5, data + 0x24 + offset, sizeof(int));
		memcpy(&header.unk6, data + 0x28 + offset, sizeof(int));
		memcpy(&header.fileInfoTableOffset, data + 0x2C + offset, sizeof(int));

		memcpy(&header.headerSize, data + 0x30 + offset, sizeof(int));

		memcpy(&header.unk8, data + 0x34 + offset, sizeof(char));
		memcpy(&header.numSectionTypes, data + 0x35 + offset, sizeof(char));
		memcpy(&header.compression, data + 0x36 + offset, sizeof(char));
		memcpy(&header.unk9, data + 0x37 + offset, sizeof(char));

		int position = 0x38;
		for (int i = 0; i < header.numSectionTypes; i++) {
			memcpy_s(sectionEntries[i].sectionName, 8, data + position, 8);
			memcpy(&sectionEntries[i].uncompressedSize, data + position + 0x8, sizeof(int));
			memcpy(&sectionEntries[i].unk1, data + position + 0xC, sizeof(int));
			memcpy(&sectionEntries[i].compressedSize, data + position + 0x20, sizeof(int));

			position += 0x24;
		}

		// Run a few checks
		char* dataSect = getSectionData(0);

		memcpy(&fileInfoTable.debugTable.sizeOfStringTable, dataSect + header.fileInfoTableOffset, sizeof(int));

		int offsToTable = header.fileInfoTableOffset + 4;
		if (fileInfoTable.debugTable.sizeOfStringTable != 0) {
			offsToTable += (0x4 * header.numOfFiles) + fileInfoTable.debugTable.sizeOfStringTable;

			fileInfoTable.debugTable.stringTableOffsets = new int[header.numOfFiles]();
			fileInfoTable.debugTable.fileNames = new char* [header.numOfFiles]();

			int strTablePos = header.fileInfoTableOffset + 4 + (4 * header.numOfFiles);

			for (int i = 0; i < header.numOfFiles; i++) {
				memcpy(&fileInfoTable.debugTable.stringTableOffsets[i], dataSect + header.fileInfoTableOffset + 4 + (4 * i), sizeof(int));
			}

			for (int i = 0; i < header.numOfFiles; i++) {
				int length = strlen(dataSect + strTablePos + fileInfoTable.debugTable.stringTableOffsets[i]);
				fileInfoTable.debugTable.fileNames[i] = new char[length + 1]();
				strcpy_s(fileInfoTable.debugTable.fileNames[i], length + 1, dataSect + strTablePos + fileInfoTable.debugTable.stringTableOffsets[i]);

				printf("FILE %d - %s\n", i, fileInfoTable.debugTable.fileNames[i]);
			}
		}

		fileInfoTable.fileInfoEntries = new FileInfoEntry[header.numOfFileEntries]();

		for (int i = 0; i < header.numOfFileEntries; i++) {
			memcpy(&fileInfoTable.fileInfoEntries[i].ID, dataSect + offsToTable + (0xE * i), sizeof(int));
			memcpy(&fileInfoTable.fileInfoEntries[i].dataOffset, dataSect + offsToTable + (0xE * i) + 4, sizeof(int));
			memcpy(&fileInfoTable.fileInfoEntries[i].dataSize, dataSect + offsToTable + (0xE * i) + 8, sizeof(int));
			memcpy(&fileInfoTable.fileInfoEntries[i].section, dataSect + offsToTable + (0xE * i) + 0xC, sizeof(char));
			memcpy(&fileInfoTable.fileInfoEntries[i].unk1, dataSect + offsToTable + (0xE * i) + 0xD, sizeof(char));

			printf("FILE INFO %u [%u %u %u %u]\n", fileInfoTable.fileInfoEntries[i].ID, fileInfoTable.fileInfoEntries[i].dataOffset, fileInfoTable.fileInfoEntries[i].dataSize, fileInfoTable.fileInfoEntries[i].section, fileInfoTable.fileInfoEntries[i].unk1);
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
	catch (int ex) {
	}

	return true;
}

BundleV26::~BundleV26() {
	if (bundleData != nullptr) {
		free(bundleData);
		bundleData = nullptr;
	}
}

char* BundleV26::getSectionData(int section) {
	if (bundleData == nullptr) {
		printf("We somehow don't have bundle data for this. This is bad.\n");
		return nullptr;
	}

	if (section < 0 || section >= header.numSectionTypes) {
		printf("Invalid range for the section index.\n");
		return nullptr;
	}

	char* sectData = new char[sectionEntries[section].uncompressedSize];

	if (header.compression) {
		char* data = (char*)malloc(sectionEntries[section].compressedSize);

		size_t sectCompedSize = sectionEntries[section].compressedSize;
		size_t sectUncompedSize = sectionEntries[section].uncompressedSize;

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

	memcpy_s(sectData, sectionEntries[section].uncompressedSize, bundleData + header.headerSize + getOffsetOfSection(section), sectionEntries[section].uncompressedSize);

	return sectData;
}

char* BundleV26::getFileData(char* fileName, int fileInfoIdx) {
	if (bundleData == nullptr) {
		printf("Error occured while trying to open the file.\n");
		return NULL;
	}

	int sectionOffset = getOffsetOfSection(fileInfoTable.fileInfoEntries[fileInfoIdx].section - 1);

	int totalOffsetToDataSect = header.headerSize;

	//Initialize our new section.
	char* sect = (char*)malloc(fileInfoTable.fileInfoEntries[fileInfoIdx].dataSize);
	if (sect == nullptr) {
		throw("Memory Allocation Error for the section.");
		return NULL;
	}

	memset(sect, 0, fileInfoTable.fileInfoEntries[fileInfoIdx].dataSize);

	printf("Bundle::getFileData() - Allocating file info %d in section %s of size %d.\n", fileInfoIdx, sectionEntries[fileInfoTable.fileInfoEntries[fileInfoIdx].section - 1].sectionName, fileInfoTable.fileInfoEntries[fileInfoIdx].dataSize);

	// Just do this in a completely different way
	if (header.compression) {
		totalOffsetToDataSect = header.headerSize;

		size_t sectCompedSize = sectionEntries[fileInfoTable.fileInfoEntries[fileInfoIdx].section - 1].compressedSize;
		size_t sectUncompedSize = sectionEntries[fileInfoTable.fileInfoEntries[fileInfoIdx].section - 1].uncompressedSize;
		char* data = (char*)malloc(sectCompedSize);
		if (data == nullptr) {
			throw("Memory Allocation Error for the compression code with the compressed memory.");
			return NULL;
		}

		memcpy_s(data, sectCompedSize, bundleData + totalOffsetToDataSect + sectionOffset, sectCompedSize);

		char* outputData = InflateData(data, 6, sectCompedSize, sectUncompedSize);

		memcpy_s(sect, fileInfoTable.fileInfoEntries[fileInfoIdx].dataSize, outputData + fileInfoTable.fileInfoEntries[fileInfoIdx].dataOffset, fileInfoTable.fileInfoEntries[fileInfoIdx].dataSize);

		return sect;
	}

	memcpy_s(sect, fileInfoTable.fileInfoEntries[fileInfoIdx].dataSize, (bundleData + totalOffsetToDataSect + sectionOffset + fileInfoTable.fileInfoEntries[fileInfoIdx].dataOffset), fileInfoTable.fileInfoEntries[fileInfoIdx].dataSize);

	printf("Bundle::getFileData() - File Data for entry %d successfully obtained.\n", fileInfoIdx);

	return sect;
}