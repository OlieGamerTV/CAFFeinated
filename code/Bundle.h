#pragma once
#include <vector>

enum CaffType : int {
	NONE = 0,
	GHOUL_BUNDLE,
	GHOUL_DEMAND,
	BUNDLEV26,
	BUNDLEV31,
	PINATA_DBBUNDLE,
	BUNDLEV36,
	NB_STREAMBUNDLE,
	RR_RPK,
};

struct FileInfoEntry {
	int ID = 0;
	int dataOffset = 0;
	int dataSize = 0;
	char section = 0;
	char unk1 = 0;
};

struct SectionEntry {
public:
	int strOffset = 0;
	char unk1 = 0;
	int unk2 = 0;
	int uncompressedSize = 0;
	// presumably a 0x10 padding layer
	int compressedSize = 0;
};

struct TableInfo {
public:
	int size = 0;
	unsigned int base = 0;
	int overlap = 0;
	int loadSize = 0;
	int compressedSize = 0;
};

struct SectionLabel {
public:
	char label[16];
};

struct FileLabel {
	FileLabel() {
		memset(label, 0, 128);
	}

	~FileLabel() {
		memset(label, 0, 128);
	}

public:
	char label[128];
};

struct FileLabelTable {
	int totalLabelTableSize = 0;
	FileLabel* fileLabels;
};

struct SectionTable {
public:
	SectionEntry entries[6];
	SectionLabel sectionLabels[6];
	int* fileLabelOffsets = nullptr;
	FileLabelTable fileLabelTable;

	int adbStringLen = 0;
	char* adbString = nullptr;

	FileInfoEntry* fileInfos = nullptr;

	int sectionLabelOffset = 0;
	int sectionFileLabelOffset = 0;
	int fileInfosOffset = 0;

	~SectionTable();
};

struct FileToSectInfo {
	int fileInfoID[1024];
	int fileLabelID[1024];
};

struct V31DebugNameTable {
public:
	int sizeOfStringTable;
	int* stringTableOffsets;
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
	int uncompressedSize;
	int unk1;
	char potentialPadding[0x10];
	int compressedSize;
};

struct CaffV26Header : CaffHeader {
public:
	int numOfFiles; // 0x14
	int numOfFileEntries; // 0x18
	int unk3; // 0x1C
	int unk4; // 0x20
	int unk5; // 0x24
	int unk6; // 0x28
	int fileInfoTableOffset; // 0x2C
	int headerSize; // 0x30

	char unk8 = 0; // 0x34
	char numSectionTypes = 0; // 0x35
	char compression = 0; // 0x36
	char unk9 = 0; // 0x37
};

enum V31FileType : int {
	V31_Unknown,
	V31_Texture,
	V31_ConkerText,
	V31_Text,
	V31_PDZPackage,
	V31_KameoDB
};

struct V31SectionEntry {
	char sectionName[8];
	int unk1;
	int unk2;
	int uncompressedSize;
	char potentialPadding[0x10];
	int compressedSize;
};

struct CaffV31Header : CaffHeader {
public:
	unsigned int headerHash = 0; // 0x14
	int numOfFiles; // 0x18
	int numOfFileEntries; // 0x1C
	int unk3; // 0x20
	int unk4; // 0x24
	int unk5; // 0x28
	int unk6; // 0x2C
	int unk7; // 0x30
	int fileInfoTableOffset; // 0x34
	int headerSize; // 0x38

	char byteswapFlags = 0; // 0x3C
	char numSectionTypes = 0; // 0x3D
	char compression = 0; // 0x3E
	char isDebug = 0; // 0x3F
};

struct CaffV36Header : CaffHeader {
public:
	int headerSize = 0x78; // 0x14
	unsigned int headerHash = 0; // 0x18
	int numAssets = 0; // 0x1C
	int numSections = 0; // 0x20
	int numByteswapGroups = 0; // 0x24
	int numByteswaps = 0; // 0x28
	int numRelocationGroups = 0; // 0x2C
	int numRelocations = 0; // 0x30
	int numMappedByteswapGroups = 0; // 0x34
	int numMappedByteswaps = 0; // 0x38
	int numMappedRelocationGroups = 0; // 0x3C
	int numMappedRelocations = 0; // 0x40
	int numPoolItems = 0; // 0x44
	char byteswapFlags = 0; // 0x48
	char numSectionTypes = 0; // 0x49
	char compression = 0; // 0x4A
	char numPools = 0; // 0x4B
	int sectionTypeNamesBufferLen = 0; // 0x4C, the length of the symbol string table in bytes.

	TableInfo sectTable;
	TableInfo fileTable;

	int sectionTableUncompedSize = 0;
	int sectionTableCompedSize = 0; // If "isCompressed" is 0, this will be equal to "sectionTableUncompedSize".

	int fileTableUncompedSize = 0;
	int fileTableCompedSize = 0; // If "isCompressed" is 0, this will be equal to "fileTableUncompedSize".
};

struct BundleV36 {
public:
	CaffV36Header header;
	SectionTable sectionTable;

	unsigned int* hashTable = nullptr;

	char* bundleData = nullptr;

	bool isReady = false;

	~BundleV36();

	bool readStandaloneBundleFile(char* fileName);
	bool readBundleFileV0036(char* data);

	bool readBundleHeaderV0036(char* data);
	bool readBundleSectionFileV0036(char* data);

	void writeStandaloneBundleFile(char* fileName);

	void getArrayOfData();

	// Allocates and returns the target file data.
	char* getFileData(char* fileName, int fileInfoIdx);

	bool isTextureGpuSection(int sectId) {
		if (strcmp(sectionTable.sectionLabels[sectId].label, ".texturegpu") == 0) {
			return true;
		}
		return false;
	}

	bool isGPUSection(int sectId) {
		if (strcmp(sectionTable.sectionLabels[sectId].label, ".gpu") == 0) {
			return true;
		}
		if (strcmp(sectionTable.sectionLabels[sectId].label, ".texturegpu") == 0) {
			return true;
		}
		return false;
	}

	bool isGPUCachedSection(int sectId) {
		if (strcmp(sectionTable.sectionLabels[sectId].label, ".gpucached") == 0) {
			return true;
		}
		return false;
	}

	bool isStreamSection(int sectId) {
		if (strcmp(sectionTable.sectionLabels[sectId].label, ".stream") == 0) {
			return true;
		}
		return false;
	}

	// Gets the offset of the target section by adding the size of the sections before it.
	// If using the section ID from "fileInfos", subtract by 1 before passing the value in.
	int getOffsetOfSection(int section) {
		int offset = 0;
		for (int i = 0; i < header.numSectionTypes; i++) {
			if (i == section) {
				break;
			}
			if (header.compression) {
				offset = offset + sectionTable.entries[i].compressedSize;
			}
			else {
				offset = offset + sectionTable.entries[i].uncompressedSize;
			}
		}
		return offset;
	}

	size_t getBaseSizeOfCompedBundle() const {
		return header.headerSize + header.fileTableCompedSize + header.sectionTableCompedSize;
	}

	size_t getTotalSizeOfCompedBundle() const {
		size_t totalSize = header.headerSize + header.fileTableCompedSize + header.sectionTableCompedSize;

		for (int i = 0; i < header.numSectionTypes; i++) {
			totalSize += sectionTable.entries[i].compressedSize;
		}

		return totalSize;
	}

	size_t getDefaultSizeofSection(int section) {
		for (int i = 0; i < header.numSectionTypes; i++) {
			if (i == section) {
				if (header.compression) {
					return sectionTable.entries[i].compressedSize;
				}
				else {
					return sectionTable.entries[i].uncompressedSize;
				}
			}
		}
		return 0;
	}

	size_t getUncompressedSizeofSection(int section) {
		for (int i = 0; i < header.numSectionTypes; i++) {
			if (i == section) {
				return sectionTable.entries[i].uncompressedSize;
			}
		}
		return 0;
	}

	size_t getCompressedSizeofSection(int section) {
		for (int i = 0; i < header.numSectionTypes; i++) {
			if (i == section) {
				return sectionTable.entries[i].compressedSize;
			}
		}
		return 0;
	}


	int getFileIdxFromSymbol(const char* symbol) {
		if (sectionTable.fileLabelTable.fileLabels == nullptr) {
			printf("fileLabels is null.\n");
			return -1;
		}


		for (int i = 0; i < header.numAssets; i++) {
			char* label = sectionTable.fileLabelTable.fileLabels[i].label;

			if (strcmp(label, symbol) == 0) {
				printf("Target symbol %s was found in the bundle at index %d.\n", symbol, i);
				return i;
			}
		}
		printf("Target symbol %s was not found in the bundle\n", symbol);
		return -1;
	}

	int GetMatchingFileInfoIdx(int fileIdx, int section) {
		if (sectionTable.fileInfos == NULL) return -1;

		for (int i = 0; i < header.numSections; i++) {
			if (sectionTable.fileInfos[i].ID == fileIdx && sectionTable.fileInfos[i].section == section) {
				//printf("Target IDX %d was found in the bundle at index %d.\n", fileIdx, i);
				return i;
			}
		}
		//printf("Target IDX %d was not found in the bundle\n", fileIdx);
		return -1;
	}

	int getFileInfoIdxFromFileIdx(const char* symbol, int sectionIdx) {
		int fileIdx = getFileIdxFromSymbol(symbol);
		if (fileIdx == -1) return -1;

		return getFileInfoIdxFromFileIdx(fileIdx, sectionIdx);
	}

	int getFileInfoIdxFromFileIdx(int fileIdx, int sectionIdx) {
		if (sectionTable.fileInfos == NULL) return -1;

		for (int i = 0; i < header.numSections; i++) {
			if (sectionTable.fileInfos[i].ID == fileIdx + 1 && sectionTable.fileInfos[i].section == sectionIdx + 1) {
				printf("Target IDX %d was found in the bundle at index %d.\n", fileIdx, i);
				return i;
			}
		}
		printf("Target IDX %d was not found in the bundle\n", fileIdx);
		return -1;
	}

	int getGPUFileInfoIdxFromFileIdx(const char* symbol) {
		int fileIdx = getFileIdxFromSymbol(symbol);
		if (fileIdx == -1) return -1;

		return getGPUFileInfoIdxFromFileIdx(fileIdx);
	}

	int getGPUFileInfoIdxFromFileIdx(int fileIdx) {
		if (sectionTable.fileInfos == NULL) return -1;

		for (int i = 0; i < header.numSections; i++) {
			if (sectionTable.fileInfos[i].ID == fileIdx + 1 && isGPUSection(sectionTable.fileInfos[i].section - 1)) {
				printf("Target IDX %d was found in the bundle at index %d.\n", fileIdx, i);
				return i;
			}
		}
		printf("Target IDX %d was not found in the bundle\n", fileIdx);
		return -1;
	}

	int doesFileExist(const char* fileName) {
		if (sectionTable.fileLabelTable.fileLabels == nullptr) {
			return 0;
		}

		for (int i = 0; i < header.numAssets; i++) {
			char* symbol = sectionTable.fileLabelTable.fileLabels[i].label;

			if (strcmp(symbol, fileName) == 0) {
				return 1;
			}
		}
		return 0;
	}

	// Consts
	int headerSize() const {
		return header.headerSize;
	}

	unsigned int bundleCRC() const {
		return header.headerHash;
	}

	int numOfSymbols() const {
		return header.numAssets;
	}

	int numOfFileParts() const {
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
	int getOffsetOfSection(int section) {
		int offset = 0;
		for (int i = 0; i < header.numSectionTypes; i++) {
			if (i == section) {
				break;
			}

			if (header.compression == 1) {
				offset = offset + sectionEntries[i].compressedSize;
				continue;
			}

			if (header.compression == 0) {
				offset = offset + sectionEntries[i].uncompressedSize;
				continue;
			}
		}
		return offset;
	}

	// Allocates and returns the target file data.
	char* getSectionData(int section);

	// Allocates and returns the target file data.
	char* getFileData(char* fileName, int fileInfoIdx);

	int GetMatchingFileInfoIdx(int fileIdx, int section) {
		if (fileInfoTable.fileInfoEntries == nullptr) return -1;

		for (int i = 0; i < header.numOfFileEntries; i++) {
			if (fileInfoTable.fileInfoEntries[i].ID == fileIdx && fileInfoTable.fileInfoEntries[i].section == section) {
				//printf("Target IDX %d was found in the bundle at index %d.\n", fileIdx, i);
				return i;
			}
		}
		//printf("Target IDX %d was not found in the bundle\n", fileIdx);
		return -1;
	}

	int GetTotalSizeOfContainedFile(int fileIdx) {
		if (fileInfoTable.fileInfoEntries == nullptr) return 0;

		int size = 0;

		for (int i = 0; i < header.numOfFileEntries; i++) {
			if (fileInfoTable.fileInfoEntries[i].ID == fileIdx) {
				size += fileInfoTable.fileInfoEntries[i].dataSize;
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
	int getOffsetOfSection(int section) {
		int offset = 0;
		for (int i = 0; i < header.numSectionTypes; i++) {
			if (i == section) {
				break;
			}

			if (header.compression == 1) {
				offset = offset + sectionEntries[i].compressedSize;
				continue;
			}

			if (header.compression == 0) {
				offset = offset + sectionEntries[i].uncompressedSize;
				continue;
			}
		}
		return offset;
	}

	// Allocates and returns the target file data.
	char* getSectionData(int section);

	// Allocates and returns the target file data.
	char* getFileData(char* fileName, int fileInfoIdx);

	int GetMatchingFileInfoIdx(int fileIdx, int section) {
		if (fileInfoTable.fileInfoEntries == nullptr) return -1;

		for (int i = 0; i < header.numOfFileEntries; i++) {
			if (fileInfoTable.fileInfoEntries[i].ID == fileIdx && fileInfoTable.fileInfoEntries[i].section == section) {
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
	BundleV36* V36Bundle = nullptr;
	BundleV31* V31Bundle = nullptr;
	BundleV26* V26Bundle = nullptr;

	bool ReadBundleFile(char* data);
	void ClearActiveBundleFile();

	const BundleV36* GetV36BundlePtr() { return V36Bundle; }
	const BundleV31* GetV31BundlePtr() { return V31Bundle; }
	const BundleV26* GetV26BundlePtr() { return V26Bundle; }
};