#ifndef _STREAMBUNDLE
#define _STREAMBUNDLE
#include "Bundle.h"
#include "XWaveBank.h"

#ifdef _WIN32
#pragma once
#endif

static const float STREAMBUNDLE_NOVVERSION = 281.4100341796879;
static const float STREAMBUNDLE_SEPTVERSION = 173.369964599609;
static const uint32_t STREAMBUNDLE_IDENTIFIER_NOVVERSION = 0x438CB47C; // Used in Nuts & Bolts
static const uint32_t STREAMBUNDLE_IDENTIFIER_SEPT20VERSION = 0x433051EC; // Used in final Kinect Sports
static const uint32_t STREAMBUNDLE_IDENTIFIER_SEPT18VERSION = 0x432D5EB6; // Used in E3 2010 Kinect Sports

enum EntryType : char {
	ENTRY_NONE,
	ENTRY_BUNDLE,
	ENTRY_DNBW
};

struct StreamFileEntry {
	uint32_t assetId = 0;
	int32_t startOffset = 0;
	int32_t assetSize = 0;
};

struct StreamHeader {
	uint32_t version; // 0x00
	int32_t entrySize = 0xC; // 0x04
	int32_t numEntries = 0; // 0x08
	time_t timestamp = 0; // 0x0C
	int32_t numParents = 0; // 0x10
	std::vector<uint32_t> referenceTable;
};

struct StreamEntry {
	BundleFile bundleFile;
	DNBW waveBankFile;
	EntryType entryType = ENTRY_NONE;
};

struct StreamBundle {
public:
	char* fileData = nullptr;
	StreamHeader header;
	std::vector<StreamFileEntry> fileEntries;

	std::vector<StreamEntry> bundleFiles;

	bool isReady = false;

	void ClearActiveData();

	bool readStandaloneStreamBundleFile(char* fileName);
	bool readStreamBundleFile(char* data);

	bool readStreamBundleFileHeader();
	bool readStreamBunfleFileEntries();

	// Allocates and returns the target file data.
	char* getFileData(int32_t fileIdx);
	char* getBundleFileData(char* fileName, int32_t fileAid, int32_t fileIdx);

	// Gets the offset of the target file by adding the size of the files before it.
	int32_t getOffsetOfFile(uint32_t aid) {
		int32_t offset = 0;
		for (int32_t i = 0; i < header.numEntries; i++) {
			if (fileEntries[i].assetId == aid) {
				return fileEntries[i].startOffset;
			}
		}
		return 0;
	}

	//IMGUI Popups
	bool addStreamedReference(bool* open);
};
#endif