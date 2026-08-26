#pragma once
#ifndef _STREAMBUNDLE
#define _STREAMBUNDLE
#include "Bundle.h"
#include "XWaveBank.h"

enum EntryType : char {
	ENTRY_NONE,
	ENTRY_BUNDLE,
	ENTRY_DNBW
};

struct StreamFileEntry {
	uint32_t aid = 0;
	int32_t offset = 0;
	int32_t dataSize = 0;
};

struct StreamHeader {
	int32_t offset = 0xC;
	int32_t totalFileTotal = 0;
	uint32_t timestamp = 0;

	int32_t referenceTableCount = 0;
	uint32_t* referenceTable = nullptr;
};

struct StreamEntry {
	BundleV36 bundleFile;
	DNBW waveBankFile;
	EntryType entryType = ENTRY_NONE;
};

struct StreamBundle {
public:
	char* fileData = nullptr;
	StreamHeader header;
	StreamFileEntry* fileEntries = nullptr;

	StreamEntry* bundleFiles = nullptr;

	bool isReady = false;

	void ClearActiveData();

	bool readStandaloneStreamBundleFile(char* fileName);
	bool readStreamBundleFile(char* data);

	// Allocates and returns the target file data.
	char* getFileData(int32_t fileIdx);
	char* getBundleFileData(char* fileName, int32_t fileAid, int32_t fileIdx);

	// Gets the offset of the target file by adding the size of the files before it.
	int32_t getOffsetOfFile(uint32_t aid) {
		int32_t offset = 0;
		for (int32_t i = 0; i < header.totalFileTotal; i++) {
			if (fileEntries[i].aid == aid) {
				return fileEntries[i].offset;
			}
		}
		return 0;
	}

	//IMGUI Popups
	bool addStreamedReference(bool* open);
};
#endif