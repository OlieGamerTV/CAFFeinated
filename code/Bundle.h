#pragma once
#ifndef _BUNDLE
#define _BUNDLE
#include <vector>

static const int32_t PADDINGSIZE_TEXTUREGPU = 0x1000;
static const int32_t PADDINGSIZE_MODELGPU = 0x20;
static const int32_t PADDINGSIZE_STANDARD = 0x10;
static const int32_t PADDINGSIZE_MODELSTREAM = 4;

enum CaffType : int32_t {
	NONE = 0,
	GHOUL_BUNDLE,
	GHOUL_DEMAND,
	BUNDLEV40,
	BUNDLEV36,
	BUNDLEV26,
	BUNDLEV31,
	PINATA_DBBUNDLE,
	PINATA_PKG,
	NB_STREAMBUNDLE,
	RR_RPK,
};

struct ByteSwapGroupEntry {
	int32_t section = 0;
	int32_t count = 0;
	int32_t numItemsToByteswap = 0;
	int32_t size = 0;
	int32_t stride = 0;
};

struct FileInfoEntry {
	int32_t asset = 0;
	int32_t start = 0;
	int32_t size = 0;
	char sectionType = 0;
	char startAlignment = 0;
};

struct SectionEntry {
public:
	int32_t strOffset = 0;
	char alignment = 0;
	int32_t poolSection = 0;
	int32_t size = 0;
	int32_t base = 0;
	int32_t offset = 0;
	int32_t overlap = 0;
	int32_t loadSize = 0;
	int32_t compressedSize = 0;
};

struct TableInfo {
public:
	int32_t size = 0;
	uint32_t base = 0;
	int32_t overlap = 0;
	int32_t loadSize = 0;
	int32_t compressedSize = 0;
};

struct SectionLabel {
public:
	char label[16];
};

struct FileLabelTable {
	int32_t totalLabelTableSize = 0;
	char** fileLabels;
};

struct SectionTable {
public:
	SectionEntry entries[6];
	SectionLabel sectionLabels[6];
	int32_t* fileLabelOffsets = nullptr;
	FileLabelTable fileLabelTable;

	int32_t adbStringLen = 0;
	char* adbString = nullptr;

	FileInfoEntry* fileInfos = nullptr;

	int32_t sectionLabelOffset = 0;
	int32_t sectionFileLabelOffset = 0;
	int32_t fileInfosOffset = 0;

	~SectionTable();
};

struct FileToSectInfo {
	int32_t fileInfoID[1024];
	int32_t fileLabelID[1024];
};

struct V31DebugNameTable {
public:
	int32_t sizeOfStringTable;
	int32_t* stringTableOffsets;
	char** fileNames = nullptr;
};

struct V31FileTable {
public:
	V31DebugNameTable debugTable;
	FileInfoEntry* fileInfoEntries;
};

/// <summary>
/// The general header of the CAFF bundle.
/// </summary>
struct CaffHeader {
public:
	char identifier[4]; // Should always equal "CAFF".
	char versionString[16]; // Depends on what version of CAFF you're opening. The most common being "07.08.06.0036".
};

struct V26SectionEntry {
	char sectionName[8];
	int32_t size;
	int32_t unk1;
	int32_t base = 0;
	int32_t offset = 0;
	int32_t overlap = 0;
	int32_t loadSize = 0;
	int32_t compressedSize;
};

struct CaffV26Header : CaffHeader {
public:
	int32_t numOfFiles; // 0x14
	int32_t numOfFileEntries; // 0x18
	int32_t unk3; // 0x1C
	int32_t unk4; // 0x20
	int32_t unk5; // 0x24
	int32_t unk6; // 0x28
	int32_t fileInfoTableOffset; // 0x2C
	int32_t headerSize; // 0x30

	char unk8 = 0; // 0x34
	char numSectionTypes = 0; // 0x35
	char compression = 0; // 0x36
	char unk9 = 0; // 0x37
};

enum V31FileType : int32_t {
	V31_Unknown,
	V31_Texture,
	V31_ConkerText,
	V31_Text,
	V31_PDZPackage,
	V31_KameoDB
};

struct V31SectionEntry {
	char sectionType[11];
	char alignment = 0;
	int32_t poolSection = 0;
	int32_t size = 0;
	int32_t base = 0;
	int32_t offset = 0;
	int32_t overlap = 0;
	int32_t loadSize = 0;
	int32_t compressedSize = 0;
};

struct CaffV31Header : CaffHeader {
public:
	uint32_t headerHash = 0; // 0x14
	int32_t numAssets = 0; // 0x18
	int32_t numSections = 0; // 0x1C
	int32_t numByteswapGroups = 0; // 0x20
	int32_t numByteswaps = 0; // 0x24
	int32_t numRelocationGroups = 0; // 0x28
	int32_t numRelocations = 0; // 0x2C
	int32_t numPoolItems = 0; // 0x30
	int32_t tableSectionOffset = 0; // 0x34
	int32_t headerSize = 0; // 0x38

	char byteswapFlags = 0; // 0x3C
	char numSectionTypes = 0; // 0x3D
	char compression = 0; // 0x3E
	char numPools = 0; // 0x3F
};

struct CaffV36Header : CaffHeader {
public:
	int32_t headerSize = 0x78; // 0x14
	uint32_t headerHash = 0; // 0x18
	int32_t numAssets = 0; // 0x1C
	int32_t numSections = 0; // 0x20
	int32_t numByteswapGroups = 0; // 0x24
	int32_t numByteswaps = 0; // 0x28
	int32_t numRelocationGroups = 0; // 0x2C
	int32_t numRelocations = 0; // 0x30
	int32_t numMappedByteswapGroups = 0; // 0x34
	int32_t numMappedByteswaps = 0; // 0x38
	int32_t numMappedRelocationGroups = 0; // 0x3C
	int32_t numMappedRelocations = 0; // 0x40
	int32_t numPoolItems = 0; // 0x44
	char byteswapFlags = 0; // 0x48
	char numSectionTypes = 0; // 0x49
	char compression = 0; // 0x4A
	char numPools = 0; // 0x4B
	int32_t sectionTypeNamesBufferLen = 0; // 0x4C, the length of the symbol string table in bytes.

	TableInfo sectTable;
	TableInfo fileTable;
};

// Pretty much the only thing different between V0036 and V0040 is 
struct CaffV40Header : CaffHeader {
public:
	uint32_t timestamp; // 0x18
	int32_t headerSize = 0x80; // 0x1C
	uint32_t headerHash = 0; // 0x20
	int32_t numAssets = 0; // 0x24
	int32_t numSections = 0; // 0x28
	int32_t numByteswapGroups = 0; // 0x2C
	int32_t numByteswaps = 0; // 0x30
	int32_t numRelocationGroups = 0; // 0x34
	int32_t numRelocations = 0; // 0x38
	int32_t numMappedByteswapGroups = 0; // 0x3C
	int32_t numMappedByteswaps = 0; // 0x40
	int32_t numMappedRelocationGroups = 0; // 0x44
	int32_t numMappedRelocations = 0; // 0x48
	int32_t numPoolItems = 0; // 0x4C
	char byteswapFlags = 0; // 0x50
	char numSectionTypes = 0; // 0x51
	char compression = 0; // 0x52
	char numPools = 0; // 0x53
	int32_t sectionTypeNamesBufferLen = 0; // 0x54, the length of the symbol string table in bytes.

	TableInfo sectTable;
	TableInfo fileTable;
};

struct BundleV40 {
public:
	CaffV40Header header;
	SectionTable sectionTable;

	uint32_t* hashTable = nullptr;

	char* bundleData = nullptr;

	bool isReady = false;

	~BundleV40();

	bool readStandaloneBundleFile(char* fileName);
	bool readBundleFileV0040(char* data);

	bool readBundleHeaderV0040(char* data);
	bool readBundleSectionFileV0040(char* data);

	void writeStandaloneBundleFile(char* fileName);

	void getArrayOfData();

	// Allocates and returns the target file data.
	char* getFileData(char* fileName, int32_t fileInfoIdx);

	bool isTextureGpuSection(int32_t sectId) {
		if (strcmp(sectionTable.sectionLabels[sectId].label, ".texturegpu") == 0) {
			return true;
		}
		return false;
	}

	bool isGPUSection(int32_t sectId) {
		if (strcmp(sectionTable.sectionLabels[sectId].label, ".gpu") == 0) {
			return true;
		}
		if (strcmp(sectionTable.sectionLabels[sectId].label, ".texturegpu") == 0) {
			return true;
		}
		return false;
	}

	bool isGPUCachedSection(int32_t sectId) {
		if (strcmp(sectionTable.sectionLabels[sectId].label, ".gpucached") == 0) {
			return true;
		}
		return false;
	}

	bool isStreamSection(int32_t sectId) {
		if (strcmp(sectionTable.sectionLabels[sectId].label, ".stream") == 0) {
			return true;
		}
		return false;
	}

	// Gets the offset of the target section by adding the size of the sections before it.
	// If using the section ID from "fileInfos", subtract by 1 before passing the value in.
	int32_t getOffsetOfSection(int32_t section) {
		int32_t offset = 0;
		for (int32_t i = 0; i < header.numSectionTypes; i++) {
			if (i == section) {
				break;
			}
			if (header.compression) {
				offset = offset + sectionTable.entries[i].compressedSize;
			}
			else {
				offset = offset + sectionTable.entries[i].size;
			}
		}
		return offset;
	}

	size_t getBaseSizeOfCompedBundle() const {
		return header.headerSize + header.fileTable.compressedSize + header.sectTable.compressedSize;
	}

	size_t getTotalSizeOfCompedBundle() const {
		size_t totalSize = header.headerSize + header.fileTable.compressedSize + header.sectTable.compressedSize;

		for (int32_t i = 0; i < header.numSectionTypes; i++) {
			totalSize += sectionTable.entries[i].compressedSize;
		}

		return totalSize;
	}

	size_t getDefaultSizeofSection(int32_t section) {
		for (int32_t i = 0; i < header.numSectionTypes; i++) {
			if (i == section) {
				if (header.compression) {
					return sectionTable.entries[i].compressedSize;
				}
				else {
					return sectionTable.entries[i].size;
				}
			}
		}
		return 0;
	}

	size_t getUncompressedSizeofSection(int32_t section) {
		for (int32_t i = 0; i < header.numSectionTypes; i++) {
			if (i == section) {
				return sectionTable.entries[i].size;
			}
		}
		return 0;
	}

	size_t getCompressedSizeofSection(int32_t section) {
		for (int32_t i = 0; i < header.numSectionTypes; i++) {
			if (i == section) {
				return sectionTable.entries[i].compressedSize;
			}
		}
		return 0;
	}


	int32_t getFileIdxFromSymbol(const char* symbol) {
		if (sectionTable.fileLabelTable.fileLabels == nullptr) {
			printf("fileLabels is null.\n");
			return -1;
		}


		for (int32_t i = 0; i < header.numAssets; i++) {
			char* label = sectionTable.fileLabelTable.fileLabels[i];

			if (strcmp(label, symbol) == 0) {
				printf("Target symbol %s was found in the bundle at index %d.\n", symbol, i);
				return i;
			}
		}
		printf("Target symbol %s was not found in the bundle\n", symbol);
		return -1;
	}

	int32_t GetMatchingFileInfoIdx(int32_t fileIdx, int32_t section) {
		if (sectionTable.fileInfos == NULL) return -1;

		for (int32_t i = 0; i < header.numSections; i++) {
			if (sectionTable.fileInfos[i].asset == fileIdx && sectionTable.fileInfos[i].sectionType == section) {
				//printf("Target IDX %d was found in the bundle at index %d.\n", fileIdx, i);
				return i;
			}
		}
		//printf("Target IDX %d was not found in the bundle\n", fileIdx);
		return -1;
	}

	int32_t getFileInfoIdxFromFileIdx(const char* symbol, int32_t sectionIdx) {
		int32_t fileIdx = getFileIdxFromSymbol(symbol);
		if (fileIdx == -1) return -1;

		return getFileInfoIdxFromFileIdx(fileIdx, sectionIdx);
	}

	int32_t getFileInfoIdxFromFileIdx(int32_t fileIdx, int32_t sectionIdx) {
		if (sectionTable.fileInfos == NULL) return -1;

		for (int32_t i = 0; i < header.numSections; i++) {
			if (sectionTable.fileInfos[i].asset == fileIdx + 1 && sectionTable.fileInfos[i].sectionType == sectionIdx + 1) {
				printf("Target IDX %d was found in the bundle at index %d.\n", fileIdx, i);
				return i;
			}
		}
		printf("Target IDX %d was not found in the bundle\n", fileIdx);
		return -1;
	}

	int32_t getGPUFileInfoIdxFromFileIdx(const char* symbol) {
		int32_t fileIdx = getFileIdxFromSymbol(symbol);
		if (fileIdx == -1) return -1;

		return getGPUFileInfoIdxFromFileIdx(fileIdx);
	}

	int32_t getGPUFileInfoIdxFromFileIdx(int32_t fileIdx) {
		if (sectionTable.fileInfos == NULL) return -1;

		for (int32_t i = 0; i < header.numSections; i++) {
			if (sectionTable.fileInfos[i].asset == fileIdx + 1 && isGPUSection(sectionTable.fileInfos[i].sectionType - 1)) {
				printf("Target IDX %d was found in the bundle at index %d.\n", fileIdx, i);
				return i;
			}
		}
		printf("Target IDX %d was not found in the bundle\n", fileIdx);
		return -1;
	}

	int32_t doesFileExist(const char* fileName) {
		if (sectionTable.fileLabelTable.fileLabels == nullptr) {
			return 0;
		}

		for (int32_t i = 0; i < header.numAssets; i++) {
			char* symbol = sectionTable.fileLabelTable.fileLabels[i];

			if (strcmp(symbol, fileName) == 0) {
				return 1;
			}
		}
		return 0;
	}

	// Consts
	int32_t headerSize() const {
		return header.headerSize;
	}

	uint32_t bundleCRC() const {
		return header.headerHash;
	}

	int32_t numOfSymbols() const {
		return header.numAssets;
	}

	int32_t numOfFileParts() const {
		return header.numSections;
	}
};

struct BundleV36 {
public:
	CaffV36Header header;
	SectionTable sectionTable;

	uint32_t* hashTable = nullptr;

	char* bundleData = nullptr;

	bool isDirty = false;

	~BundleV36();

	bool readStandaloneBundleFile(char* fileName);
	bool readBundleFileV0036(char* data);

	bool readBundleHeaderV0036(char* data);
	bool readBundleSectionFileV0036(char* data);

	void writeStandaloneBundleFile(char* fileName);

	void getArrayOfData();

	// Allocates and returns the target file data.
	char* getFileData(char* fileName, int32_t fileInfoIdx);

	bool isTextureGpuSection(int32_t sectId) {
		if (strcmp(sectionTable.sectionLabels[sectId].label, ".texturegpu") == 0) {
			return true;
		}
		return false;
	}

	bool isGPUSection(int32_t sectId) {
		if (strcmp(sectionTable.sectionLabels[sectId].label, ".gpu") == 0) {
			return true;
		}
		if (strcmp(sectionTable.sectionLabels[sectId].label, ".texturegpu") == 0) {
			return true;
		}
		return false;
	}

	bool isGPUCachedSection(int32_t sectId) {
		if (strcmp(sectionTable.sectionLabels[sectId].label, ".gpucached") == 0) {
			return true;
		}
		return false;
	}

	bool isStreamSection(int32_t sectId) {
		if (strcmp(sectionTable.sectionLabels[sectId].label, ".stream") == 0) {
			return true;
		}
		return false;
	}

	// Gets the offset of the target section by adding the size of the sections before it.
	// If using the section ID from "fileInfos", subtract by 1 before passing the value in.
	int32_t getOffsetOfSection(int32_t section) {
		int32_t offset = 0;
		for (int32_t i = 0; i < header.numSectionTypes; i++) {
			if (i == section) {
				break;
			}
			if (header.compression) {
				offset = offset + sectionTable.entries[i].compressedSize;
			}
			else {
				offset = offset + sectionTable.entries[i].size;
			}
		}
		return offset;
	}

	size_t getBaseSizeOfCompedBundle() const {
		return header.headerSize + header.fileTable.compressedSize + header.sectTable.compressedSize;
	}

	size_t getTotalSizeOfCompedBundle() const {
		size_t totalSize = header.headerSize + header.fileTable.compressedSize + header.sectTable.compressedSize;

		for (int32_t i = 0; i < header.numSectionTypes; i++) {
			totalSize += sectionTable.entries[i].compressedSize;
		}

		return totalSize;
	}

	size_t getDefaultSizeofSection(int32_t section) {
		for (int32_t i = 0; i < header.numSectionTypes; i++) {
			if (i == section) {
				if (header.compression) {
					return sectionTable.entries[i].compressedSize;
				}
				else {
					return sectionTable.entries[i].size;
				}
			}
		}
		return 0;
	}

	size_t getUncompressedSizeofSection(int32_t section) {
		for (int32_t i = 0; i < header.numSectionTypes; i++) {
			if (i == section) {
				return sectionTable.entries[i].size;
			}
		}
		return 0;
	}

	size_t getCompressedSizeofSection(int32_t section) {
		for (int32_t i = 0; i < header.numSectionTypes; i++) {
			if (i == section) {
				return sectionTable.entries[i].compressedSize;
			}
		}
		return 0;
	}


	int32_t getFileIdxFromSymbol(const char* symbol) {
		if (sectionTable.fileLabelTable.fileLabels == nullptr) {
			printf("fileLabels is null.\n");
			return -1;
		}


		for (int32_t i = 0; i < header.numAssets; i++) {
			char* label = sectionTable.fileLabelTable.fileLabels[i];

			if (strcmp(label, symbol) == 0) {
				printf("Target symbol %s was found in the bundle at index %d.\n", symbol, i);
				return i;
			}
		}
		printf("Target symbol %s was not found in the bundle\n", symbol);
		return -1;
	}

	int32_t GetMatchingFileInfoIdx(int32_t fileIdx, int32_t section) {
		if (sectionTable.fileInfos == NULL) return -1;

		for (int32_t i = 0; i < header.numSections; i++) {
			if (sectionTable.fileInfos[i].asset == fileIdx && sectionTable.fileInfos[i].sectionType == section) {
				//printf("Target IDX %d was found in the bundle at index %d.\n", fileIdx, i);
				return i;
			}
		}
		//printf("Target IDX %d was not found in the bundle\n", fileIdx);
		return -1;
	}

	int32_t getFileInfoIdxFromFileIdx(const char* symbol, int32_t sectionIdx) {
		int32_t fileIdx = getFileIdxFromSymbol(symbol);
		if (fileIdx == -1) return -1;

		return getFileInfoIdxFromFileIdx(fileIdx, sectionIdx);
	}

	int32_t getFileInfoIdxFromFileIdx(int32_t fileIdx, int32_t sectionIdx) {
		if (sectionTable.fileInfos == NULL) return -1;

		for (int32_t i = 0; i < header.numSections; i++) {
			if (sectionTable.fileInfos[i].asset == fileIdx + 1 && sectionTable.fileInfos[i].sectionType == sectionIdx + 1) {
				printf("Target IDX %d was found in the bundle at index %d.\n", fileIdx, i);
				return i;
			}
		}
		printf("Target IDX %d was not found in the bundle\n", fileIdx);
		return -1;
	}

	int32_t getGPUFileInfoIdxFromFileIdx(const char* symbol) {
		int32_t fileIdx = getFileIdxFromSymbol(symbol);
		if (fileIdx == -1) return -1;

		return getGPUFileInfoIdxFromFileIdx(fileIdx);
	}

	int32_t getGPUFileInfoIdxFromFileIdx(int32_t fileIdx) {
		if (sectionTable.fileInfos == NULL) return -1;

		for (int32_t i = 0; i < header.numSections; i++) {
			if (sectionTable.fileInfos[i].asset == fileIdx + 1 && isGPUSection(sectionTable.fileInfos[i].sectionType - 1)) {
				printf("Target IDX %d was found in the bundle at index %d.\n", fileIdx, i);
				return i;
			}
		}
		printf("Target IDX %d was not found in the bundle\n", fileIdx);
		return -1;
	}

	int32_t doesFileExist(const char* fileName) {
		if (sectionTable.fileLabelTable.fileLabels == nullptr) {
			return 0;
		}

		for (int32_t i = 0; i < header.numAssets; i++) {
			char* symbol = sectionTable.fileLabelTable.fileLabels[i];

			if (strcmp(symbol, fileName) == 0) {
				return 1;
			}
		}
		return 0;
	}

	// Consts
	int32_t headerSize() const {
		return header.headerSize;
	}

	uint32_t bundleCRC() const {
		return header.headerHash;
	}

	int32_t numOfSymbols() const {
		return header.numAssets;
	}

	int32_t numOfFileParts() const {
		return header.numSections;
	}
};

struct BundleV31 {
public:
	CaffV31Header header;
	V31SectionEntry sectionEntries[8];
	V31FileTable fileInfoTable;

	V31FileType type;

	char* bundleData = nullptr;

	bool isReady = false;

	~BundleV31();

	bool readBundleFileV0031(char* data);

	void writeStandaloneBundleFile(char* fileName);

	void getArrayOfData();

	// Gets the offset of the target section by adding the size of the sections before it.
	// If using the section ID from "fileInfos", subtract by 1 before passing the value in.
	int32_t getOffsetOfSection(int32_t section) {
		int32_t offset = 0;
		for (int32_t i = 0; i < header.numSectionTypes; i++) {
			if (i == section) {
				break;
			}

			if (header.compression == 1) {
				offset = offset + sectionEntries[i].compressedSize;
				continue;
			}

			if (header.compression == 0) {
				offset = offset + sectionEntries[i].size;
				continue;
			}
		}
		return offset;
	}

	// Allocates and returns the target file data.
	char* getSectionData(int32_t section);

	// Allocates and returns the target file data.
	char* getFileData(char* fileName, int32_t fileInfoIdx);

	int32_t GetMatchingFileInfoIdx(int32_t fileIdx, int32_t section) {
		if (fileInfoTable.fileInfoEntries == nullptr) return -1;

		for (int32_t i = 0; i < header.numSections; i++) {
			if (fileInfoTable.fileInfoEntries[i].asset == fileIdx && fileInfoTable.fileInfoEntries[i].sectionType == section) {
				//printf("Target IDX %d was found in the bundle at index %d.\n", fileIdx, i);
				return i;
			}
		}
		//printf("Target IDX %d was not found in the bundle\n", fileIdx);
		return -1;
	}

	int32_t GetTotalSizeOfContainedFile(int32_t fileIdx) {
		if (fileInfoTable.fileInfoEntries == nullptr) return 0;

		int32_t size = 0;

		for (int32_t i = 0; i < header.numSections; i++) {
			if (fileInfoTable.fileInfoEntries[i].asset == fileIdx) {
				size += fileInfoTable.fileInfoEntries[i].size;
			}
		}
		//printf("Target IDX %d was not found in the bundle\n", fileIdx);
		return size;
	}
};

struct BundleV26 {
public:
	CaffV26Header header;
	V26SectionEntry sectionEntries[8];
	V31FileTable fileInfoTable;

	V31FileType type;

	char* bundleData = nullptr;

	bool isReady = false;

	~BundleV26();

	bool readBundleFileV0026(char* data);

	void writeStandaloneBundleFile(char* fileName);

	void getArrayOfData();

	// Gets the offset of the target section by adding the size of the sections before it.
	// If using the section ID from "fileInfos", subtract by 1 before passing the value in.
	int32_t getOffsetOfSection(int32_t section) {
		int32_t offset = 0;
		for (int32_t i = 0; i < header.numSectionTypes; i++) {
			if (i == section) {
				break;
			}

			if (header.compression == 1) {
				offset = offset + sectionEntries[i].compressedSize;
				continue;
			}

			if (header.compression == 0) {
				offset = offset + sectionEntries[i].size;
				continue;
			}
		}
		return offset;
	}

	// Allocates and returns the target file data.
	char* getSectionData(int32_t section);

	// Allocates and returns the target file data.
	char* getFileData(char* fileName, int32_t fileInfoIdx);

	int32_t GetMatchingFileInfoIdx(int32_t fileIdx, int32_t section) {
		if (fileInfoTable.fileInfoEntries == nullptr) return -1;

		for (int32_t i = 0; i < header.numOfFileEntries; i++) {
			if (fileInfoTable.fileInfoEntries[i].asset == fileIdx && fileInfoTable.fileInfoEntries[i].sectionType == section) {
				//printf("Target IDX %d was found in the bundle at index %d.\n", fileIdx, i);
				return i;
			}
		}
		//printf("Target IDX %d was not found in the bundle\n", fileIdx);
		return -1;
	}
};

struct BundleFile {
public:
	BundleV40* V40Bundle = nullptr;
	BundleV36* V36Bundle = nullptr;
	BundleV31* V31Bundle = nullptr;
	BundleV26* V26Bundle = nullptr;

	bool ReadBundleFile(char* data);
	void ClearActiveBundleFile();

	const BundleV40* GetV40BundlePtr() { return V40Bundle; }
	const BundleV36* GetV36BundlePtr() { return V36Bundle; }
	const BundleV31* GetV31BundlePtr() { return V31Bundle; }
	const BundleV26* GetV26BundlePtr() { return V26Bundle; }
};
#endif