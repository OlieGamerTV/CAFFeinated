#pragma once
#include "BaseScript.h"

#ifdef _WIN32 
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <stdlib.h>

const unsigned int VEHICLE_HEAD_GAMEID = 0xED07534D;
const unsigned int VEHICLE_SAVE_PREFIX1 = 0x48E19A3F;
const unsigned int VEHICLE_SAVE_PREFIX2 = 0xE17A5440;

const unsigned int LOCTEXT_LSBTWO_MAGIC = 0x3242534C;
const unsigned int LOCTEXT_LBSTWO_MAGIC = 0x3253424C;
const char LOCTEXT_LSBL[4] = { 'L', 'S', 'B', 'L' };
const char LOCTEXT_LBSL[4] = { 'L', 'B', 'S', 'L' };
const unsigned int LOCTEXT_LSBL_MAGIC = 0x4C53424C;
const unsigned int LOCTEXT_LBSL_MAGIC = 0x4C42534C;

#pragma region Script
struct Script {
	char* scriptPtr;

	int entryCount = 0; // Not actually part of the file. Calculated on read.
	dbScript_Base* entries;

	void ReadScript(char* data);

	char* WriteToArray();
	void WriteToFile(char* fileName, bool isSave);
};
#pragma endregion


enum AssetType : char {

};

struct AidEntry {
	unsigned int hash;
	int id;
};

#pragma region Loctext Rev One
struct LabelTableHeader {
	int totalSectLen = 0;
	int totalCount = 0;
};

struct CommentEntry {
	char unk1 = 0;
	unsigned short id = 0;
	char unk2 = 0;
	int offset = 0;
};

struct CommentStr {
	char val[8192];
};

struct CommentTable {
	LabelTableHeader header;
	CommentEntry* entries;
	CommentStr* comments;
};

struct PosTable {
	LabelTableHeader header;
	unsigned short* entries;
};

struct LabelStrInfoEntry {
	unsigned short unk = 0;
	unsigned short hash = 0;
	int offset = 0;
};

struct LabelStrEntry {
	wchar_t string[2048];
};

struct LabelStrTable {
	LabelTableHeader header;
	LabelStrInfoEntry* infoEntries;
	LabelStrEntry* strings;
};

struct TagStr {
	char val[256];
};

struct TagInfo {
	unsigned short id = 0;
	short unk1 = 0;
	int offset = 0;
};

struct LabelTagTable {
	LabelTableHeader header;
	TagInfo* infoEntries;
	TagStr* tags;
};

struct LabelHeader {
public:
	char magic[4]; // 0x0
	int headerLen = 0; // 0x4
	int entryTotal = 0; // 0x8
	int stringTableOffset = 0; // 0xC
	int tagTableOffset = 0; // 0x10
	int commentTableOffset = 0; // 0x14
	int positionTableOffset = 0; // 0x18
};

struct LabelTable {
public:
	LabelHeader header;
	LabelStrTable stringTable;
	LabelTagTable tagTable;
	CommentTable commentTable;
	PosTable posTable;
};

struct UnkEntry {
	int tagoffset = 0;
	unsigned short id = 0;
	short idx = 0;
};

struct UnkTable {
	UnkEntry* unkEntries;
	TagStr* unkTags;
};

struct Loctext {
public:
	char* loctextPtr;

	int endianness;
	int startEndianness = SRC_ENDIANBIG;

	bool usesTags = false;
	bool usesComments = false;
	bool usesPos = false;

	int labelDataOffset = 0;
	int unkTableOffset = 0;
	short unkTableCount = 0;
	short unk1 = 0;

	LabelTable labelTable;
	UnkTable unknownTable;

	void ReadLoctext(char* data);

	void ReadLabelData();
	void ReadTagData();
	void ReadCommentData();
	void ReadPosData();

	void ExportToFileRaw(char* fileName);
	void ExportToFileBank(char* fileName, int endianness);

	void WriteLoctext(char* filename);

	int GetIdxOfConnectedString(unsigned short id) {
		for (int i = 0; i < labelTable.stringTable.header.totalCount; i++) {
			if (labelTable.stringTable.infoEntries[i].hash == id) return i;
		}

		return -1;
	}

	int GetIdxOfConnectedTag(unsigned short id) {
		for (int i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			if (labelTable.tagTable.infoEntries[i].id == id) return i;
		}

		return -1;
	}

	int GetIdxOfConnectedComment(unsigned short id) {
		if (labelTable.header.commentTableOffset == 0) return -2;

		for (int i = 0; i < labelTable.commentTable.header.totalCount; i++) {
			if (labelTable.commentTable.entries[i].id == id) return i;
		}

		return -2;
	}

	bool IsHashConnectedToTag(unsigned short id) {
		for (int i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			if (labelTable.tagTable.infoEntries[i].id == id) return true;
		}

		return false;
	}

	bool IsIdxConnectedToComment(unsigned short id) {
		if (labelTable.header.commentTableOffset == 0) return false;

		for (int i = 0; i < labelTable.commentTable.header.totalCount; i++) {
			if (labelTable.commentTable.entries[i].id == id) return true;
		}

		return false;
	}
};
#pragma endregion

#pragma region Loctext Rev Two
struct LabTwoHeader {
public:
	unsigned int magic; // 0x0
	int unk1; // 0x4
	int unk2; // 0x8
	int headerLen; // 0xC
	int entryTotal; // 0x10
	int stringTableOffset; // 0x14
	int tagTableOffset; // 0x18
	int commentTableOffset; // 0x1C
	int positionTableOffset; // 0x20
};

struct LabTwoTable {
public:
	LabTwoHeader header;
	LabelStrTable stringTable;
	LabelTagTable tagTable;
	CommentTable commentTable;
	PosTable posTable;
};

struct LocTwo {
public:
	char* loctextPtr;

	int labelDataOffset = 0;
	int unkTableOffset = 0;
	short unkTableCount = 0;
	short unk1 = 0;

	LabTwoTable labelTable;
	UnkTable unknownTable;

	void ReadLoctext(char* data);

	void ReadLabelData();
	void ReadTagData();
	void ReadCommentData();
	void ReadPosData();

	void ExportToFile(char* fileName);

	int GetIdxOfConnectedString(unsigned short id) {
		for (int i = 0; i < labelTable.stringTable.header.totalCount; i++) {
			if (labelTable.stringTable.infoEntries[i].hash == id) return i;
		}

		return -1;
	}

	int GetIdxOfConnectedTag(unsigned short id) {
		for (int i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			if (labelTable.tagTable.infoEntries[i].id == id) return i;
		}

		return -1;
	}

	int GetIdxOfConnectedComment(unsigned short id) {
		if (labelTable.header.commentTableOffset == 0) return -2;

		for (int i = 0; i < labelTable.commentTable.header.totalCount; i++) {
			if (labelTable.commentTable.entries[i].id == id) return i;
		}

		return -2;
	}

	bool IsIdxConnectedToTag(unsigned short id) {
		for (int i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			if (labelTable.tagTable.infoEntries[i].id == id) return true;
		}

		return false;
	}

	bool IsIdxConnectedToComment(unsigned short id) {
		if (labelTable.header.commentTableOffset == 0) return false;

		for (int i = 0; i < labelTable.commentTable.header.totalCount; i++) {
			if (labelTable.commentTable.entries[i].id == id) return true;
		}

		return false;
	}
};
#pragma endregion

struct VehiclePart {
public:
	char xPos = 0; // 0x0
	char yPos = 0; // 0x1
	char zPos = 0; // 0x2
	char isChallengePart = 0; // 0x3
	char isPainted = 0; // 0x4
	char unk1 = 0; // 0x5
	char unk2 = 0; // 0x6
	char unk3 = 0; // 0x7
	unsigned int partIdx = 0; // 0x8

	float yaw = 0; // 0xC
	float pitch = 0; // 0x10
	float roll = 0; // 0x14

	unsigned int color = 0xFFFFFFFF; // 0x18

	//unsigned char colR = 0xFF; // 0x18
	//unsigned char colG = 0xFF; // 0x19
	//unsigned char colB = 0xFF; // 0x1A
	//unsigned char colA = 0xFF; // 0x1B

	int unk4 = 0; // 0x1C
	int unk5 = 0; // 0x20
};

struct Vehicle {
public:
	char* vehiclePtr;

	bool isSaveVehicle = false;

	unsigned short numOfParts = 0; // 0x0
	short unk1 = 0; // 0x2
	float preloadPower = 0; // 0x4
	float preloadFuel = 0; // 0x8
	float preloadAmmo = 0; // 0xC
	float preloadWeight = 0; // 0x10
	float preloadBlocksUsed = 0; // 0x14
	int unk2 = 0; // 0x18
	int unk3 = 0; // 0x1C

	// A buffer of 0x40 bytes is reserved for this.
	// A total of 64 characters will be in this.
	char vehicleName[0x40];

	// A buffer of 0x40 bytes is reserved for this.
	// A total of 32 characters will be in this.
	wchar_t vehicleUnicodeName[0x20];

	unsigned int aButtonAssignment = 0; // 0x60
	unsigned int bButtonAssignment = 0; // 0x64
	unsigned int xButtonAssignment = 0; // 0x68

	int unk4 = 0; // 0x6C
	int unk5 = 0; // 0x70
	int unk6 = 0; // 0x74
	char nameType = 0; // 0x78
	char unk7 = 0; // 0x79
	char unk8 = 0; // 0x7A
	char unk9 = 0; // 0x7B

	VehiclePart* parts = nullptr;

	void ReadVehicle(char* data, bool isFromSave);

	char* WriteToArray();
	void WriteToFile(char* fileName, bool isSave);
	void WriteHeaderFile(char* fileName);

	bool IsPositionTaken(char x, char y, char z) {
		for (int i = 0; i < numOfParts; i++) {
			if (parts[i].xPos == x && parts[i].yPos == y && parts[i].zPos == z) return true;
		}

		return false;
	}

	void AddEntry(VehiclePart part) {
		numOfParts++;

		parts = (VehiclePart*)realloc(parts, (sizeof(VehiclePart) * numOfParts));
		parts[numOfParts - 1] = part;
	}

	void RemoveEntry(int idx) {
		VehiclePart* tmpParts = (VehiclePart*)malloc((sizeof(VehiclePart) * numOfParts) - sizeof(VehiclePart));
		int index = 0;
		for (int i = 0; i < numOfParts; i++) {
			if (i == idx) continue;
			tmpParts[index] = parts[i];
			index++;
		}
		free(parts);

		numOfParts--;
		parts = (VehiclePart*)malloc(sizeof(VehiclePart) * numOfParts);

		for (int i = 0; i < numOfParts; i++) {
			parts[i] = tmpParts[i];
		}

		free(tmpParts);
	}
};

struct Manifest {
	char* manifestPtr = nullptr;

	unsigned int magic = 0;
	unsigned int timestamp = 0;

	// Tables
	int aidTableOffset = 0;
	int aidTableCount = 0;

	int referenceTableOffset = 0;
	int referenceTableCount = 0;

	int xcueRefTableOffset = 0;
	int xcueRefTableCount = 0;

	AidEntry* aidTable = nullptr;
	int* referenceTable = nullptr;
	int* xcueRefTable = nullptr;

	void ReadManifest(char* data);

	unsigned int GetAidHash(int id) {
		if (aidTable == NULL) return 0;

		for (int i = 0; i < aidTableCount; i++) {
			if (aidTable[i].id == id) return aidTable[i].hash;
		}
		return 0;
	}

	~Manifest();
};

struct TextureDataSect {
public:
	char magic[8]; // The identifier of the file.
	char version[0x10]; // The version of the file.

	// Texture Info
	char unk_0x18 = 0;
	char unk_0x19 = 0;
	char isSwizzled = 0; // 0x1A
	unsigned char textureType = 0; // 0x1B

	int unk_0x1C = 0;
	int unk_0x20 = 0;
	short width = 0;
	short height = 0;
	int frameCount = 1; // 0x38 (For textures that only have one frame, this is set to 0.)
	int gpuOffsTablePos = 0;
	int* gpuOffsTable;
};

struct Texture {
public:
	TextureDataSect headerSect;
	char* textureHeaderPtr;
	bool refresh = true;

	unsigned char* textureDataPtr;

	void ReadTextureInfo(char* data);

	void SetTextureHeaderPtr(char* ptr) {
		textureHeaderPtr = ptr;
	}

	void SetTextureDataPtr(unsigned char* ptr) {
		textureDataPtr = ptr;
	}
};