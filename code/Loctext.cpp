#include "Loctext.h"

#include "CommonReader.h"

#ifdef _WIN32 
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#pragma region Loctext
void Loctext::ClearActiveData()
{
	if (loctextPtr != nullptr)
	{
		free(loctextPtr);
		loctextPtr = nullptr;
	}
	
	if (labelTable.stringTable.infoEntries != nullptr)
	{
		delete[] labelTable.stringTable.infoEntries;
		labelTable.stringTable.infoEntries = nullptr;
	}
	
	if (labelTable.stringTable.strings != nullptr)
	{
		delete[] labelTable.stringTable.strings;
		labelTable.stringTable.strings = nullptr;
	}
	
	if (labelTable.tagTable.infoEntries != nullptr)
	{
		delete[] labelTable.tagTable.infoEntries;
		labelTable.tagTable.infoEntries = nullptr;
	}
	
	if (labelTable.tagTable.tags != nullptr)
	{
		delete[] labelTable.tagTable.tags;
		labelTable.tagTable.tags = nullptr;
	}
	
	if (labelTable.commentTable.comments != nullptr)
	{
		delete[] labelTable.commentTable.comments;
		labelTable.commentTable.comments = nullptr;
	}
	
	if (labelTable.commentTable.entries != nullptr)
	{
		delete[] labelTable.commentTable.entries;
		labelTable.commentTable.entries = nullptr;
	}
	
	if (labelTable.posTable.entries != nullptr)
	{
		delete[] labelTable.posTable.entries;
		labelTable.posTable.entries = nullptr;
	}
}

void Loctext::ReadLoctext(char* data) {
	loctextPtr = data;

	try{
		uint32_t MAGCHECK = 0;
		memcpy(&MAGCHECK, loctextPtr, sizeof(int32_t));

		int32_t isMagic = 0;

		if (MAGCHECK == LOCTEXT_LBSL_MAGIC) {
			isMagic = 1;
		}

		if (MAGCHECK == LOCTEXT_LSBL_MAGIC) {
			isMagic = 1;
		}

		// Small hack for if a file is provided right at the magic, skip needing to deal with the initial table offsets entirely if we are.
		if (isMagic == 0) {
			// Really stupid hack for Goldeneye 007 (XBLA), which has a dumbass header offsetting the entire file.
			int32_t offs = 0;
			if (strcmp(loctextPtr, "text") == 0) {
				if (strcmp(loctextPtr + 5, "02.09.05.0034") == 0) {
					offs = 0x14;
				}

				if (strcmp(loctextPtr + 5, "04.05.05.0032") == 0) {
					offs = 0x14;
				}

				if (strcmp(loctextPtr + 5, "04.05.05.0032") != 0 && strcmp(loctextPtr + 5, "02.09.05.0034") != 0) {
					offs = 0x20;
				}
			}

			int32_t labelDataOffsetVar = 0;
			int32_t unkTableOffsetVar = 0;
			int16_t unkTableCountVar = 0;
			int16_t unk1Var = 0;

			memcpy(&labelDataOffsetVar, loctextPtr + offs, sizeof(int32_t));
			memcpy(&unkTableOffsetVar, loctextPtr + 4 + offs, sizeof(int32_t));
			memcpy(&unkTableCountVar, loctextPtr + 8 + offs, sizeof(int16_t));
			memcpy(&unk1Var, loctextPtr + 0xA + offs, sizeof(int16_t));

			labelDataOffset = flipEndian(labelDataOffsetVar);
			unkTableOffset = flipEndian(unkTableOffsetVar);
			unkTableCount = flipEndian(unkTableCountVar);
			unk1 = flipEndian(unk1Var);

			printf("Main File - <%d %d>\n", labelDataOffset, flipEndian(labelDataOffset));

			int32_t end = SRC_ENDIANBIG;

			if (labelDataOffset >= 4096 || labelDataOffset < 0) {
				end = SRC_ENDIANLITTLE;
			}

			if (end == SRC_ENDIANLITTLE) {
				labelDataOffset = flipEndian(labelDataOffset);
				unkTableOffset = flipEndian(unkTableOffset);
				unkTableCount = flipEndian(unkTableCount);
				unk1 = flipEndian(unk1);
			}

			unknownTable.unkEntries = new UnkEntry[unkTableCount];
			unknownTable.unkTags = new TagStr[unkTableCount];

			for (int32_t i = 0; i < unkTableCount; i++) {
				int32_t offset = 0;
				int16_t id = 0;
				int16_t idx = 0;

				if (end == SRC_ENDIANLITTLE) {
					memcpy(&unknownTable.unkEntries[i].tagoffset, loctextPtr + unkTableOffset + (8 * i), sizeof(int32_t));
					memcpy(&unknownTable.unkEntries[i].id, loctextPtr + unkTableOffset + (8 * i) + 4, sizeof(int16_t));
					memcpy(&unknownTable.unkEntries[i].idx, loctextPtr + unkTableOffset + (8 * i) + 6, sizeof(int16_t));
				}
				else if (end == SRC_ENDIANBIG) {
					memcpy(&offset, loctextPtr + unkTableOffset + (8 * i), sizeof(int32_t));
					memcpy(&id, loctextPtr + unkTableOffset + (8 * i) + 4, sizeof(int16_t));
					memcpy(&idx, loctextPtr + unkTableOffset + (8 * i) + 6, sizeof(int16_t));

					unknownTable.unkEntries[i].tagoffset = flipEndian(offset);
					unknownTable.unkEntries[i].id = flipEndian(id);
					unknownTable.unkEntries[i].idx = flipEndian(idx);
				}

				printf("Unknown Table - Entry %d = <%d %02x %d>\n", i, unknownTable.unkEntries[i].tagoffset, unknownTable.unkEntries[i].id, unknownTable.unkEntries[i].idx);
			}
		}

		memcpy(&labelTable.header.magic, loctextPtr + labelDataOffset, sizeof(int32_t));

		// The only section that cares about endianness is the label data. Every other section treats it as little endian.
		ReadLabelData();

		usesTags = labelTable.header.tagTableOffset != 0;
		usesComments = labelTable.header.commentTableOffset != 0;
		usesPos = labelTable.header.positionTableOffset != 0;

		// Kameo and PDZ don't use this table, so it needs to be checked beforehand.
		if (labelTable.header.tagTableOffset != 0) {
			ReadTagData();
		}

		//Now for the completely optional tables.
		if (labelTable.header.commentTableOffset != 0) {
			ReadCommentData();
		}

		if (labelTable.header.positionTableOffset != 0) {
			ReadPosData();
		}
	}
	catch(int32_t ex){
		printf("Loctext::ReadLoctext() - An error occured while reading the loctext file.\n");
	}
}

void Loctext::ReadLabelData() {
	if (loctextPtr == NULL) {
		printf("Loctext::ReadLabelData() called while loctextPtr is null.\n");
		return;
	}

	int32_t dataOffs = 0;
	int32_t dataSize = 6;
	
	if (labelTable.header.magic[0] == LOCTEXT_LSBL[0] && labelTable.header.magic[1] == LOCTEXT_LSBL[1] && labelTable.header.magic[2] == LOCTEXT_LSBL[2] && labelTable.header.magic[3] == LOCTEXT_LSBL[3]) {
		endianness = SRC_ENDIANLITTLE;
	}

	if (labelTable.header.magic[0] == LOCTEXT_LBSL[0] && labelTable.header.magic[1] == LOCTEXT_LBSL[1] && labelTable.header.magic[2] == LOCTEXT_LBSL[2] && labelTable.header.magic[3] == LOCTEXT_LBSL[3]) {
		endianness = SRC_ENDIANBIG;
	}

	if (endianness != SRC_ENDIANLITTLE && endianness != SRC_ENDIANBIG) {
		printf("Loctext::ReadLabelData() somehow ended up with an invalid endian value of %d.\n", endianness);
		return;
	}

	printf("Loctext::ReadLabelData() ENDIAN - %08x.\n", labelTable.header.magic);

	if (endianness == SRC_ENDIANLITTLE) {
		memcpy(&labelTable.header.headerLen, loctextPtr + labelDataOffset + 4 + dataOffs, sizeof(int32_t));
		memcpy(&labelTable.header.entryTotal, loctextPtr + labelDataOffset + 8 + dataOffs, sizeof(int32_t));
		memcpy(&labelTable.header.stringTableOffset, loctextPtr + labelDataOffset + 0xC + dataOffs, sizeof(int32_t));
		memcpy(&labelTable.header.tagTableOffset, loctextPtr + labelDataOffset + 0x10 + dataOffs, sizeof(int32_t));
		memcpy(&labelTable.header.commentTableOffset, loctextPtr + labelDataOffset + 0x14 + dataOffs, sizeof(int32_t));
		memcpy(&labelTable.header.positionTableOffset, loctextPtr + labelDataOffset + 0x18 + dataOffs, sizeof(int32_t));

		memcpy(&labelTable.stringTable.header.totalSectLen, loctextPtr + labelDataOffset + labelTable.header.stringTableOffset, sizeof(int32_t));
		memcpy(&labelTable.stringTable.header.totalCount, loctextPtr + labelDataOffset + labelTable.header.stringTableOffset + 4, sizeof(int32_t));
	}
	else if (endianness == SRC_ENDIANBIG) {
		int32_t unk1; // 0x4
		int32_t unk2; // 0x8
		int32_t stringTableOffset; // 0xC
		int32_t tagTableOffset; // 0x10
		int32_t commentTableOffset; // 0x14
		int32_t unkTableOffset; // 0x18

		memcpy(&unk1, loctextPtr + labelDataOffset + 4 + dataOffs, sizeof(int32_t));
		memcpy(&unk2, loctextPtr + labelDataOffset + 8 + dataOffs, sizeof(int32_t));
		memcpy(&stringTableOffset, loctextPtr + labelDataOffset + 0xC + dataOffs, sizeof(int32_t));
		memcpy(&tagTableOffset, loctextPtr + labelDataOffset + 0x10 + dataOffs, sizeof(int32_t));
		memcpy(&commentTableOffset, loctextPtr + labelDataOffset + 0x14 + dataOffs, sizeof(int32_t));
		memcpy(&unkTableOffset, loctextPtr + labelDataOffset + 0x18 + dataOffs, sizeof(int32_t));

		labelTable.header.headerLen = flipEndian(unk1);
		labelTable.header.entryTotal = flipEndian(unk2);
		labelTable.header.stringTableOffset = flipEndian(stringTableOffset);
		labelTable.header.tagTableOffset = flipEndian(tagTableOffset);
		labelTable.header.commentTableOffset = flipEndian(commentTableOffset);
		labelTable.header.positionTableOffset = flipEndian(unkTableOffset);

		int32_t totalSectLenVar = 0;
		int32_t totalStrings = 0;

		memcpy(&totalSectLenVar, loctextPtr + labelDataOffset + labelTable.header.stringTableOffset + dataOffs, sizeof(int32_t));
		memcpy(&totalStrings, loctextPtr + labelDataOffset + labelTable.header.stringTableOffset + 4 + dataOffs, sizeof(int32_t));

		labelTable.stringTable.header.totalSectLen = flipEndian(totalSectLenVar);
		labelTable.stringTable.header.totalCount = flipEndian(totalStrings);
	}

	printf("Loctext File: %d %d %d %d\n", labelTable.header.stringTableOffset, labelTable.header.tagTableOffset, labelTable.header.commentTableOffset, labelTable.header.positionTableOffset);
	printf("String Table: %d %d\n", labelTable.stringTable.header.totalSectLen, labelTable.stringTable.header.totalCount);

	int32_t strInfoOffset = labelDataOffset + labelTable.header.stringTableOffset + 8;
	labelTable.stringTable.infoEntries = new LabelStrInfoEntry[labelTable.stringTable.header.totalCount];
	labelTable.stringTable.strings = new LabelStrEntry[labelTable.stringTable.header.totalCount];
	for (int32_t i = 0; i < labelTable.stringTable.header.totalCount; i++) {
		if (endianness == SRC_ENDIANLITTLE) {
			memcpy(&labelTable.stringTable.infoEntries[i].hash, loctextPtr + strInfoOffset + (6 * i), sizeof(int16_t));
			memcpy(&labelTable.stringTable.infoEntries[i].offset, loctextPtr + strInfoOffset + (6 * i) + 2, sizeof(int32_t));
		}
		else if (endianness == SRC_ENDIANBIG) {
			uint16_t strUnkVal; // 0x0 (Only on LSB2 W/ no Tag Table.)
			uint16_t strIdVar; // 0x0 (0x2 on LSB2 W/ no Tag Table.)
			int32_t strOffsetVar; // 0x2 (0x4 on LSB2 W/ no Tag Table.)

			/*if (labelTable.header.magic == LOCTEXT_LSBTWO_MAGIC) {
				if (labelTable.header.tagTableOffset == 0) {
					memcpy(&strUnkVal, loctextPtr + strInfoOffset + (dataSize * i), sizeof(int16_t));
					memcpy(&strIdVar, loctextPtr + strInfoOffset + (dataSize * i) + 2, sizeof(int16_t));
					memcpy(&strOffsetVar, loctextPtr + strInfoOffset + (dataSize * i) + 4, sizeof(int32_t));

					labelTable.stringTable.infoEntries[i].unk = flipEndian(strUnkVal);
				}
				else {
					memcpy(&strIdVar, loctextPtr + strInfoOffset + (dataSize * i), sizeof(int16_t));
					memcpy(&strOffsetVar, loctextPtr + strInfoOffset + (dataSize * i) + 2, sizeof(int32_t));
				}
			}
			else {
				memcpy(&strIdVar, loctextPtr + strInfoOffset + (dataSize * i), sizeof(int16_t));
				memcpy(&strOffsetVar, loctextPtr + strInfoOffset + (dataSize * i) + 2, sizeof(int32_t));
			}*/

			memcpy(&strIdVar, loctextPtr + strInfoOffset + (dataSize * i), sizeof(int16_t));
			memcpy(&strOffsetVar, loctextPtr + strInfoOffset + (dataSize * i) + 2, sizeof(int32_t));

			labelTable.stringTable.infoEntries[i].hash = flipEndian(strIdVar);
			labelTable.stringTable.infoEntries[i].offset = flipEndian(strOffsetVar);
		}
		printf("String Table Entry %d: %04x %d\n", i, labelTable.stringTable.infoEntries[i].hash, labelTable.stringTable.infoEntries[i].offset);
	}

	int32_t strEntryBaseOffset = strInfoOffset + ((labelTable.stringTable.header.totalCount + 1) * dataSize);
	for (int32_t i = 0; i < labelTable.stringTable.header.totalCount; i++) {
		int32_t offs = strEntryBaseOffset + (labelTable.stringTable.infoEntries[i].offset * 2);

		wchar_t chr = 0xFFFF;
		int32_t idx = 0;
		if (endianness == SRC_ENDIANLITTLE) {
			while (chr != '\0') {
				memcpy(&chr, loctextPtr + offs, sizeof(char16_t));
				labelTable.stringTable.strings[i].string[idx] = chr;
				idx++;
				offs += 2;
			}
		}
		else if (endianness == SRC_ENDIANBIG) {
			while (chr != '\0') {
				memcpy(&chr, loctextPtr + offs, sizeof(char16_t));
				labelTable.stringTable.strings[i].string[idx] = flipEndian(chr);
				idx++;
				offs += 2;
			}
		}

		printf(u8"String Table Val %d: (Length %d) %ws", i, (int32_t)wcslen(labelTable.stringTable.strings[i].string), labelTable.stringTable.strings[i].string);
		printf("\n");
	}
}

void Loctext::ReadTagData() {
	if (loctextPtr == NULL) {
		printf("Loctext::ReadTagData() called while loctextPtr is null.\n");
		return;
	}

	bool doesEndMatter = false;

	/*if (labelTable.header.magic == LOCTEXT_LSBTWO_MAGIC) {
		doesEndMatter = true;
	}*/

	if (doesEndMatter) {
		int32_t totalSectLen = 0;
		int32_t totalCount = 0;

		// Endianness doesn't matter here for some reason. All games that use this always treats these as little endian.
		memcpy(&totalSectLen, loctextPtr + labelDataOffset + labelTable.header.tagTableOffset, sizeof(int32_t));
		memcpy(&totalCount, loctextPtr + labelDataOffset + labelTable.header.tagTableOffset + 4, sizeof(int32_t));

		labelTable.tagTable.header.totalSectLen = flipEndian(totalSectLen);
		labelTable.tagTable.header.totalCount = flipEndian(totalCount);

		int32_t tagTableOffset = labelDataOffset + labelTable.header.tagTableOffset + 8;
		labelTable.tagTable.infoEntries = new TagInfo[labelTable.tagTable.header.totalCount];
		labelTable.tagTable.tags = new TagStr[labelTable.tagTable.header.totalCount];
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			int16_t id = 0;
			int16_t unk1 = 0;
			int32_t offset = 0;

			memcpy(&id, loctextPtr + tagTableOffset + (8 * i), sizeof(int16_t));
			memcpy(&unk1, loctextPtr + tagTableOffset + (8 * i) + 2, sizeof(int16_t));
			memcpy(&offset, loctextPtr + tagTableOffset + (8 * i) + 4, sizeof(int32_t));

			labelTable.tagTable.infoEntries[i].id = flipEndian(id);
			labelTable.tagTable.infoEntries[i].unk1 = flipEndian(unk1);
				labelTable.tagTable.infoEntries[i].offset = flipEndian(offset);

			printf("Tag Table Entry %d: %04x %04x %d\n", i, labelTable.tagTable.infoEntries[i].id, labelTable.tagTable.infoEntries[i].unk1, labelTable.tagTable.infoEntries[i].offset);
		}

		int32_t strEntryBaseOffset = tagTableOffset + (labelTable.stringTable.header.totalCount * 8);
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			int32_t offs = strEntryBaseOffset + labelTable.tagTable.infoEntries[i].offset;

			strcpy(labelTable.tagTable.tags[i].val, loctextPtr + offs);

			printf("Tag Table Val %d: %s\n", i, labelTable.tagTable.tags[i].val);
		}
	}
	else {
		// Endianness doesn't matter here for some reason. All games that use this always treats these as little endian.
		memcpy(&labelTable.tagTable.header.totalSectLen, loctextPtr + labelDataOffset + labelTable.header.tagTableOffset, sizeof(int32_t));
		memcpy(&labelTable.tagTable.header.totalCount, loctextPtr + labelDataOffset + labelTable.header.tagTableOffset + 4, sizeof(int32_t));

		int32_t tagTableOffset = labelDataOffset + labelTable.header.tagTableOffset + 8;
		labelTable.tagTable.infoEntries = new TagInfo[labelTable.tagTable.header.totalCount];
		labelTable.tagTable.tags = new TagStr[labelTable.tagTable.header.totalCount];
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {

			memcpy(&labelTable.tagTable.infoEntries[i].id, loctextPtr + tagTableOffset + (8 * i), sizeof(int16_t));
			memcpy(&labelTable.tagTable.infoEntries[i].unk1, loctextPtr + tagTableOffset + (8 * i) + 2, sizeof(int16_t));
			memcpy(&labelTable.tagTable.infoEntries[i].offset, loctextPtr + tagTableOffset + (8 * i) + 4, sizeof(int32_t));

			printf("Tag Table Entry %d: %04x %04x %d\n", i, labelTable.tagTable.infoEntries[i].id, labelTable.tagTable.infoEntries[i].unk1, labelTable.tagTable.infoEntries[i].offset);
		}

		int32_t strEntryBaseOffset = tagTableOffset + (labelTable.stringTable.header.totalCount * 8);
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			int32_t offs = strEntryBaseOffset + labelTable.tagTable.infoEntries[i].offset;

			strcpy(labelTable.tagTable.tags[i].val, loctextPtr + offs);

			printf("Tag Table Val %d: %s\n", i, labelTable.tagTable.tags[i].val);
		}
	}
}

void Loctext::ReadCommentData() {
	if (loctextPtr == NULL) {
		printf("Loctext::ReadCommentData() called while loctextPtr is null.\n");
		return;
	}

	bool doesEndMatter = false;

	/*if (labelTable.header.magic == LOCTEXT_LSBTWO_MAGIC) {
		doesEndMatter = true;
	}*/

	if (doesEndMatter) {
		int32_t sectLen = 0;
		int32_t totalCount = 0;

		memcpy(&sectLen, loctextPtr + labelDataOffset + labelTable.header.commentTableOffset, sizeof(int32_t));
		memcpy(&totalCount, loctextPtr + labelDataOffset + labelTable.header.commentTableOffset + 4, sizeof(int32_t));

		labelTable.commentTable.header.totalSectLen = flipEndian(sectLen);
		labelTable.commentTable.header.totalCount = flipEndian(totalCount);

		int32_t commentTableOffset = labelDataOffset + labelTable.header.commentTableOffset + 8;
		labelTable.commentTable.entries = new CommentEntry[labelTable.commentTable.header.totalCount];
		labelTable.commentTable.comments = new CommentStr[labelTable.commentTable.header.totalCount];

		for (int32_t i = 0; i < labelTable.commentTable.header.totalCount; i++) {
			int16_t id = 0;
			int32_t offset = 0;
			memcpy(&labelTable.commentTable.entries[i].unk1, loctextPtr + commentTableOffset + (8 * i), sizeof(char));
			memcpy(&id, loctextPtr + commentTableOffset + (8 * i) + 1, sizeof(int16_t));
			memcpy(&labelTable.commentTable.entries[i].unk2, loctextPtr + commentTableOffset + (8 * i) + 3, sizeof(char));
			memcpy(&offset, loctextPtr + commentTableOffset + (8 * i) + 4, sizeof(int32_t));

			labelTable.commentTable.entries[i].id = flipEndian(id);
			labelTable.commentTable.entries[i].offset = flipEndian(offset);

			printf("Comment Table Entry %d: %04x %d\n", i, labelTable.commentTable.entries[i].id, labelTable.commentTable.entries[i].offset);
		}

		int32_t strEntryBaseOffset = commentTableOffset + (labelTable.commentTable.header.totalCount * 8);
		for (int32_t i = 0; i < labelTable.commentTable.header.totalCount; i++) {
			int32_t offs = strEntryBaseOffset + labelTable.commentTable.entries[i].offset;

			strcpy(labelTable.commentTable.comments[i].val, loctextPtr + offs);

			printf("Comment Table Val %d: %s", i, labelTable.commentTable.comments[i].val);
		}
	}
	else {
		// Endianness doesn't matter here for some reason. All games that use this always treats these as little endian.
		memcpy(&labelTable.commentTable.header.totalSectLen, loctextPtr + labelDataOffset + labelTable.header.commentTableOffset, sizeof(int32_t));
		memcpy(&labelTable.commentTable.header.totalCount, loctextPtr + labelDataOffset + labelTable.header.commentTableOffset + 4, sizeof(int32_t));

		int32_t commentTableOffset = labelDataOffset + labelTable.header.commentTableOffset + 8;
		labelTable.commentTable.entries = new CommentEntry[labelTable.commentTable.header.totalCount];
		labelTable.commentTable.comments = new CommentStr[labelTable.commentTable.header.totalCount];

		for (int32_t i = 0; i < labelTable.commentTable.header.totalCount; i++) {
			memcpy(&labelTable.commentTable.entries[i].unk1, loctextPtr + commentTableOffset + (8 * i), sizeof(char));
			memcpy(&labelTable.commentTable.entries[i].id, loctextPtr + commentTableOffset + (8 * i) + 1, sizeof(int16_t));
			memcpy(&labelTable.commentTable.entries[i].unk2, loctextPtr + commentTableOffset + (8 * i) + 3, sizeof(char));
			memcpy(&labelTable.commentTable.entries[i].offset, loctextPtr + commentTableOffset + (8 * i) + 4, sizeof(int32_t));

			printf("Comment Table Entry %d: %04x %d\n", i, labelTable.commentTable.entries[i].id, labelTable.commentTable.entries[i].offset);
		}

		int32_t strEntryBaseOffset = commentTableOffset + (labelTable.commentTable.header.totalCount * 8);
		for (int32_t i = 0; i < labelTable.commentTable.header.totalCount; i++) {
			int32_t offs = strEntryBaseOffset + labelTable.commentTable.entries[i].offset;

			strcpy(labelTable.commentTable.comments[i].val, loctextPtr + offs);

			printf("Comment Table Val %d: %s", i, labelTable.commentTable.comments[i].val);
		}
	}
}

void Loctext::ReadPosData() {
	if (loctextPtr == NULL) {
		printf("Loctext::ReadPosData() called while loctextPtr is null.\n");
		return;
	}

	bool doesEndMatter = false;

	/*if (labelTable.header.magic == LOCTEXT_LSBTWO_MAGIC) {
		doesEndMatter = true;
	}*/

	if (doesEndMatter) {
		int32_t sectLen = 0;
		int32_t totalCount = 0;

		memcpy(&sectLen, loctextPtr + labelDataOffset + labelTable.header.positionTableOffset, sizeof(int32_t));
		memcpy(&totalCount, loctextPtr + labelDataOffset + labelTable.header.positionTableOffset + 4, sizeof(int32_t));

		labelTable.posTable.header.totalSectLen = flipEndian(sectLen);
		labelTable.posTable.header.totalCount = flipEndian(totalCount);

		int32_t posTableOffset = labelDataOffset + labelTable.header.positionTableOffset + 8;
		labelTable.posTable.entries = new uint16_t[labelTable.posTable.header.totalCount];
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			uint16_t posIdVar = 0; // 0x0

			memcpy(&posIdVar, loctextPtr + posTableOffset + (2 * i), sizeof(int16_t));

			labelTable.posTable.entries[i] = flipEndian(posIdVar);

			printf("Pos Table Entry %d: %04x\n", i, labelTable.posTable.entries[i]);
		}
	}
	else {
		// Endianness doesn't matter here for some reason. All games that use this always treats these as little endian.
		memcpy(&labelTable.posTable.header.totalSectLen, loctextPtr + labelDataOffset + labelTable.header.positionTableOffset, sizeof(int32_t));
		memcpy(&labelTable.posTable.header.totalCount, loctextPtr + labelDataOffset + labelTable.header.positionTableOffset + 4, sizeof(int32_t));

		int32_t posTableOffset = labelDataOffset + labelTable.header.positionTableOffset + 8;
		labelTable.posTable.entries = new uint16_t[labelTable.posTable.header.totalCount];
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			uint16_t posIdVar = 0; // 0x0

			memcpy(&posIdVar, loctextPtr + posTableOffset + (2 * i), sizeof(int16_t));

			labelTable.posTable.entries[i] = posIdVar;

			printf("Pos Table Entry %d: %04x\n", i, labelTable.posTable.entries[i]);
		}
	}
}

void Loctext::ExportToFileRaw(char* fileName) {
	int32_t position = 0;

	std::ofstream is(fileName, std::ios_base::trunc);

	if (labelTable.stringTable.header.totalCount == 0) {
		is.flush();
		is.close();
		return;
	}

	if (!usesTags) {
		for (int32_t i = 0; i < labelTable.stringTable.header.totalCount; i++) {

			char fullStr[4096];

			char conv[2048];

			int32_t total = wcstombs(conv, labelTable.stringTable.strings[i].string, 2048);

			sprintf(fullStr, "%s\n", conv);

			is.write(fullStr, strlen(fullStr));
		}
	}

	if (usesTags) {
		for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
			uint16_t idx = 0;

			// Check if the position table is present. If it is, get the value from that. If not then fallback on the tag table entries.
			if (usesPos) {
				idx = labelTable.posTable.entries[i];
			}
			else {
				idx = labelTable.tagTable.infoEntries[i].id;
			}

			// Some comments have new line characters while others don't.
			bool hasCommentGotNewLine = true;

			if (IsIdxConnectedToComment(idx)) {
				is.write(labelTable.commentTable.comments[GetIdxOfConnectedComment(idx)].val, strlen(labelTable.commentTable.comments[GetIdxOfConnectedComment(idx)].val));

				// Check if the comment has an new line character.
				hasCommentGotNewLine = strchr(labelTable.commentTable.comments[GetIdxOfConnectedComment(idx)].val, '\n') != nullptr;
			}

			if (IsHashConnectedToTag(idx)) {
				char fullStr[4096];

				char conv[2048];

				int32_t total = wcstombs(conv, labelTable.stringTable.strings[GetIdxOfConnectedString(idx)].string, 2048);
				
				// If a comment doesn't have a new line, apply one before we write our next value. Otherwise just write as normal.
				if (hasCommentGotNewLine) {
					sprintf(fullStr, "%s\t\t\t= \"%s\"\n", labelTable.tagTable.tags[GetIdxOfConnectedTag(idx)].val, conv);
				}
				else {
					sprintf(fullStr, "\n%s\t\t\t= \"%s\"\n", labelTable.tagTable.tags[GetIdxOfConnectedTag(idx)].val, conv);
				}

				is.write(fullStr, strlen(fullStr));
			}
		}
	}

	if (IsIdxConnectedToComment(-1)) {
		is.write(labelTable.commentTable.comments[GetIdxOfConnectedComment(-1)].val, strlen(labelTable.commentTable.comments[GetIdxOfConnectedComment(-1)].val));
	}

	is.flush();
	is.close();
}

void Loctext::ExportToFileBank(char* fileName, int32_t endianness) {
	int32_t bufSize = 0xC + labelTable.header.headerLen + labelTable.stringTable.header.totalSectLen + labelTable.tagTable.header.totalSectLen + labelTable.commentTable.header.totalSectLen + labelTable.posTable.header.totalSectLen;

	char* buffer = (char*)malloc(bufSize);
	int32_t position = 0;

	std::ofstream is(fileName, std::ios_base::trunc);

	for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
		uint16_t idx = 0;

		// Check if the position table is present. If it is, get the value from that. If not then fallback on the tag table entries.
		if (labelTable.header.positionTableOffset != 0) {
			idx = labelTable.posTable.entries[i];
		}
		else {
			idx = labelTable.tagTable.infoEntries[i].id;
		}

		// Some comments have new line characters while others don't.
		bool hasCommentGotNewLine = true;

		if (IsIdxConnectedToComment(idx)) {
			is.write(labelTable.commentTable.comments[GetIdxOfConnectedComment(idx)].val, strlen(labelTable.commentTable.comments[GetIdxOfConnectedComment(idx)].val));

			// Check if the comment has an new line character.
			hasCommentGotNewLine = strchr(labelTable.commentTable.comments[GetIdxOfConnectedComment(idx)].val, '\n') != nullptr;
		}

		if (IsHashConnectedToTag(idx)) {
			char fullStr[4096];

			char conv[2048];

			int32_t total = wcstombs(conv, labelTable.stringTable.strings[GetIdxOfConnectedString(idx)].string, 2048);

			// If a comment doesn't have a new line, apply one before we write our next value. Otherwise just write as normal.
			if (hasCommentGotNewLine) {
				sprintf(fullStr, "%s\t\t\t= \"%s\"\n", labelTable.tagTable.tags[GetIdxOfConnectedTag(idx)].val, conv);
			}
			else {
				sprintf(fullStr, "\n%s\t\t\t= \"%s\"\n", labelTable.tagTable.tags[GetIdxOfConnectedTag(idx)].val, conv);
			}

			is.write(fullStr, strlen(fullStr));
		}
	}

	if (IsIdxConnectedToComment(-1)) {
		is.write(labelTable.commentTable.comments[GetIdxOfConnectedComment(-1)].val, strlen(labelTable.commentTable.comments[GetIdxOfConnectedComment(-1)].val));
	}

	is.flush();
	is.close();
}

void Loctext::WriteLoctext(char* filename) {
	int32_t position = 0;

	FILE* writeStrm = fopen(filename, "wb");

	if (writeStrm == nullptr) {
		printf("Error occured while trying to open the write stream. Error Code 0x%08X\n", errno);
		return;
	}

	try {
		int32_t CVal = 0xC;
		int32_t zeroVal = 0;

		if (startEndianness == SRC_ENDIANLITTLE) {
			fwrite(&CVal, 4, 1, writeStrm);
			fwrite(&zeroVal, 4, 1, writeStrm);
			fwrite(&zeroVal, 4, 1, writeStrm);
		}
		else {
			CVal = flipEndian(CVal);
			fwrite(&CVal, 4, 1, writeStrm);
			fwrite(&zeroVal, 4, 1, writeStrm);
			fwrite(&zeroVal, 4, 1, writeStrm);
		}

		int32_t lableTableSize = 16 + (labelTable.stringTable.header.totalCount * 6);
		int32_t tagTableSize = 8 + (labelTable.tagTable.header.totalCount * 8);
		int32_t commentTableSize = 8 + (labelTable.commentTable.header.totalCount * 8);
		int32_t posTableSize = 8 + (labelTable.posTable.header.totalCount * 2);

		//Precalculate some offs.
		for (int32_t i = 0; i < labelTable.stringTable.header.totalCount; i++) {
			lableTableSize += (wcslen(labelTable.stringTable.strings[i].string) * 2) + 2;
		}

		if (usesTags) {
			for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
				tagTableSize += strlen(labelTable.tagTable.tags[i].val) + 1;
			}
		}
		else {
			tagTableSize = 0;
		}

		if (usesComments) {
			for (int32_t i = 0; i < labelTable.commentTable.header.totalCount; i++) {
				commentTableSize += strlen(labelTable.commentTable.comments[i].val) + 1;
			}
		}
		else {
			commentTableSize = 0;
		}

		if (!usesPos) posTableSize = 0;

		int32_t locBankHeadSize = 0x1C;
		int32_t locEntryCount = 4;
		int32_t lableTableOffs = 0x1C;
		int32_t tagTableOffs = 0;
		int32_t commentTableOffs = 0;
		int32_t posTableOffs = 0;

		if (usesTags) {
			tagTableOffs = locBankHeadSize + lableTableSize;
		}
		if (usesComments) {
			commentTableOffs = locBankHeadSize + lableTableSize + tagTableSize;
		}
		if (usesPos) {
			posTableOffs = locBankHeadSize + lableTableSize + tagTableSize + commentTableSize;
		}

		if (endianness == SRC_ENDIANLITTLE) {
			fwrite(&LOCTEXT_LSBL, 4, 1, writeStrm);
			fwrite(&locBankHeadSize, 4, 1, writeStrm);
			fwrite(&locEntryCount, 4, 1, writeStrm);
			fwrite(&lableTableOffs, 4, 1, writeStrm);
			fwrite(&tagTableOffs, 4, 1, writeStrm);
			fwrite(&commentTableOffs, 4, 1, writeStrm);
			fwrite(&posTableOffs, 4, 1, writeStrm);
		}
		else {
			fwrite(&LOCTEXT_LBSL, 4, 1, writeStrm);

			locBankHeadSize = flipEndian(locBankHeadSize);
			locEntryCount = flipEndian(locEntryCount);
			lableTableOffs = flipEndian(lableTableOffs);
			tagTableOffs = flipEndian(tagTableOffs);
			commentTableOffs = flipEndian(commentTableOffs);
			posTableOffs = flipEndian(posTableOffs);

			fwrite(&locBankHeadSize, 4, 1, writeStrm);
			fwrite(&locEntryCount, 4, 1, writeStrm);
			fwrite(&lableTableOffs, 4, 1, writeStrm);
			fwrite(&tagTableOffs, 4, 1, writeStrm);
			fwrite(&commentTableOffs, 4, 1, writeStrm);
			fwrite(&posTableOffs, 4, 1, writeStrm);
		}

		// String Table Time
		if (endianness == SRC_ENDIANLITTLE) {
			fwrite(&lableTableSize, 4, 1, writeStrm);
			fwrite(&labelTable.stringTable.header.totalCount, 4, 1, writeStrm);
		}
		else {
			lableTableSize = flipEndian(lableTableSize);
			int32_t strTableTotalCount = flipEndian(labelTable.stringTable.header.totalCount);
			fwrite(&lableTableSize, 4, 1, writeStrm);
			fwrite(&strTableTotalCount, 4, 1, writeStrm);
		}

		int32_t currOffset = 0;
		for (int32_t i = 0; i < labelTable.stringTable.header.totalCount; i++) {
			int16_t hash = labelTable.stringTable.infoEntries[i].hash;
			int32_t offset = currOffset;

			if (endianness == SRC_ENDIANLITTLE) {
				fwrite(&hash, 2, 1, writeStrm);
				fwrite(&offset, 4, 1, writeStrm);
			}
			else {
				hash = flipEndian(hash);
				offset = flipEndian(offset);
				fwrite(&hash, 2, 1, writeStrm);
				fwrite(&offset, 4, 1, writeStrm);
			}

			currOffset += wcslen(labelTable.stringTable.strings[i].string) + 1;
		}

		int16_t blankHash = -1;
		// Insert a blank entry
		fwrite(&blankHash, 2, 1, writeStrm);
		if (endianness == SRC_ENDIANLITTLE) {
			fwrite(&currOffset, 4, 1, writeStrm);
		}
		else {
			int32_t lastOffs = flipEndian(currOffset);
			fwrite(&lastOffs, 4, 1, writeStrm);
		}
		
		for (int32_t i = 0; i < labelTable.stringTable.header.totalCount; i++) {
			if (endianness == SRC_ENDIANLITTLE) {
				fwrite(&labelTable.stringTable.strings[i].string, 2, wcslen(labelTable.stringTable.strings[i].string) + 1, writeStrm);
			}
			else {
				for (int32_t c = 0; c < wcslen(labelTable.stringTable.strings[i].string) + 1; c++) {
					wchar_t val = flipEndian(labelTable.stringTable.strings[i].string[c]);
					fwrite(&val, 2, 1, writeStrm);
				}
			}
		}

		int16_t blank = 0;
		fwrite(&blank, 2, 1, writeStrm);

		//Tag Table Time
		if (usesTags) {
			fwrite(&tagTableSize, 4, 1, writeStrm);
			fwrite(&labelTable.tagTable.header.totalCount, 4, 1, writeStrm);

			currOffset = 0;
			for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
				fwrite(&labelTable.tagTable.infoEntries[i].id, 2, 1, writeStrm);
				fwrite(&labelTable.tagTable.infoEntries[i].unk1, 2, 1, writeStrm);
				fwrite(&currOffset, 4, 1, writeStrm);
				currOffset += strlen(labelTable.tagTable.tags[i].val) + 1;
			}

			for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
				fwrite(&labelTable.tagTable.tags[i].val, 1, strlen(labelTable.tagTable.tags[i].val) + 1, writeStrm);
			}
		}

		//Comment Table Time
		if (usesComments) {
			fwrite(&commentTableSize, 4, 1, writeStrm);
			fwrite(&labelTable.commentTable.header.totalCount, 4, 1, writeStrm);

			int32_t commentOffs = 0;

			for (int32_t i = 0; i < labelTable.commentTable.header.totalCount; i++) {
				fwrite(&labelTable.commentTable.entries[i].unk1, 1, 1, writeStrm);
				fwrite(&labelTable.commentTable.entries[i].id, 2, 1, writeStrm);
				fwrite(&labelTable.commentTable.entries[i].unk2, 1, 1, writeStrm);
				fwrite(&commentOffs, 4, 1, writeStrm);

				commentOffs += strlen(labelTable.commentTable.comments[i].val) + 1;
			}
			for (int32_t i = 0; i < labelTable.commentTable.header.totalCount; i++) {
				fwrite(&labelTable.commentTable.comments[i].val, 1, strlen(labelTable.commentTable.comments[i].val) + 1, writeStrm);
			}
		}

		//Position Table Time
		if (usesPos) {
			fwrite(&posTableSize, 4, 1, writeStrm);
			fwrite(&labelTable.posTable.header.totalCount, 4, 1, writeStrm);

			for (int32_t i = 0; i < labelTable.posTable.header.totalCount; i++) {
				fwrite(&labelTable.posTable.entries[i], 2, 1, writeStrm);
			}
		}
	}
	catch (std::exception e) {

	}

	fflush(writeStrm);
	fclose(writeStrm);
}
#pragma endregion

#pragma region Loctext Rev Two
void LocTwo::ReadLoctext(char* data) {
	loctextPtr = data;

	try {
		uint32_t MAGCHECK = 0;
		memcpy(&MAGCHECK, loctextPtr, sizeof(int32_t));

		int32_t isMagic = 0;

		if (MAGCHECK == LOCTEXT_LSBTWO_MAGIC) {
			isMagic = 1;
		}

		if (MAGCHECK == LOCTEXT_LBSTWO_MAGIC) {
			isMagic = 1;
		}

		// Small hack for if a file is provided right at the magic, skip needing to deal with the initial table offsets entirely if we are.
		if (isMagic == 0) {
			// Really stupid hack for Goldeneye 007 (XBLA), which has a dumbass header offsetting the entire file.
			int32_t offs = 0;
			if (strcmp(loctextPtr, "text") == 0) {
				offs = 0x14;
			}

			int32_t labelDataOffsetVar = 0;
			int32_t unkTableOffsetVar = 0;
			int16_t unkTableCountVar = 0;
			int16_t unk1Var = 0;

			memcpy(&labelDataOffsetVar, loctextPtr + offs, sizeof(int32_t));
			memcpy(&unkTableOffsetVar, loctextPtr + 4 + offs, sizeof(int32_t));
			memcpy(&unkTableCountVar, loctextPtr + 8 + offs, sizeof(int16_t));
			memcpy(&unk1Var, loctextPtr + 0xA + offs, sizeof(int16_t));

			labelDataOffset = flipEndian(labelDataOffsetVar);
			unkTableOffset = flipEndian(unkTableOffsetVar);
			unkTableCount = flipEndian(unkTableCountVar);
			unk1 = flipEndian(unk1Var);

			printf("Main File - <%d %d>\n", labelDataOffset, flipEndian(labelDataOffset));

			int32_t end = SRC_ENDIANBIG;

			if (labelDataOffset >= 4096 || labelDataOffset < 0) {
				end = SRC_ENDIANLITTLE;
			}

			if (end == SRC_ENDIANLITTLE) {
				labelDataOffset = flipEndian(labelDataOffset);
				unkTableOffset = flipEndian(unkTableOffset);
				unkTableCount = flipEndian(unkTableCount);
				unk1 = flipEndian(unk1);
			}

			unknownTable.unkEntries = new UnkEntry[unkTableCount];
			unknownTable.unkTags = new TagStr[unkTableCount];

			for (int32_t i = 0; i < unkTableCount; i++) {
				int32_t offset = 0;
				int16_t id = 0;
				int16_t idx = 0;

				if (end == SRC_ENDIANLITTLE) {
					memcpy(&unknownTable.unkEntries[i].tagoffset, loctextPtr + unkTableOffset + (8 * i), sizeof(int32_t));
					memcpy(&unknownTable.unkEntries[i].id, loctextPtr + unkTableOffset + (8 * i) + 4, sizeof(int16_t));
					memcpy(&unknownTable.unkEntries[i].idx, loctextPtr + unkTableOffset + (8 * i) + 6, sizeof(int16_t));
				}
				else if (end == SRC_ENDIANBIG) {
					memcpy(&offset, loctextPtr + unkTableOffset + (8 * i), sizeof(int32_t));
					memcpy(&id, loctextPtr + unkTableOffset + (8 * i) + 4, sizeof(int16_t));
					memcpy(&idx, loctextPtr + unkTableOffset + (8 * i) + 6, sizeof(int16_t));

					unknownTable.unkEntries[i].tagoffset = flipEndian(offset);
					unknownTable.unkEntries[i].id = flipEndian(id);
					unknownTable.unkEntries[i].idx = flipEndian(idx);
				}

				printf("Unknown Table - Entry %d = <%d %02x %d>\n", i, unknownTable.unkEntries[i].tagoffset, unknownTable.unkEntries[i].id, unknownTable.unkEntries[i].idx);
			}
		}

		memcpy(&labelTable.header.magic, loctextPtr + labelDataOffset, sizeof(int32_t));


		ReadLabelData();

		// Kameo and Perfect Dark Zero don't use this table, so it needs to be checked beforehand.
		if (labelTable.header.tagTableOffset != 0) {
			ReadTagData();
		}

		//Now for the completely optional tables.
		if (labelTable.header.commentTableOffset != 0) {
			ReadCommentData();
		}

		if (labelTable.header.positionTableOffset != 0) {
			ReadPosData();
		}
	}
	catch (int32_t ex) {
		printf("Loctext::ReadLoctext() - An error occured while reading the loctext file.\n");
	}
}

void LocTwo::ReadLabelData() {
	if (loctextPtr == NULL) {
		printf("Loctext::ReadLabelData() called while loctextPtr is null.\n");
		return;
	}

	int32_t dataSize = 6;

	if (labelTable.header.magic[0] == LOCTEXT_LSB2[0] && labelTable.header.magic[1] == LOCTEXT_LSB2[1] && labelTable.header.magic[2] == LOCTEXT_LSB2[2] && labelTable.header.magic[3] == LOCTEXT_LSB2[3]) {
		endianness = SRC_ENDIANBIG;
	}

	if (labelTable.header.magic[0] == LOCTEXT_LBS2[0] && labelTable.header.magic[1] == LOCTEXT_LBS2[1] && labelTable.header.magic[2] == LOCTEXT_LBS2[2] && labelTable.header.magic[3] == LOCTEXT_LBS2[3]) {
		endianness = SRC_ENDIANLITTLE;
	}

	if (endianness != SRC_ENDIANLITTLE && endianness != SRC_ENDIANBIG) {
		printf("Loctext::ReadLabelData() somehow ended up with an invalid endian value of %d.\n", endianness);
		return;
	}

	printf("Loctext::ReadLabelData() ENDIAN - %08x.\n", labelTable.header.magic);

	memcpy(&labelTable.header.unk1, loctextPtr + labelDataOffset + 4, sizeof(int32_t));
	memcpy(&labelTable.header.unk2, loctextPtr + labelDataOffset + 8, sizeof(int32_t));
	memcpy(&labelTable.header.headerLen, loctextPtr + labelDataOffset + 0xC, sizeof(int32_t));
	memcpy(&labelTable.header.entryTotal, loctextPtr + labelDataOffset + 0x10, sizeof(int32_t));
	memcpy(&labelTable.header.stringTableOffset, loctextPtr + labelDataOffset + 0x14, sizeof(int32_t));
	memcpy(&labelTable.header.tagTableOffset, loctextPtr + labelDataOffset + 0x18, sizeof(int32_t));
	memcpy(&labelTable.header.commentTableOffset, loctextPtr + labelDataOffset + 0x1C, sizeof(int32_t));
	memcpy(&labelTable.header.positionTableOffset, loctextPtr + labelDataOffset + 0x20, sizeof(int32_t));

	if (endianness == SRC_ENDIANBIG) {
		labelTable.header.headerLen = flipEndian(labelTable.header.headerLen);
		labelTable.header.entryTotal = flipEndian(labelTable.header.entryTotal);
		labelTable.header.stringTableOffset = flipEndian(labelTable.header.stringTableOffset);
		labelTable.header.tagTableOffset = flipEndian(labelTable.header.tagTableOffset);
		labelTable.header.commentTableOffset = flipEndian(labelTable.header.commentTableOffset);
		labelTable.header.positionTableOffset = flipEndian(labelTable.header.positionTableOffset);
	}

	usesTags = labelTable.header.tagTableOffset != 0;
	usesComments = labelTable.header.commentTableOffset != 0;
	usesPos = labelTable.header.positionTableOffset != 0;

	printf("Loctext File: %d %d %d %d\n", labelTable.header.stringTableOffset, labelTable.header.tagTableOffset, labelTable.header.commentTableOffset, labelTable.header.positionTableOffset);
	printf("String Table: %d %d\n", labelTable.stringTable.header.totalSectLen, labelTable.stringTable.header.totalCount);

	memcpy(&labelTable.stringTable.header.totalSectLen, loctextPtr + labelDataOffset + labelTable.header.stringTableOffset, sizeof(int32_t));
	memcpy(&labelTable.stringTable.header.totalCount, loctextPtr + labelDataOffset + labelTable.header.stringTableOffset + 4, sizeof(int32_t));

	if (endianness == SRC_ENDIANBIG) {
		labelTable.stringTable.header.totalSectLen = flipEndian(labelTable.stringTable.header.totalSectLen);
		labelTable.stringTable.header.totalCount = flipEndian(labelTable.stringTable.header.totalCount);
	}

	int32_t strInfoOffset = labelDataOffset + labelTable.header.stringTableOffset + 8;
	labelTable.stringTable.infoEntries = new LabelTwoStrInfoEntry[labelTable.stringTable.header.totalCount];
	labelTable.stringTable.strings = new LabelStrEntry[labelTable.stringTable.header.totalCount];

	// LSB2's on Kinect Sports: Season 2 have an additional tag which messes up things.
	int32_t offsCheck = 0;
	bool isKSS2 = false;
	memcpy(&offsCheck, loctextPtr + strInfoOffset + 2, sizeof(int32_t));

	if (offsCheck != 0) {
		isKSS2 = true;
		dataSize = 8;
	}

	for (int32_t i = 0; i < labelTable.stringTable.header.totalCount; i++) {
		if (isKSS2) {
			memcpy(&labelTable.stringTable.infoEntries[i].unk, loctextPtr + strInfoOffset + (8 * i), sizeof(int16_t));
			memcpy(&labelTable.stringTable.infoEntries[i].hash, loctextPtr + strInfoOffset + (8 * i) + 2, sizeof(int16_t));
			memcpy(&labelTable.stringTable.infoEntries[i].offset, loctextPtr + strInfoOffset + (8 * i) + 4, sizeof(int32_t));
		}
		else {
			labelTable.stringTable.infoEntries[i].unk = 0;
			memcpy(&labelTable.stringTable.infoEntries[i].hash, loctextPtr + strInfoOffset + (6 * i), sizeof(int16_t));
			memcpy(&labelTable.stringTable.infoEntries[i].offset, loctextPtr + strInfoOffset + (6 * i) + 2, sizeof(int32_t));
		}

		if (endianness == SRC_ENDIANBIG) {
			labelTable.stringTable.infoEntries[i].unk = flipEndian(labelTable.stringTable.infoEntries[i].unk);
			labelTable.stringTable.infoEntries[i].hash = flipEndian(labelTable.stringTable.infoEntries[i].hash);
			labelTable.stringTable.infoEntries[i].offset = flipEndian(labelTable.stringTable.infoEntries[i].offset);
		}
		
		printf("String Table Entry %d: %04x %d\n", i, labelTable.stringTable.infoEntries[i].hash, labelTable.stringTable.infoEntries[i].offset);
	}

	int32_t strEntryBaseOffset = strInfoOffset + ((labelTable.stringTable.header.totalCount + 1) * dataSize);
	for (int32_t i = 0; i < labelTable.stringTable.header.totalCount; i++) {
		int32_t offs = strEntryBaseOffset + (labelTable.stringTable.infoEntries[i].offset * 2);

		wchar_t chr = 0xFFFF;
		int32_t idx = 0;
		if (endianness == SRC_ENDIANLITTLE) {
			while (chr != '\0') {
				memcpy(&chr, loctextPtr + offs, sizeof(char16_t));
				labelTable.stringTable.strings[i].string[idx] = chr;
				idx++;
				offs += 2;
			}
		}
		else if (endianness == SRC_ENDIANBIG) {
			while (chr != '\0') {
				memcpy(&chr, loctextPtr + offs, sizeof(char16_t));
				labelTable.stringTable.strings[i].string[idx] = flipEndian(chr);
				idx++;
				offs += 2;
			}
		}

		printf(u8"String Table Val %d: (Length %d) %ws", i, (int32_t)wcslen(labelTable.stringTable.strings[i].string), labelTable.stringTable.strings[i].string);
		printf("\n");
	}
}

void LocTwo::ReadTagData() {
	if (loctextPtr == NULL) {
		printf("Loctext::ReadTagData() called while loctextPtr is null.\n");
		return;
	}

	// Endianness doesn't matter here for some reason. All games that use this always treats these as little endian.
	memcpy(&labelTable.tagTable.header.totalSectLen, loctextPtr + labelDataOffset + labelTable.header.tagTableOffset, sizeof(int32_t));
	memcpy(&labelTable.tagTable.header.totalCount, loctextPtr + labelDataOffset + labelTable.header.tagTableOffset + 4, sizeof(int32_t));

	if (endianness == SRC_ENDIANBIG) {
		labelTable.tagTable.header.totalSectLen = flipEndian(labelTable.tagTable.header.totalSectLen);
		labelTable.tagTable.header.totalCount = flipEndian(labelTable.tagTable.header.totalCount);
	}

	int32_t tagTableOffset = labelDataOffset + labelTable.header.tagTableOffset + 8;
	labelTable.tagTable.infoEntries = new TagInfo[labelTable.tagTable.header.totalCount];
	labelTable.tagTable.tags = new TagStr[labelTable.tagTable.header.totalCount];
	for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
		memcpy(&labelTable.tagTable.infoEntries[i].id, loctextPtr + tagTableOffset + (8 * i), sizeof(int16_t));
		memcpy(&labelTable.tagTable.infoEntries[i].unk1, loctextPtr + tagTableOffset + (8 * i) + 2, sizeof(int16_t));
		memcpy(&labelTable.tagTable.infoEntries[i].offset, loctextPtr + tagTableOffset + (8 * i) + 4, sizeof(int32_t));

		if (endianness == SRC_ENDIANBIG) {
			labelTable.tagTable.infoEntries[i].id = flipEndian(labelTable.tagTable.infoEntries[i].id);
			labelTable.tagTable.infoEntries[i].unk1 = flipEndian(labelTable.tagTable.infoEntries[i].unk1);
			labelTable.tagTable.infoEntries[i].offset = flipEndian(labelTable.tagTable.infoEntries[i].offset);
		}

		printf("Tag Table Entry %d: %04x %04x %d\n", i, labelTable.tagTable.infoEntries[i].id, labelTable.tagTable.infoEntries[i].unk1, labelTable.tagTable.infoEntries[i].offset);
	}

	int32_t strEntryBaseOffset = tagTableOffset + (labelTable.stringTable.header.totalCount * 8);
	for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
		int32_t offs = strEntryBaseOffset + labelTable.tagTable.infoEntries[i].offset;

		strcpy(labelTable.tagTable.tags[i].val, loctextPtr + offs);

		printf("Tag Table Val %d: %s\n", i, labelTable.tagTable.tags[i].val);
	}
}

void LocTwo::ReadCommentData() {
	if (loctextPtr == NULL) {
		printf("Loctext::ReadCommentData() called while loctextPtr is null.\n");
		return;
	}

	memcpy(&labelTable.commentTable.header.totalSectLen, loctextPtr + labelDataOffset + labelTable.header.commentTableOffset, sizeof(int32_t));
	memcpy(&labelTable.commentTable.header.totalCount, loctextPtr + labelDataOffset + labelTable.header.commentTableOffset + 4, sizeof(int32_t));

	if (endianness == SRC_ENDIANBIG) {
		labelTable.commentTable.header.totalSectLen = flipEndian(labelTable.commentTable.header.totalSectLen);
		labelTable.commentTable.header.totalCount = flipEndian(labelTable.commentTable.header.totalCount);
	}

	int32_t commentTableOffset = labelDataOffset + labelTable.header.commentTableOffset + 8;
	labelTable.commentTable.entries = new CommentEntry[labelTable.commentTable.header.totalCount];
	labelTable.commentTable.comments = new CommentStr[labelTable.commentTable.header.totalCount];

	for (int32_t i = 0; i < labelTable.commentTable.header.totalCount; i++) {
		memcpy(&labelTable.commentTable.entries[i].unk1, loctextPtr + commentTableOffset + (8 * i), sizeof(char));
		memcpy(&labelTable.commentTable.entries[i].id, loctextPtr + commentTableOffset + (8 * i) + 1, sizeof(int16_t));
		memcpy(&labelTable.commentTable.entries[i].unk2, loctextPtr + commentTableOffset + (8 * i) + 3, sizeof(char));
		memcpy(&labelTable.commentTable.entries[i].offset, loctextPtr + commentTableOffset + (8 * i) + 4, sizeof(int32_t));

		if (endianness == SRC_ENDIANBIG) {
			labelTable.commentTable.entries[i].id = flipEndian(labelTable.commentTable.entries[i].id);
			labelTable.commentTable.entries[i].offset = flipEndian(labelTable.commentTable.entries[i].offset);
		}

		printf("Comment Table Entry %d: %04x %d\n", i, labelTable.commentTable.entries[i].id, labelTable.commentTable.entries[i].offset);
	}

	int32_t strEntryBaseOffset = commentTableOffset + (labelTable.commentTable.header.totalCount * 8);
	for (int32_t i = 0; i < labelTable.commentTable.header.totalCount; i++) {
		int32_t offs = strEntryBaseOffset + labelTable.commentTable.entries[i].offset;
		strcpy(labelTable.commentTable.comments[i].val, loctextPtr + offs);

		printf("Comment Table Val %d: %s", i, labelTable.commentTable.comments[i].val);
	}
}

void LocTwo::ReadPosData() {
	if (loctextPtr == NULL) {
		printf("Loctext::ReadPosData() called while loctextPtr is null.\n");
		return;
	}

	memcpy(&labelTable.posTable.header.totalSectLen, loctextPtr + labelDataOffset + labelTable.header.positionTableOffset, sizeof(int32_t));
	memcpy(&labelTable.posTable.header.totalCount, loctextPtr + labelDataOffset + labelTable.header.positionTableOffset + 4, sizeof(int32_t));

	if (endianness == SRC_ENDIANBIG) {
		labelTable.posTable.header.totalSectLen = flipEndian(labelTable.posTable.header.totalSectLen);
		labelTable.posTable.header.totalCount = flipEndian(labelTable.posTable.header.totalCount);
	}

	int32_t posTableOffset = labelDataOffset + labelTable.header.positionTableOffset + 8;
	labelTable.posTable.entries = new uint16_t[labelTable.posTable.header.totalCount];
	for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {

		memcpy(&labelTable.posTable.entries[i], loctextPtr + posTableOffset + (2 * i), sizeof(int16_t));

		if (endianness == SRC_ENDIANBIG) {
			labelTable.posTable.entries[i] = flipEndian(labelTable.posTable.entries[i]);
		}

		printf("Pos Table Entry %d: %04x\n", i, labelTable.posTable.entries[i]);
	}
}

void LocTwo::ExportToFile(char* fileName) {
	int32_t position = 0;

	std::ofstream is(fileName, std::ios_base::trunc);

	for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
		uint16_t idx = 0;

		// Check if the position table is present. If it is, get the value from that. If not then fallback on the tag table entries.
		if (labelTable.header.positionTableOffset != 0) {
			idx = labelTable.posTable.entries[i];
		}
		else {
			idx = labelTable.tagTable.infoEntries[i].id;
		}

		if (IsIdxConnectedToComment(idx)) {
			is.write(labelTable.commentTable.comments[GetIdxOfConnectedComment(idx)].val, strlen(labelTable.commentTable.comments[GetIdxOfConnectedComment(idx)].val));
		}

		if (IsIdxConnectedToTag(idx)) {
			char fullStr[4096];

			char conv[2048];

			int32_t total = wcstombs(conv, labelTable.stringTable.strings[GetIdxOfConnectedString(idx)].string, 2048);

			sprintf(fullStr, "%s\t\t\t= \"%s\"\n", labelTable.tagTable.tags[GetIdxOfConnectedTag(idx)].val, conv);

			is.write(fullStr, strlen(fullStr));
		}
	}

	if (IsIdxConnectedToComment(-1)) {
		is.write(labelTable.commentTable.comments[GetIdxOfConnectedComment(-1)].val, strlen(labelTable.commentTable.comments[GetIdxOfConnectedComment(-1)].val));
	}

	is.flush();
	is.close();
}
#pragma endregion

#pragma region Loctext Rev Turn 10 V3
void LocTurn10V3::ReadLoctext(char* data) {
	loctextPtr = data;

	try {
		ReadLabelData();

		// Kameo and Perfect Dark Zero don't use this table, so it needs to be checked beforehand.
		if (labelTable.header.tagTableOffset != 0) {
			ReadTagData();
		}
	}
	catch (int32_t ex) {
		printf("Loctext::ReadLoctext() - An error occured while reading the loctext file.\n");
	}
}

void LocTurn10V3::ReadLabelData() {
	if (loctextPtr == NULL) {
		printf("Loctext::ReadLabelData() called while loctextPtr is null.\n");
		return;
	}

	int16_t verCheck = 0;
	
	memcpy(&verCheck, loctextPtr, sizeof(int16_t));

	if ((verCheck & 0xFF00) == 3) {
		endianness = SRC_ENDIANBIG;
	}

	if (verCheck == 3) {
		endianness = SRC_ENDIANLITTLE;
	}

	if (endianness != SRC_ENDIANLITTLE && endianness != SRC_ENDIANBIG) {
		printf("Loctext::ReadLabelData() somehow ended up with an invalid endian value of %d.\n", endianness);
		return;
	}

	printf("Loctext::ReadLabelData() ENDIAN - %04x.\n", verCheck);

	memcpy(&labelTable.header.unk1, loctextPtr, 1);
	memcpy(&labelTable.header.version, loctextPtr + 1, 1);
	memcpy(&labelTable.header.numSects, loctextPtr + 2, 1);
	memcpy(&labelTable.header.unk2, loctextPtr + 3, 1);
	memcpy(&labelTable.header.stringTableOffset, loctextPtr + 4, sizeof(int32_t));
	memcpy(&labelTable.header.tagTableOffset, loctextPtr + 8, sizeof(int32_t));

	if (endianness == SRC_ENDIANBIG) {
		labelTable.header.stringTableOffset = flipEndian(labelTable.header.stringTableOffset);
		labelTable.header.tagTableOffset = flipEndian(labelTable.header.tagTableOffset);
	}

	usesTags = labelTable.header.tagTableOffset != 0;

	printf("Loctext File: %d %d\n", labelTable.header.stringTableOffset, labelTable.header.tagTableOffset);

	memcpy(&labelTable.stringTable.header.totalSectLen, loctextPtr + labelTable.header.stringTableOffset, sizeof(int32_t));
	memcpy(&labelTable.stringTable.header.totalStringSectLen, loctextPtr + labelTable.header.stringTableOffset + 4, sizeof(int32_t));
	memcpy(&labelTable.stringTable.header.totalCount, loctextPtr + labelTable.header.stringTableOffset + 8, sizeof(int32_t));

	if (endianness == SRC_ENDIANBIG) {
		labelTable.stringTable.header.totalSectLen = flipEndian(labelTable.stringTable.header.totalSectLen);
		labelTable.stringTable.header.totalStringSectLen = flipEndian(labelTable.stringTable.header.totalStringSectLen);
		labelTable.stringTable.header.totalCount = flipEndian(labelTable.stringTable.header.totalCount);
	}
	
	
	printf("String Table: %d %d\n", labelTable.stringTable.header.totalSectLen, labelTable.stringTable.header.totalCount);

	int32_t strInfoOffset = labelTable.header.stringTableOffset + 12;
	labelTable.stringTable.infoEntries = new LabelTwoStrInfoEntry[labelTable.stringTable.header.totalCount];
	labelTable.stringTable.strings = new LabelStrEntry[labelTable.stringTable.header.totalCount];

	for (int32_t i = 0; i < labelTable.stringTable.header.totalCount; i++) {
		memcpy(&labelTable.stringTable.infoEntries[i].hash, loctextPtr + strInfoOffset + (8 * i), sizeof(int16_t));
		memcpy(&labelTable.stringTable.infoEntries[i].unk, loctextPtr + strInfoOffset + (8 * i) + 2, sizeof(int16_t));
		memcpy(&labelTable.stringTable.infoEntries[i].offset, loctextPtr + strInfoOffset + (8 * i) + 4, sizeof(int32_t));

		if (endianness == SRC_ENDIANBIG) {
			labelTable.stringTable.infoEntries[i].hash = flipEndian(labelTable.stringTable.infoEntries[i].hash);
			labelTable.stringTable.infoEntries[i].unk = flipEndian(labelTable.stringTable.infoEntries[i].unk);
			labelTable.stringTable.infoEntries[i].offset = flipEndian(labelTable.stringTable.infoEntries[i].offset);
		}
		
		printf("String Table Entry %d: %04x, %04x %d\n", i, labelTable.stringTable.infoEntries[i].unk, labelTable.stringTable.infoEntries[i].hash, labelTable.stringTable.infoEntries[i].offset);
	}

	int32_t strEntryBaseOffset = strInfoOffset + (labelTable.stringTable.header.totalCount * 8);
	for (int32_t i = 0; i < labelTable.stringTable.header.totalCount; i++) {
		int32_t offs = strEntryBaseOffset + labelTable.stringTable.infoEntries[i].offset;

		wchar_t chr = 0xFFFF;
		int32_t idx = 0;
		if (endianness == SRC_ENDIANLITTLE) {
			while (chr != '\0') {
				memcpy(&chr, loctextPtr + offs, sizeof(char16_t));
				labelTable.stringTable.strings[i].string[idx] = chr;
				idx++;
				offs += 2;
			}
		}
		else if (endianness == SRC_ENDIANBIG) {
			while (chr != '\0') {
				memcpy(&chr, loctextPtr + offs, sizeof(char16_t));
				labelTable.stringTable.strings[i].string[idx] = flipEndian(chr);
				idx++;
				offs += 2;
			}
		}

		printf(u8"String Table Val %d: (Length %d) %ws", i, (int32_t)wcslen(labelTable.stringTable.strings[i].string), labelTable.stringTable.strings[i].string);
		printf("\n");
	}
}

void LocTurn10V3::ReadTagData() {
	if (loctextPtr == NULL) {
		printf("Loctext::ReadTagData() called while loctextPtr is null.\n");
		return;
	}

	// Endianness doesn't matter here for some reason. All games that use this always treats these as little endian.
	memcpy(&labelTable.tagTable.header.totalSectLen, loctextPtr + labelTable.header.tagTableOffset, sizeof(int32_t));
	memcpy(&labelTable.tagTable.header.totalStringSectLen, loctextPtr + labelTable.header.tagTableOffset + 4, sizeof(int32_t));
	memcpy(&labelTable.tagTable.header.totalCount, loctextPtr + labelTable.header.tagTableOffset + 8, sizeof(int32_t));

	if (endianness == SRC_ENDIANBIG) {
		labelTable.tagTable.header.totalSectLen = flipEndian(labelTable.tagTable.header.totalSectLen);
		labelTable.tagTable.header.totalStringSectLen = flipEndian(labelTable.tagTable.header.totalStringSectLen);
		labelTable.tagTable.header.totalCount = flipEndian(labelTable.tagTable.header.totalCount);
	}

	int32_t tagTableOffset = labelTable.header.tagTableOffset + 12;
	labelTable.tagTable.infoEntries = new TagInfo[labelTable.tagTable.header.totalCount];
	labelTable.tagTable.tags = new TagTurn10Str[labelTable.tagTable.header.totalCount];
	for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
		memcpy(&labelTable.tagTable.infoEntries[i].id, loctextPtr + tagTableOffset + (8 * i), sizeof(int16_t));
		memcpy(&labelTable.tagTable.infoEntries[i].unk1, loctextPtr + tagTableOffset + (8 * i) + 2, sizeof(int16_t));
		memcpy(&labelTable.tagTable.infoEntries[i].offset, loctextPtr + tagTableOffset + (8 * i) + 4, sizeof(int32_t));

		if (endianness == SRC_ENDIANBIG) {
			labelTable.tagTable.infoEntries[i].id = flipEndian(labelTable.tagTable.infoEntries[i].id);
			labelTable.tagTable.infoEntries[i].unk1 = flipEndian(labelTable.tagTable.infoEntries[i].unk1);
			labelTable.tagTable.infoEntries[i].offset = flipEndian(labelTable.tagTable.infoEntries[i].offset);
		}

		printf("Tag Table Entry %d: %04x %04x %d\n", i, labelTable.tagTable.infoEntries[i].id, labelTable.tagTable.infoEntries[i].unk1, labelTable.tagTable.infoEntries[i].offset);
	}

	int32_t strEntryBaseOffset = tagTableOffset + (labelTable.stringTable.header.totalCount * 8);
	for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
		int32_t offs = strEntryBaseOffset + labelTable.tagTable.infoEntries[i].offset;

		wchar_t chr = 0xFFFF;
		int32_t idx = 0;
		if (endianness == SRC_ENDIANLITTLE) {
			while (chr != '\0') {
				memcpy(&chr, loctextPtr + offs, sizeof(char16_t));
				labelTable.tagTable.tags[i].val[idx] = chr;
				idx++;
				offs += 2;
			}
		}
		else if (endianness == SRC_ENDIANBIG) {
			while (chr != '\0') {
				memcpy(&chr, loctextPtr + offs, sizeof(char16_t));
				labelTable.tagTable.tags[i].val[idx] = flipEndian(chr);
				idx++;
				offs += 2;
			}
		}

		printf(u8"Tag Table Val %d: (Length %d) %ws", i, (int32_t)wcslen(labelTable.tagTable.tags[i].val), labelTable.tagTable.tags[i].val);
		printf("\n");
	}
}

void LocTurn10V3::ExportToFile(char* fileName) {
	int32_t position = 0;

	std::ofstream is(fileName, std::ios_base::trunc);

	for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
		uint16_t idx = labelTable.tagTable.infoEntries[i].id;

		if (IsIdxConnectedToTag(idx)) {
			char fullStr[4096];

			char conv[2048];
			char tagConv[256];

			int32_t total = wcstombs(conv, labelTable.stringTable.strings[GetIdxOfConnectedString(idx)].string, 2048);
			int32_t total2 = wcstombs(tagConv, labelTable.tagTable.tags[GetIdxOfConnectedTag(idx)].val, 256);

			sprintf(fullStr, "%s\t\t\t= \"%s\"\n", tagConv, conv);

			is.write(fullStr, strlen(fullStr));
		}
	}

	is.flush();
	is.close();
}
#pragma endregion

#pragma region Loctext Rev Turn 10 V4
void LocTurn10V4::ReadLoctext(char* data) {
	loctextPtr = data;

	try {
		ReadLabelData();

		// Kameo and Perfect Dark Zero don't use this table, so it needs to be checked beforehand.
		if (labelTable.header.tagTableOffset != 0) {
			ReadTagData();
		}
	}
	catch (int32_t ex) {
		printf("Loctext::ReadLoctext() - An error occured while reading the loctext file.\n");
	}
}

void LocTurn10V4::ReadLabelData() {
	if (loctextPtr == NULL) {
		printf("Loctext::ReadLabelData() called while loctextPtr is null.\n");
		return;
	}

	int16_t verCheck = 0;
	
	memcpy(&verCheck, loctextPtr, sizeof(int16_t));

	if ((verCheck & 0xFF00) == 4) {
		endianness = SRC_ENDIANBIG;
	}

	if (verCheck == 4) {
		endianness = SRC_ENDIANLITTLE;
	}

	if (endianness != SRC_ENDIANLITTLE && endianness != SRC_ENDIANBIG) {
		printf("Loctext::ReadLabelData() somehow ended up with an invalid endian value of %d.\n", endianness);
		return;
	}

	printf("Loctext::ReadLabelData() ENDIAN - %04x.\n", verCheck);

	memcpy(&labelTable.header.unk1, loctextPtr, 1);
	memcpy(&labelTable.header.version, loctextPtr + 1, 1);
	memcpy(&labelTable.header.fileName, loctextPtr + 2, 0x80);
	memcpy(&labelTable.header.numSects, loctextPtr + 0x82, 1);
	memcpy(&labelTable.header.unk2, loctextPtr + 0x83, 1);
	memcpy(&labelTable.header.stringTableOffset, loctextPtr + 0x84, sizeof(int32_t));
	memcpy(&labelTable.header.tagTableOffset, loctextPtr + 0x88, sizeof(int32_t));

	if (endianness == SRC_ENDIANBIG) {
		labelTable.header.stringTableOffset = flipEndian(labelTable.header.stringTableOffset);
		labelTable.header.tagTableOffset = flipEndian(labelTable.header.tagTableOffset);
	}

	usesTags = labelTable.header.tagTableOffset != 0;

	printf("Loctext File: %d %d\n", labelTable.header.stringTableOffset, labelTable.header.tagTableOffset);

	memcpy(&labelTable.stringTable.header.totalSectLen, loctextPtr + labelTable.header.stringTableOffset, sizeof(int32_t));
	memcpy(&labelTable.stringTable.header.totalStringSectLen, loctextPtr + labelTable.header.stringTableOffset + 4, sizeof(int32_t));
	memcpy(&labelTable.stringTable.header.totalCount, loctextPtr + labelTable.header.stringTableOffset + 8, sizeof(int32_t));

	if (endianness == SRC_ENDIANBIG) {
		labelTable.stringTable.header.totalSectLen = flipEndian(labelTable.stringTable.header.totalSectLen);
		labelTable.stringTable.header.totalStringSectLen = flipEndian(labelTable.stringTable.header.totalStringSectLen);
		labelTable.stringTable.header.totalCount = flipEndian(labelTable.stringTable.header.totalCount);
	}
	
	
	printf("String Table: %d %d\n", labelTable.stringTable.header.totalSectLen, labelTable.stringTable.header.totalCount);

	int32_t strInfoOffset = labelTable.header.stringTableOffset + 12;
	labelTable.stringTable.infoEntries = new LabelTwoStrInfoEntry[labelTable.stringTable.header.totalCount];
	labelTable.stringTable.strings = new LabelStrEntry[labelTable.stringTable.header.totalCount];

	for (int32_t i = 0; i < labelTable.stringTable.header.totalCount; i++) {
		memcpy(&labelTable.stringTable.infoEntries[i].hash, loctextPtr + strInfoOffset + (8 * i), sizeof(int16_t));
		memcpy(&labelTable.stringTable.infoEntries[i].unk, loctextPtr + strInfoOffset + (8 * i) + 2, sizeof(int16_t));
		memcpy(&labelTable.stringTable.infoEntries[i].offset, loctextPtr + strInfoOffset + (8 * i) + 4, sizeof(int32_t));

		if (endianness == SRC_ENDIANBIG) {
			labelTable.stringTable.infoEntries[i].hash = flipEndian(labelTable.stringTable.infoEntries[i].hash);
			labelTable.stringTable.infoEntries[i].unk = flipEndian(labelTable.stringTable.infoEntries[i].unk);
			labelTable.stringTable.infoEntries[i].offset = flipEndian(labelTable.stringTable.infoEntries[i].offset);
		}
		
		printf("String Table Entry %d: %04x, %04x %d\n", i, labelTable.stringTable.infoEntries[i].unk, labelTable.stringTable.infoEntries[i].hash, labelTable.stringTable.infoEntries[i].offset);
	}

	int32_t strEntryBaseOffset = strInfoOffset + (labelTable.stringTable.header.totalCount * 8);
	for (int32_t i = 0; i < labelTable.stringTable.header.totalCount; i++) {
		int32_t offs = strEntryBaseOffset + labelTable.stringTable.infoEntries[i].offset;

		wchar_t chr = 0xFFFF;
		int32_t idx = 0;
		if (endianness == SRC_ENDIANLITTLE) {
			while (chr != '\0') {
				memcpy(&chr, loctextPtr + offs, sizeof(char16_t));
				labelTable.stringTable.strings[i].string[idx] = chr;
				idx++;
				offs += 2;
			}
		}
		else if (endianness == SRC_ENDIANBIG) {
			while (chr != '\0') {
				memcpy(&chr, loctextPtr + offs, sizeof(char16_t));
				labelTable.stringTable.strings[i].string[idx] = flipEndian(chr);
				idx++;
				offs += 2;
			}
		}

		printf(u8"String Table Val %d: (Length %d) %ws", i, (int32_t)wcslen(labelTable.stringTable.strings[i].string), labelTable.stringTable.strings[i].string);
		printf("\n");
	}
}

void LocTurn10V4::ReadTagData() {
	if (loctextPtr == NULL) {
		printf("Loctext::ReadTagData() called while loctextPtr is null.\n");
		return;
	}

	// Endianness doesn't matter here for some reason. All games that use this always treats these as little endian.
	memcpy(&labelTable.tagTable.header.totalSectLen, loctextPtr + labelTable.header.tagTableOffset, sizeof(int32_t));
	memcpy(&labelTable.tagTable.header.totalStringSectLen, loctextPtr + labelTable.header.tagTableOffset + 4, sizeof(int32_t));
	memcpy(&labelTable.tagTable.header.totalCount, loctextPtr + labelTable.header.tagTableOffset + 8, sizeof(int32_t));

	if (endianness == SRC_ENDIANBIG) {
		labelTable.tagTable.header.totalSectLen = flipEndian(labelTable.tagTable.header.totalSectLen);
		labelTable.tagTable.header.totalStringSectLen = flipEndian(labelTable.tagTable.header.totalStringSectLen);
		labelTable.tagTable.header.totalCount = flipEndian(labelTable.tagTable.header.totalCount);
	}

	int32_t tagTableOffset = labelTable.header.tagTableOffset + 12;
	labelTable.tagTable.infoEntries = new TagInfo[labelTable.tagTable.header.totalCount];
	labelTable.tagTable.tags = new TagTurn10Str[labelTable.tagTable.header.totalCount];
	for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
		memcpy(&labelTable.tagTable.infoEntries[i].id, loctextPtr + tagTableOffset + (8 * i), sizeof(int16_t));
		memcpy(&labelTable.tagTable.infoEntries[i].unk1, loctextPtr + tagTableOffset + (8 * i) + 2, sizeof(int16_t));
		memcpy(&labelTable.tagTable.infoEntries[i].offset, loctextPtr + tagTableOffset + (8 * i) + 4, sizeof(int32_t));

		if (endianness == SRC_ENDIANBIG) {
			labelTable.tagTable.infoEntries[i].id = flipEndian(labelTable.tagTable.infoEntries[i].id);
			labelTable.tagTable.infoEntries[i].unk1 = flipEndian(labelTable.tagTable.infoEntries[i].unk1);
			labelTable.tagTable.infoEntries[i].offset = flipEndian(labelTable.tagTable.infoEntries[i].offset);
		}

		printf("Tag Table Entry %d: %04x %04x %d\n", i, labelTable.tagTable.infoEntries[i].id, labelTable.tagTable.infoEntries[i].unk1, labelTable.tagTable.infoEntries[i].offset);
	}

	int32_t strEntryBaseOffset = tagTableOffset + (labelTable.stringTable.header.totalCount * 8);
	for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
		int32_t offs = strEntryBaseOffset + labelTable.tagTable.infoEntries[i].offset;

		wchar_t chr = 0xFFFF;
		int32_t idx = 0;
		if (endianness == SRC_ENDIANLITTLE) {
			while (chr != '\0') {
				memcpy(&chr, loctextPtr + offs, sizeof(char16_t));
				labelTable.tagTable.tags[i].val[idx] = chr;
				idx++;
				offs += 2;
			}
		}
		else if (endianness == SRC_ENDIANBIG) {
			while (chr != '\0') {
				memcpy(&chr, loctextPtr + offs, sizeof(char16_t));
				labelTable.tagTable.tags[i].val[idx] = flipEndian(chr);
				idx++;
				offs += 2;
			}
		}

		printf(u8"Tag Table Val %d: (Length %d) %ws", i, (int32_t)wcslen(labelTable.tagTable.tags[i].val), labelTable.tagTable.tags[i].val);
		printf("\n");
	}
}

void LocTurn10V4::ExportToFile(char* fileName) {
	int32_t position = 0;

	std::ofstream is(fileName, std::ios_base::trunc);

	for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
		uint16_t idx = labelTable.tagTable.infoEntries[i].id;

		if (IsIdxConnectedToTag(idx)) {
			char fullStr[4096];

			char conv[2048];
			char tagConv[256];

			int32_t total = wcstombs(conv, labelTable.stringTable.strings[GetIdxOfConnectedString(idx)].string, 2048);
			int32_t total2 = wcstombs(tagConv, labelTable.tagTable.tags[GetIdxOfConnectedTag(idx)].val, 256);

			sprintf(fullStr, "%s\t\t\t= \"%s\"\n", tagConv, conv);

			is.write(fullStr, strlen(fullStr));
		}
	}

	is.flush();
	is.close();
}
#pragma endregion

#pragma region Loctext Rev Turn 10 V8
void LocTurn10V8::ReadLoctext(char* data) {
	loctextPtr = data;

	try {
		ReadLabelData();

		// Kameo and Perfect Dark Zero don't use this table, so it needs to be checked beforehand.
		if (labelTable.header.tagTableOffset != 0) {
			ReadTagData();
		}
	}
	catch (int32_t ex) {
		printf("Loctext::ReadLoctext() - An error occured while reading the loctext file.\n");
	}
}

void LocTurn10V8::ReadLabelData() {
	if (loctextPtr == NULL) {
		printf("Loctext::ReadLabelData() called while loctextPtr is null.\n");
		return;
	}

	int16_t verCheck = 0;
	
	memcpy(&verCheck, loctextPtr, sizeof(int16_t));

	if ((verCheck & 0xFF00) == 4) {
		endianness = SRC_ENDIANBIG;
	}

	if (verCheck == 4) {
		endianness = SRC_ENDIANLITTLE;
	}

	if (endianness != SRC_ENDIANLITTLE && endianness != SRC_ENDIANBIG) {
		printf("Loctext::ReadLabelData() somehow ended up with an invalid endian value of %d.\n", endianness);
		return;
	}

	printf("Loctext::ReadLabelData() ENDIAN - %04x.\n", verCheck);

	memcpy(&labelTable.header.unk1, loctextPtr, 1);
	memcpy(&labelTable.header.version, loctextPtr + 1, 1);
	memcpy(&labelTable.header.fileName, loctextPtr + 2, 0x80);
	memcpy(&labelTable.header.numSects, loctextPtr + 0x82, 1);
	memcpy(&labelTable.header.unk2, loctextPtr + 0x83, 1);
	memcpy(&labelTable.header.stringTableOffset, loctextPtr + 0x84, sizeof(int32_t));
	memcpy(&labelTable.header.tagTableOffset, loctextPtr + 0x88, sizeof(int32_t));

	if (endianness == SRC_ENDIANBIG) {
		labelTable.header.stringTableOffset = flipEndian(labelTable.header.stringTableOffset);
		labelTable.header.tagTableOffset = flipEndian(labelTable.header.tagTableOffset);
	}

	usesTags = labelTable.header.tagTableOffset != 0;

	printf("Loctext File: %d %d\n", labelTable.header.stringTableOffset, labelTable.header.tagTableOffset);

	memcpy(&labelTable.stringTable.header.totalSectLen, loctextPtr + labelTable.header.stringTableOffset, sizeof(int32_t));
	memcpy(&labelTable.stringTable.header.totalStringSectLen, loctextPtr + labelTable.header.stringTableOffset + 4, sizeof(int32_t));
	memcpy(&labelTable.stringTable.header.totalCount, loctextPtr + labelTable.header.stringTableOffset + 8, sizeof(int32_t));

	if (endianness == SRC_ENDIANBIG) {
		labelTable.stringTable.header.totalSectLen = flipEndian(labelTable.stringTable.header.totalSectLen);
		labelTable.stringTable.header.totalStringSectLen = flipEndian(labelTable.stringTable.header.totalStringSectLen);
		labelTable.stringTable.header.totalCount = flipEndian(labelTable.stringTable.header.totalCount);
	}
	
	
	printf("String Table: %d %d\n", labelTable.stringTable.header.totalSectLen, labelTable.stringTable.header.totalCount);

	int32_t strInfoOffset = labelTable.header.stringTableOffset + 12;
	labelTable.stringTable.infoEntries = new LabelTwoStrInfoEntry[labelTable.stringTable.header.totalCount];
	labelTable.stringTable.strings = new LabelTurn10V8StrEntry[labelTable.stringTable.header.totalCount];

	for (int32_t i = 0; i < labelTable.stringTable.header.totalCount; i++) {
		memcpy(&labelTable.stringTable.infoEntries[i].hash, loctextPtr + strInfoOffset + (8 * i), sizeof(int16_t));
		memcpy(&labelTable.stringTable.infoEntries[i].unk, loctextPtr + strInfoOffset + (8 * i) + 2, sizeof(int16_t));
		memcpy(&labelTable.stringTable.infoEntries[i].offset, loctextPtr + strInfoOffset + (8 * i) + 4, sizeof(int32_t));

		if (endianness == SRC_ENDIANBIG) {
			labelTable.stringTable.infoEntries[i].hash = flipEndian(labelTable.stringTable.infoEntries[i].hash);
			labelTable.stringTable.infoEntries[i].unk = flipEndian(labelTable.stringTable.infoEntries[i].unk);
			labelTable.stringTable.infoEntries[i].offset = flipEndian(labelTable.stringTable.infoEntries[i].offset);
		}
		
		printf("String Table Entry %d: %04x, %04x %d\n", i, labelTable.stringTable.infoEntries[i].unk, labelTable.stringTable.infoEntries[i].hash, labelTable.stringTable.infoEntries[i].offset);
	}

	int32_t strEntryBaseOffset = strInfoOffset + (labelTable.stringTable.header.totalCount * 8);
	for (int32_t i = 0; i < labelTable.stringTable.header.totalCount; i++) {
		int32_t offs = strEntryBaseOffset + labelTable.stringTable.infoEntries[i].offset;

		wchar_t chr = 0xFFFF;
		int32_t idx = 0;
		
		strcpy(labelTable.stringTable.strings[i].string, loctextPtr + offs);

		printf(u8"String Table Val %d: (Length %d) %s", i, (int32_t)strlen(labelTable.stringTable.strings[i].string), labelTable.stringTable.strings[i].string);
		printf("\n");
	}
}

void LocTurn10V8::ReadTagData() {
	if (loctextPtr == NULL) {
		printf("Loctext::ReadTagData() called while loctextPtr is null.\n");
		return;
	}

	// Endianness doesn't matter here for some reason. All games that use this always treats these as little endian.
	memcpy(&labelTable.tagTable.header.totalSectLen, loctextPtr + labelTable.header.tagTableOffset, sizeof(int32_t));
	memcpy(&labelTable.tagTable.header.totalStringSectLen, loctextPtr + labelTable.header.tagTableOffset + 4, sizeof(int32_t));
	memcpy(&labelTable.tagTable.header.totalCount, loctextPtr + labelTable.header.tagTableOffset + 8, sizeof(int32_t));

	if (endianness == SRC_ENDIANBIG) {
		labelTable.tagTable.header.totalSectLen = flipEndian(labelTable.tagTable.header.totalSectLen);
		labelTable.tagTable.header.totalStringSectLen = flipEndian(labelTable.tagTable.header.totalStringSectLen);
		labelTable.tagTable.header.totalCount = flipEndian(labelTable.tagTable.header.totalCount);
	}

	int32_t tagTableOffset = labelTable.header.tagTableOffset + 12;
	labelTable.tagTable.infoEntries = new TagInfo[labelTable.tagTable.header.totalCount];
	labelTable.tagTable.tags = new TagStr[labelTable.tagTable.header.totalCount];
	for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
		memcpy(&labelTable.tagTable.infoEntries[i].id, loctextPtr + tagTableOffset + (8 * i), sizeof(int16_t));
		memcpy(&labelTable.tagTable.infoEntries[i].unk1, loctextPtr + tagTableOffset + (8 * i) + 2, sizeof(int16_t));
		memcpy(&labelTable.tagTable.infoEntries[i].offset, loctextPtr + tagTableOffset + (8 * i) + 4, sizeof(int32_t));

		if (endianness == SRC_ENDIANBIG) {
			labelTable.tagTable.infoEntries[i].id = flipEndian(labelTable.tagTable.infoEntries[i].id);
			labelTable.tagTable.infoEntries[i].unk1 = flipEndian(labelTable.tagTable.infoEntries[i].unk1);
			labelTable.tagTable.infoEntries[i].offset = flipEndian(labelTable.tagTable.infoEntries[i].offset);
		}

		printf("Tag Table Entry %d: %04x %04x %d\n", i, labelTable.tagTable.infoEntries[i].id, labelTable.tagTable.infoEntries[i].unk1, labelTable.tagTable.infoEntries[i].offset);
	}

	int32_t strEntryBaseOffset = tagTableOffset + (labelTable.stringTable.header.totalCount * 8);
	for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
		int32_t offs = strEntryBaseOffset + labelTable.tagTable.infoEntries[i].offset;

		strcpy(labelTable.tagTable.tags[i].val, loctextPtr + offs);

		printf("Tag Table Val %d: %s\n", i, labelTable.tagTable.tags[i].val);
	}
}

void LocTurn10V8::ExportToFile(char* fileName) {
	int32_t position = 0;

	std::ofstream is(fileName, std::ios_base::trunc);

	for (int32_t i = 0; i < labelTable.tagTable.header.totalCount; i++) {
		uint16_t idx = labelTable.tagTable.infoEntries[i].id;

		if (IsIdxConnectedToTag(idx)) {
			char fullStr[4096];

			sprintf(fullStr, "%s\t\t\t= \"%s\"\n", labelTable.tagTable.tags[GetIdxOfConnectedTag(idx)].val, labelTable.stringTable.strings[GetIdxOfConnectedString(idx)].string);

			is.write(fullStr, strlen(fullStr));
		}
	}

	is.flush();
	is.close();
}
#pragma endregion

LoctextFile::~LoctextFile()
{
	switch (currentlyLoadedLoctext)
	{
	case Loc1:
		{
			loc1File.ClearActiveData();
		}
	}
}

void LoctextFile::ParseLoctextData(char* data) {
	if (data == nullptr) return;

	// early check
	if (data[0] == LOCTEXT_LSBL[0] && data[1] == LOCTEXT_LSBL[1] && data[2] == LOCTEXT_LSBL[2] && data[3] == LOCTEXT_LSBL[3]) {
		printf("Reading LSBL");
		loc1File.ReadLoctext(data);
		currentlyLoadedLoctext = Loc1;
		return;
	}

	if (data[0] == LOCTEXT_LBSL[0] && data[1] == LOCTEXT_LBSL[1] && data[2] == LOCTEXT_LBSL[2] && data[3] == LOCTEXT_LBSL[3]) {
		printf("Reading LBSL");
		loc1File.ReadLoctext(data);
		currentlyLoadedLoctext = Loc1;
		return;
	}

	if (data[0] == LOCTEXT_LSB2[0] && data[1] == LOCTEXT_LSB2[1] && data[2] == LOCTEXT_LSB2[2] && data[3] == LOCTEXT_LSB2[3]) {
		printf("Reading LSB2");
		loc2File.ReadLoctext(data);
		currentlyLoadedLoctext = Loc2;
		return;
	}

	if (data[0] == LOCTEXT_LBS2[0] && data[1] == LOCTEXT_LBS2[1] && data[2] == LOCTEXT_LBS2[2] && data[3] == LOCTEXT_LBS2[3]) {
		printf("Reading LBS2");
		loc2File.ReadLoctext(data);
		currentlyLoadedLoctext = Loc2;
		return;
	}
	
	char verCheck = 0;
	
	memcpy(&verCheck, data + 1, 1);
	
	if (verCheck == 3)
	{
		locTurn10V3File.ReadLoctext(data);
		currentlyLoadedLoctext = LocT10V3;
		return;
	}
	
	if (verCheck == 4)
	{
		locTurn10V4File.ReadLoctext(data);
		currentlyLoadedLoctext = LocT10V4;
		return;
	}
	
	if (verCheck == 8)
	{
		locTurn10V8File.ReadLoctext(data);
		currentlyLoadedLoctext = LocT10V8;
		return;
	}
	
	int32_t headOffset = 0;

	memcpy(&headOffset, data, sizeof(int32_t));

	if (data[0] == 't' && data[1] == 'e' && data[2] == 'x' && data[3] == 't'){
		memcpy(&headOffset, data + 0x14, sizeof(int32_t));
	}

	if ((headOffset & 0x00000000FF) == 0) {
		headOffset = flipEndian(headOffset);
	}

	if (data[headOffset] == LOCTEXT_LSBL[0] && data[headOffset + 1] == LOCTEXT_LSBL[1] && data[headOffset + 2] == LOCTEXT_LSBL[2] && data[headOffset + 3] == LOCTEXT_LSBL[3]) {
		printf("Reading LSBL");
		loc1File.ReadLoctext(data);
		currentlyLoadedLoctext = Loc1;
		return;
	}

	if (data[headOffset] == LOCTEXT_LBSL[0] && data[headOffset + 1] == LOCTEXT_LBSL[1] && data[headOffset + 2] == LOCTEXT_LBSL[2] && data[headOffset + 3] == LOCTEXT_LBSL[3]) {
		printf("Reading LSBL");
		loc1File.ReadLoctext(data);
		currentlyLoadedLoctext = Loc1;
		return;
	}

	if (data[headOffset] == LOCTEXT_LSB2[0] && data[headOffset + 1] == LOCTEXT_LSB2[1] && data[headOffset + 2] == LOCTEXT_LSB2[2] && data[headOffset + 3] == LOCTEXT_LSB2[3]) {
		printf("Reading LSB2");
		loc2File.ReadLoctext(data);
		currentlyLoadedLoctext = Loc2;
		return;
	}

	if (data[headOffset] == LOCTEXT_LBS2[0] && data[headOffset + 1] == LOCTEXT_LBS2[1] && data[headOffset + 2] == LOCTEXT_LBS2[2] && data[headOffset + 3] == LOCTEXT_LBS2[3]) {
		printf("Reading LSB2");
		loc2File.ReadLoctext(data);
		currentlyLoadedLoctext = Loc2;
		return;
	}

	printf("No text file was found.\n");
}

void LoctextFile::ExportToFileRaw(char* fileName) {
	int32_t position = 0;

	std::ofstream is(fileName, std::ios_base::trunc);

	if (GetStringCount() == 0) {
		printf("LOCTEXT IS EMPTY.\n");
		is.flush();
		is.close();
		return;
	}

	if (!GetIsUsingTags()) {
		printf("LOCTEXT HAS NO TAGS.\n");
		for (int32_t i = 0; i < GetStringCount(); i++) {

			char fullStr[4096];

			char conv[2048];
			
			if (currentlyLoadedLoctext != LocT10V8)
			{
				int32_t total = wcstombs(conv, GetStringPtr(i), 2048);
			}
			
			if (currentlyLoadedLoctext == LocT10V8)
			{
				sprintf(fullStr, "%s\n", (char*)GetStringPtr(i));
			}
			else
			{
				sprintf(fullStr, "%s\n", conv);
			}

			is.write(fullStr, strlen(fullStr));
		}
	}

	if (GetIsUsingTags()) {
		printf("LOCTEXT HAS TAGS.\n");
		for (int32_t i = 0; i < GetStringCount(); i++) {
			uint16_t idx = GetPosPtr(i);

			// Some comments have new line characters while others don't.
			bool hasCommentGotNewLine = true;

			if (IsIdxConnectedToComment(idx)) {
				is.write(GetCommentPtr(IsIdxConnectedToComment(idx)), strlen(GetCommentPtr(IsIdxConnectedToComment(idx))));

				// Check if the comment has an new line character.
				hasCommentGotNewLine = strchr(GetCommentPtr(IsIdxConnectedToComment(idx)), '\n') != nullptr;
			}

			if (IsHashConnectedToTag(idx)) {
				char fullStr[4096];

				char conv[2048];
				char tagConv[256];

				if (currentlyLoadedLoctext != LocT10V8)
				{
					int32_t total = wcstombs(conv, GetStringPtr(GetIdxOfConnectedString(idx)), 2048);
				}
				else
				{
					strcpy(conv, (char*)GetStringPtr(GetIdxOfConnectedString(idx)));
				}
				
				if (currentlyLoadedLoctext == LocT10V3 || currentlyLoadedLoctext == LocT10V4)
				{
					int32_t total = wcstombs(tagConv, GetWideTagPtr(GetIdxOfConnectedTag(idx)), 256);
				}
				else
				{
					strcpy(tagConv, GetTagPtr(GetIdxOfConnectedTag(idx)));
				}

				// If a comment doesn't have a new line, apply one before we write our next value. Otherwise just write as normal.
				if (hasCommentGotNewLine) {
					sprintf(fullStr, "%s\t\t\t= \"%s\"\n", tagConv, conv);
				}
				else {
					sprintf(fullStr, "\n%s\t\t\t= \"%s\"\n", tagConv, conv);
				}

				is.write(fullStr, strlen(fullStr));
			}
		}
	}

	if (IsIdxConnectedToComment(-1)) {
		is.write(GetCommentPtr(GetIdxOfConnectedComment(-1)), strlen(GetCommentPtr(GetIdxOfConnectedComment(-1))));
	}

	is.flush();
	is.close();
}