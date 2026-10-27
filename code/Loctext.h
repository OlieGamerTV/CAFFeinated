#pragma once
#ifndef _LOCTEXT
#define  _LOCTEXT
#include <stdint.h>
#include "ReaderUtils.h"

const uint32_t LOCTEXT_LSBTWO_MAGIC = 0x3242534C;
const uint32_t LOCTEXT_LBSTWO_MAGIC = 0x3253424C;
const char LOCTEXT_LSBL[5] = { 'L', 'S', 'B', 'L', '\0'};
const char LOCTEXT_LBSL[5] = { 'L', 'B', 'S', 'L', '\0' };
const char LOCTEXT_LSB2[5] = { 'L', 'S', 'B', '2', '\0' };
const char LOCTEXT_LBS2[5] = { 'L', 'B', 'S', '2', '\0' };
const uint32_t LOCTEXT_LSBL_MAGIC = 0x4C53424C;
const uint32_t LOCTEXT_LBSL_MAGIC = 0x4C42534C;

#pragma region Loctext Rev One
struct LabelTableHeader {
	int32_t totalSectLen = 0;
	int32_t totalCount = 0;
};

struct CommentEntry {
	char unk1 = 0;
	uint16_t id = 0;
	char unk2 = 0;
	int32_t offset = 0;
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
	uint16_t* entries;
};

struct LabelStrInfoEntry {
	uint16_t hash = 0;
	int32_t offset = 0;
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
	uint16_t id = 0;
	int16_t unk1 = 0;
	int32_t offset = 0;
};

struct LabelTagTable {
	LabelTableHeader header;
	TagInfo* infoEntries;
	TagStr* tags;
};

struct LabelHeader {
public:
	char magic[4]; // 0x0
	int32_t headerLen = 0; // 0x4
	int32_t entryTotal = 0; // 0x8
	int32_t stringTableOffset = 0; // 0xC
	int32_t tagTableOffset = 0; // 0x10
	int32_t commentTableOffset = 0; // 0x14
	int32_t positionTableOffset = 0; // 0x18
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
	int32_t tagoffset = 0;
	uint16_t id = 0;
	int16_t idx = 0;
};

struct UnkTable {
	UnkEntry* unkEntries;
	TagStr* unkTags;
};

struct Loctext {
public:
	char* loctextPtr;

	int32_t endianness;
	int32_t startEndianness = SRC_ENDIANBIG;

	bool usesTags = false;
	bool usesComments = false;
	bool usesPos = false;

	int32_t labelDataOffset = 0;
	int32_t unkTableOffset = 0;
	int16_t unkTableCount = 0;
	int16_t unk1 = 0;

	LabelTable labelTable;
	UnkTable unknownTable;

	void ReadLoctext(char* data);

	void ReadLabelData();
	void ReadTagData();
	void ReadCommentData();
	void ReadPosData();

	void ExportToFileRaw(char* fileName);
	void ExportToFileBank(char* fileName, int32_t endianness);

	void WriteLoctext(char* filename);
	
	void ClearActiveData();

	int32_t GetIdxOfConnectedString(uint16_t id) {
		for (int32_t i = 0; i < labelTable.stringTable.header.totalCount; i++) {
			if (labelTable.stringTable.infoEntries[i].hash == id) return i;
		}

		return -1;
	}

	int32_t GetIdxOfConnectedTag(uint16_t id) {
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			if (labelTable.tagTable.infoEntries[i].id == id) return i;
		}

		return -1;
	}

	int32_t GetIdxOfConnectedComment(uint16_t id) {
		if (!usesComments) return -2;

		for (int32_t i = 0; i < labelTable.commentTable.header.totalCount; i++) {
			if (labelTable.commentTable.entries[i].id == id) return i;
		}

		return -2;
	}

	bool IsHashConnectedToTag(uint16_t id) {
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			if (labelTable.tagTable.infoEntries[i].id == id) return true;
		}

		return false;
	}

	bool IsIdxConnectedToComment(uint16_t id) {
		if (!usesComments) return false;

		for (int32_t i = 0; i < labelTable.commentTable.header.totalCount; i++) {
			if (labelTable.commentTable.entries[i].id == id) return true;
		}

		return false;
	}
};
#pragma endregion

#pragma region Loctext Rev Two
struct LabTwoHeader {
public:
	char magic[4]; // 0x0
	int32_t unk1; // 0x4
	int32_t unk2; // 0x8
	int32_t headerLen; // 0xC
	int32_t entryTotal; // 0x10
	int32_t stringTableOffset; // 0x14
	int32_t tagTableOffset; // 0x18
	int32_t commentTableOffset; // 0x1C
	int32_t positionTableOffset; // 0x20
};

struct LabelTwoStrInfoEntry {
	uint16_t unk = 0;
	uint16_t hash = 0;
	int32_t offset = 0;
};

struct LabelTwoStrTable {
	LabelTableHeader header;
	LabelTwoStrInfoEntry* infoEntries;
	LabelStrEntry* strings;
};

struct LabTwoTable {
public:
	LabTwoHeader header;
	LabelTwoStrTable stringTable;
	LabelTagTable tagTable;
	CommentTable commentTable;
	PosTable posTable;
};

struct LocTwo {
public:
	char* loctextPtr;

	int32_t labelDataOffset = 0;
	int32_t unkTableOffset = 0;
	int16_t unkTableCount = 0;
	int16_t unk1 = 0;

	LabTwoTable labelTable;
	UnkTable unknownTable;

	int32_t endianness;
	int32_t startEndianness = SRC_ENDIANBIG;

	bool usesTags = false;
	bool usesComments = false;
	bool usesPos = false;

	void ReadLoctext(char* data);

	void ReadLabelData();
	void ReadTagData();
	void ReadCommentData();
	void ReadPosData();

	void ExportToFile(char* fileName);

	int32_t GetIdxOfConnectedString(uint16_t id) {
		for (int32_t i = 0; i < labelTable.stringTable.header.totalCount; i++) {
			if (labelTable.stringTable.infoEntries[i].hash == id) return i;
		}

		return -1;
	}

	int32_t GetIdxOfConnectedTag(uint16_t id) {
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			if (labelTable.tagTable.infoEntries[i].id == id) return i;
		}

		return -1;
	}

	int32_t GetIdxOfConnectedComment(uint16_t id) {
		if (labelTable.header.commentTableOffset == 0) return -2;

		for (int32_t i = 0; i < labelTable.commentTable.header.totalCount; i++) {
			if (labelTable.commentTable.entries[i].id == id) return i;
		}

		return -2;
	}

	bool IsIdxConnectedToTag(uint16_t id) {
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			if (labelTable.tagTable.infoEntries[i].id == id) return true;
		}

		return false;
	}

	bool IsHashConnectedToTag(uint16_t id) {
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			if (labelTable.tagTable.infoEntries[i].id == id) return true;
		}

		return false;
	}

	bool IsIdxConnectedToComment(uint16_t id) {
		if (labelTable.header.commentTableOffset == 0) return false;

		for (int32_t i = 0; i < labelTable.commentTable.header.totalCount; i++) {
			if (labelTable.commentTable.entries[i].id == id) return true;
		}

		return false;
	}
};
#pragma endregion

#pragma region Loctext Rev Turn 10 V3
struct LabTurn10V3Header {
public:
	char unk1; // 0x0
	char version; // 0x1
	char numSects; // 0x2, should be 2.
	char unk2; // 0x3
	int32_t stringTableOffset; // 0x4
	int32_t tagTableOffset; // 0x8
};

struct LabelTurn10TableHeader {
	int32_t totalSectLen = 0;
	int32_t totalStringSectLen = 0;
	int32_t totalCount = 0;
};

struct LabelTurn10StrTable {
	LabelTurn10TableHeader header;
	LabelTwoStrInfoEntry* infoEntries;
	LabelStrEntry* strings;
};

struct TagTurn10Str {
	wchar_t val[256];
};

struct LabelTurn10TagTable {
	LabelTurn10TableHeader header;
	TagInfo* infoEntries;
	TagTurn10Str* tags;
};

struct LabTurn10V3Table {
public:
	LabTurn10V3Header header;
	LabelTurn10StrTable stringTable;
	LabelTurn10TagTable tagTable;
};

struct LocTurn10V3 {
public:
	char* loctextPtr;

	LabTurn10V3Table labelTable;

	int32_t endianness;
	int32_t startEndianness = SRC_ENDIANBIG;

	bool usesTags = false;

	void ReadLoctext(char* data);

	void ReadLabelData();
	void ReadTagData();

	void ExportToFile(char* fileName);

	int32_t GetIdxOfConnectedString(uint16_t id) {
		for (int32_t i = 0; i < labelTable.stringTable.header.totalCount; i++) {
			if (labelTable.stringTable.infoEntries[i].hash == id) return i;
		}

		return -1;
	}

	int32_t GetIdxOfConnectedTag(uint16_t id) {
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			if (labelTable.tagTable.infoEntries[i].id == id) return i;
		}

		return -1;
	}

	bool IsIdxConnectedToTag(uint16_t id) {
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			if (labelTable.tagTable.infoEntries[i].id == id) return true;
		}

		return false;
	}

	bool IsHashConnectedToTag(uint16_t id) {
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			if (labelTable.tagTable.infoEntries[i].id == id) return true;
		}

		return false;
	}
};
#pragma endregion

#pragma region Loctext Rev Turn 10 V4
struct LabTurn10V4Header {
public:
	char unk1; // 0x0
	char version; // 0x1
	char fileName[0x80]; // 0x2
	char numSects; // 0x82, should be 2.
	char unk2; // 0x83
	int32_t stringTableOffset; // 0x84
	int32_t tagTableOffset; // 0x88
};

struct LabTurn10V4Table {
public:
	LabTurn10V4Header header;
	LabelTurn10StrTable stringTable;
	LabelTurn10TagTable tagTable;
};

struct LocTurn10V4 {
public:
	char* loctextPtr;

	LabTurn10V4Table labelTable;

	int32_t endianness;
	int32_t startEndianness = SRC_ENDIANBIG;

	bool usesTags = false;

	void ReadLoctext(char* data);

	void ReadLabelData();
	void ReadTagData();

	void ExportToFile(char* fileName);

	int32_t GetIdxOfConnectedString(uint16_t id) {
		for (int32_t i = 0; i < labelTable.stringTable.header.totalCount; i++) {
			if (labelTable.stringTable.infoEntries[i].hash == id) return i;
		}

		return -1;
	}

	int32_t GetIdxOfConnectedTag(uint16_t id) {
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			if (labelTable.tagTable.infoEntries[i].id == id) return i;
		}

		return -1;
	}

	bool IsIdxConnectedToTag(uint16_t id) {
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			if (labelTable.tagTable.infoEntries[i].id == id) return true;
		}

		return false;
	}

	bool IsHashConnectedToTag(uint16_t id) {
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			if (labelTable.tagTable.infoEntries[i].id == id) return true;
		}

		return false;
	}
};
#pragma endregion

#pragma region Loctext Rev Turn 10 V8
struct LabelTurn10V8StrEntry {
	char string[2048];
};

struct LabelTurn10V8StrTable {
	LabelTurn10TableHeader header;
	LabelTwoStrInfoEntry* infoEntries;
	LabelTurn10V8StrEntry* strings;
};

struct LabelTurn10V8TagTable {
	LabelTurn10TableHeader header;
	TagInfo* infoEntries;
	TagStr* tags;
};

struct LabTurn10V8Table {
public:
	LabTurn10V4Header header;
	LabelTurn10V8StrTable stringTable;
	LabelTurn10V8TagTable tagTable;
};

struct LocTurn10V8 {
public:
	char* loctextPtr;

	LabTurn10V8Table labelTable;

	int32_t endianness;
	int32_t startEndianness = SRC_ENDIANBIG;

	bool usesTags = false;

	void ReadLoctext(char* data);

	void ReadLabelData();
	void ReadTagData();

	void ExportToFile(char* fileName);

	int32_t GetIdxOfConnectedString(uint16_t id) {
		for (int32_t i = 0; i < labelTable.stringTable.header.totalCount; i++) {
			if (labelTable.stringTable.infoEntries[i].hash == id) return i;
		}

		return -1;
	}

	int32_t GetIdxOfConnectedTag(uint16_t id) {
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			if (labelTable.tagTable.infoEntries[i].id == id) return i;
		}

		return -1;
	}

	bool IsIdxConnectedToTag(uint16_t id) {
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			if (labelTable.tagTable.infoEntries[i].id == id) return true;
		}

		return false;
	}

	bool IsHashConnectedToTag(uint16_t id) {
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			if (labelTable.tagTable.infoEntries[i].id == id) return true;
		}

		return false;
	}
};
#pragma endregion

enum CurrentLoctext : int32_t {
	None,
	Loc1,
	Loc2,
	LocT10V3,
	LocT10V4,
	LocT10V8
};

struct LoctextFile {
public:
	Loctext loc1File;
	LocTwo loc2File;
	LocTurn10V3 locTurn10V3File;
	LocTurn10V4 locTurn10V4File;
	LocTurn10V8 locTurn10V8File;

	CurrentLoctext currentlyLoadedLoctext = None;

	void ParseLoctextData(char* data);

	void ExportToFileRaw(char* fileName);
	
	~LoctextFile();

	int32_t GetEndianness() {
		if (currentlyLoadedLoctext == None) return 0;

		switch (currentlyLoadedLoctext) {
		case Loc1:
			return loc1File.endianness;
		case Loc2:
			return loc2File.endianness;
		case LocT10V3:
			return locTurn10V3File.endianness;
		case LocT10V4:
			return locTurn10V4File.endianness;
		case LocT10V8:
			return locTurn10V8File.endianness;
		}
	}

	void SetEndianness(int32_t end) {
		if (currentlyLoadedLoctext == None) return;

		switch (currentlyLoadedLoctext) {
		case Loc1:
			loc1File.endianness = end;
			break;
		case Loc2:
			loc2File.endianness = end;
			break;
		case LocT10V3:
			locTurn10V3File.endianness = end;
			break;
		case LocT10V4:
			locTurn10V4File.endianness = end;
			break;
		case LocT10V8:
			locTurn10V8File.endianness = end;
			break;
		}
	}

	bool GetIsUsingTags() {
		switch (currentlyLoadedLoctext) {
		case Loc1:
			return loc1File.usesTags;
		case Loc2:
			return loc2File.usesTags;
		case LocT10V3:
			return locTurn10V3File.usesTags;
		case LocT10V4:
			return locTurn10V4File.usesTags;
		case LocT10V8:
			return locTurn10V8File.usesTags;
		}
		
		return false;
	}

	void SetIsUsingTags(bool val) {
		if (currentlyLoadedLoctext == None) return;

		switch (currentlyLoadedLoctext) {
		case Loc1:
			loc1File.usesTags = val;
			break;
		case Loc2:
			loc2File.usesTags = val;
			break;
		}
	}

	bool GetIsUsingComments() {
		switch (currentlyLoadedLoctext) {
		case Loc1:
			return loc1File.usesComments;
		case Loc2:
			return loc2File.usesComments;
		}
		
		return false;
	}

	void SetIsUsingComments(bool val) {
		if (currentlyLoadedLoctext == None) return;

		switch (currentlyLoadedLoctext) {
		case Loc1:
			loc1File.usesComments = val;
			break;
		case Loc2:
			loc2File.usesComments = val;
			break;
		}
	}

	bool GetIsUsingPositions() {
		switch (currentlyLoadedLoctext) {
		case Loc1:
			return loc1File.usesPos;
		case Loc2:
			return loc2File.usesPos;
		}
		
		return false;
	}

	void SetIsUsingPositions(bool val) {
		if (currentlyLoadedLoctext == None) return;

		switch (currentlyLoadedLoctext) {
		case Loc1:
			loc1File.usesPos = val;
			break;
		case Loc2:
			loc2File.usesPos = val;
			break;
		}
	}

	int32_t GetStringCount() {
		switch (currentlyLoadedLoctext) {
		case Loc1:
			return loc1File.labelTable.stringTable.header.totalCount;
		case Loc2:
			return loc2File.labelTable.stringTable.header.totalCount;
		case LocT10V3:
			return locTurn10V3File.labelTable.stringTable.header.totalCount;
		case LocT10V4:
			return locTurn10V4File.labelTable.stringTable.header.totalCount;
		case LocT10V8:
			return locTurn10V8File.labelTable.stringTable.header.totalCount;
		}
		
		return -1;
	}

	int32_t GetIdxOfConnectedString(uint16_t id) {
		switch (currentlyLoadedLoctext) {
		case Loc1:
			return loc1File.GetIdxOfConnectedString(id);
		case Loc2:
			return loc2File.GetIdxOfConnectedString(id);
		case LocT10V3:
			return locTurn10V3File.GetIdxOfConnectedString(id);
		case LocT10V4:
			return locTurn10V4File.GetIdxOfConnectedString(id);
		case LocT10V8:
			return locTurn10V8File.GetIdxOfConnectedString(id);
		}
		
		return -1;
	}

	int32_t GetIdxOfConnectedTag(uint16_t id) {
		switch (currentlyLoadedLoctext) {
		case Loc1:
			return loc1File.GetIdxOfConnectedTag(id);
		case Loc2:
			return loc2File.GetIdxOfConnectedTag(id);
		case LocT10V3:
			return locTurn10V3File.GetIdxOfConnectedTag(id);
		case LocT10V4:
			return locTurn10V4File.GetIdxOfConnectedTag(id);
		case LocT10V8:
			return locTurn10V8File.GetIdxOfConnectedTag(id);
		}
		
		return -1;
	}

	int32_t GetIdxOfConnectedComment(uint16_t id) {
		switch (currentlyLoadedLoctext) {
		case Loc1:
			return loc1File.GetIdxOfConnectedComment(id);
		case Loc2:
			return loc2File.GetIdxOfConnectedComment(id);
		}
		
		return -2;
	}

	bool IsIdxConnectedToTag(uint16_t id) {
		switch (currentlyLoadedLoctext) {
		case Loc1:
			return loc1File.GetIdxOfConnectedTag(id) != -1;
		case Loc2:
			return loc2File.IsIdxConnectedToTag(id);
		case LocT10V3:
			return locTurn10V3File.IsIdxConnectedToTag(id);
		case LocT10V4:
			return locTurn10V4File.IsIdxConnectedToTag(id);
		case LocT10V8:
			return locTurn10V8File.IsIdxConnectedToTag(id);
		}
		
		return false;
	}

	bool IsHashConnectedToTag(uint16_t id) {
		switch (currentlyLoadedLoctext) {
		case Loc1:
			return loc1File.IsHashConnectedToTag(id);
		case Loc2:
			return loc2File.IsHashConnectedToTag(id);
		case LocT10V3:
			return locTurn10V3File.IsHashConnectedToTag(id);
		case LocT10V4:
			return locTurn10V4File.IsHashConnectedToTag(id);
		case LocT10V8:
			return locTurn10V8File.IsHashConnectedToTag(id);
		}
		
		return false;
	}

	bool IsIdxConnectedToComment(uint16_t id) {
		switch (currentlyLoadedLoctext) {
		case Loc1:
			return loc1File.IsIdxConnectedToComment(id);
		case Loc2:
			return loc2File.IsIdxConnectedToComment(id);
		}
		
		return false;
	}

	wchar_t* GetStringPtr(uint16_t id) {
		switch (currentlyLoadedLoctext) {
		case Loc1:
			return loc1File.labelTable.stringTable.strings[id].string;
		case Loc2:
			return loc2File.labelTable.stringTable.strings[id].string;
		case LocT10V3:
			return locTurn10V3File.labelTable.stringTable.strings[id].string;
		case LocT10V4:
			return locTurn10V4File.labelTable.stringTable.strings[id].string;
		case LocT10V8:
			return (wchar_t*)locTurn10V8File.labelTable.stringTable.strings[id].string;
		}
		
		return nullptr;
	}

	char* GetTagPtr(uint16_t id) {
		switch (currentlyLoadedLoctext) {
		case Loc1:
			return loc1File.labelTable.tagTable.tags[id].val;
		case Loc2:
			return loc2File.labelTable.tagTable.tags[id].val;
		case LocT10V8:
			return locTurn10V8File.labelTable.tagTable.tags[id].val;
		}
		
		return nullptr;
	}
	
	wchar_t* GetWideTagPtr(uint16_t id) {
		switch (currentlyLoadedLoctext) {
		case LocT10V3:
			return locTurn10V3File.labelTable.tagTable.tags[id].val;
		case LocT10V4:
			return locTurn10V4File.labelTable.tagTable.tags[id].val;
		}
		
		return nullptr;
	}

	char* GetCommentPtr(uint16_t id) {
		switch (currentlyLoadedLoctext) {
		case Loc1:
			return loc1File.labelTable.commentTable.comments[id].val;
		case Loc2:
			return loc2File.labelTable.commentTable.comments[id].val;
		}
#
		return nullptr;
	}

	uint16_t GetPosPtr(uint16_t id) {
		switch (currentlyLoadedLoctext) {
		case Loc1:
			if (loc1File.usesPos) {
				return loc1File.labelTable.posTable.entries[id];
			}
			return loc1File.labelTable.stringTable.infoEntries[id].hash;
		case Loc2:
			if (loc2File.usesPos) {
				return loc2File.labelTable.posTable.entries[id];
			}
			return loc2File.labelTable.stringTable.infoEntries[id].hash;
		case LocT10V3:
			return locTurn10V3File.labelTable.stringTable.infoEntries[id].hash;
		case LocT10V4:
			return locTurn10V4File.labelTable.stringTable.infoEntries[id].hash;
		case LocT10V8:
			return locTurn10V8File.labelTable.stringTable.infoEntries[id].hash;
		}
		
		return -1;
	}
};

#endif