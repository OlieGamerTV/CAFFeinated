#pragma once
#include "Bundle.h"
#include "XWaveBank.h"

enum EntryType : char {
	ENTRY_NONE,
	ENTRY_BUNDLE,
	ENTRY_DNBW
};

struct StreamFileEntry {
	unsigned int aid = 0;
	int offset = 0;
	int dataSize = 0;
};

struct StreamHeader {
	int offset = 0xC;
	int totalFileTotal = 0;
	unsigned int timestamp = 0;

	int referenceTableCount = 0;
	unsigned int* referenceTable = nullptr;
};

struct StreamEntry {
	BundleV36* bundleFile = nullptr;
	DNBW* waveBankFile = nullptr;
	EntryType entryType = ENTRY_NONE;
};

struct StreamBundle {
public:
	char* fileData = nullptr;
	StreamHeader header;
	StreamFileEntry* fileEntries = nullptr;

	StreamEntry* bundleFiles = nullptr;

	bool isReady = false;

	~StreamBundle();

	void readStandaloneStreamBundleFile(char* fileName);
	void readStreamBundleFile(char* data);

	// Allocates and returns the target file data.
	char* getFileData(int fileIdx);
	char* getBundleFileData(char* fileName, int fileAid, int fileIdx);

	// Gets the offset of the target file by adding the size of the files before it.
	int getOffsetOfFile(unsigned int aid) {
		int offset = 0;
		for (int i = 0; i < header.totalFileTotal; i++) {
			if (fileEntries[i].aid == aid) {
				return fileEntries[i].offset;
			}
		}
		return 0;
	}

	//IMGUI Popups
	bool addStreamedReference(bool* open);
};