#include "CommonReader.h"

#ifdef _WIN32 
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "CaffFileTypes.h"

#pragma region Script
void Script::ReadScript(char* data) {
	scriptPtr = data;

	if (scriptPtr == NULL) {
		printf("Script::ReadScript() called while scriptPtr is null.\n");
		return;
	}
	int32_t pos = 0;

	int32_t count = 0;

	// First, get the entire count of entries. We need to go through all the entries first in order to get that.
	int32_t size = 0;
	int32_t type = 0xFFFFFFFF;

	int32_t indention = 0;
	while (type != dbScript_Null) {
		memcpy(&size, scriptPtr + pos, sizeof(int32_t));
		memcpy(&type, scriptPtr + pos + 4, sizeof(int32_t));

		size = flipEndian(size);
		type = flipEndian(type);

		switch (type) {
		    case dbScript_Condition_End:
			case dbScript_Logic_Else:
			case dbScript_Logic_Endif: {
				indention--;
		    }
		}

		for (int32_t i = 0; i < indention; i++) {
			printf("\t");
		}

		switch (type) {
		    case dbScript_Condition_TimeOut:
		    case dbScript_Condition_NumberGhouliesAlive:
		    case dbScript_Condition_ObjectPickedUp:
			case dbScript_Condition_NumberGhouliesKilled:
			case dbScript_Condition_NumberOfKnockdowns:
			case dbScript_Condition_ActorEntersRegion:
			case dbScript_Condition_PlayerEntersRegion:
			case dbScript_Condition_PlayerLeavesRegion:
			case dbScript_Condition_SpecificGhouliesAlive:
			case dbScript_Condition_SpecificGhouliesKilled:
			case dbScript_Condition_ObjectRemoved:
			case dbScript_Condition_PlayerDeflectionContact:
			case dbScript_Condition_PlayerHitGhouly:
			case dbScript_Condition_GhoulyTriggered:
			case dbScript_Condition_GhoulyExists:
			case dbScript_Condition_PlayerTalksToNPC:
			case dbScript_Condition_PlayerEntersVehicle:
			case dbScript_Logic_IfGameflag:
			case dbScript_Logic_IfChallengeResult:
			case dbScript_Logic_IfDialogResponse:
			case dbScript_Logic_IfNumObjectsFound:
			case dbScript_Logic_IfNoVehiclesInWorld:
			case dbScript_Condition_ObjectEntersRegion:
			case dbScript_Condition_CounterGreaterThan:
			case dbScript_Logic_IfRandomFloatLessThanOrEqualsProb:
			case dbScript_Logic_Else: {
			    indention++;
		    }
		}

		printf("Script Entry %d = <%d - %s>", count, size, dbScriptNames[type]);
		switch (type) {
		    case dbScript_Debug_Printf: {
				char msg[256];
				memset(msg, 0, 256);

				strncpy_s(msg, 256, scriptPtr + pos + 8, 256);

				printf(" (%s)\n", msg);
		    }
			break;
			case dbScript_Condition_IfGameFlag:
			case dbScript_Logic_IfGameflag:
			case dbScript_Condition_CounterGreaterThan:
			case dbScript_Flow_WaitOnGameFlag: {
				char flag[0x40];
				int32_t compVal = 0;
				memset(flag, 0, 0x40);

				strncpy_s(flag, 0x40, scriptPtr + pos + 8, 0x40);

				memcpy(&compVal, scriptPtr + pos + 0x48, sizeof(int32_t));

				compVal = flipEndian(compVal);

				printf(" (%s - %d)\n", flag, compVal);
			}
			break;
			case dbScript_Play_Dialog:
			case dbScript_Execute_ScriptAsset: {
				uint32_t aid = 0;

				memcpy(&aid, scriptPtr + pos + 0x8, sizeof(int32_t));

				aid = flipEndian(aid);

				printf(" (%08X)\n", aid);
			}
			break;
			case dbScript_Logic_IfRandomFloatLessThanOrEqualsProb: {
				float prob = 0;

				memcpy(&prob, scriptPtr + pos + 0x8, sizeof(int32_t));

				prob = flipEndian_f32((char*)&prob, SRC_ENDIANBIG);

				printf(" (%f)\n", prob);
			}
			break;
			default: {
				printf("\n");
			}
			break;
		}
		

		pos += size;
		count++;
	}

	printf("Script Total Count: %d\n", count);
}
#pragma endregion

#pragma region Loctext
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

		// The only section that cares about endianness is the label data. Every other section treats it as little endian.
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

	int32_t endiannes = -1;
	int32_t dataOffs = 0;
	int32_t dataSize = 6;

	if (labelTable.header.magic == LOCTEXT_LBSL_MAGIC) {
		endiannes = SRC_ENDIANLITTLE;
	}

	if (labelTable.header.magic == LOCTEXT_LSBL_MAGIC) {
		endiannes = SRC_ENDIANBIG;
	}

	if (labelTable.header.magic == LOCTEXT_LBSTWO_MAGIC) {
		endiannes = SRC_ENDIANLITTLE;
		dataOffs = 8;
		if (labelTable.header.tagTableOffset == 0) {
			dataSize = 8;
		}
	}

	if (labelTable.header.magic == LOCTEXT_LSBTWO_MAGIC) {
		endiannes = SRC_ENDIANBIG;
		dataOffs = 8;
		if (labelTable.header.tagTableOffset == 0) {
			dataSize = 8;
		}
	}

	if (endiannes != SRC_ENDIANLITTLE && endiannes != SRC_ENDIANBIG) {
		printf("Loctext::ReadLabelData() somehow ended up with an invalid endian value of %d.\n", endiannes);
		return;
	}

	printf("Loctext::ReadLabelData() ENDIAN - %08x.\n", labelTable.header.magic);

	if (endiannes == SRC_ENDIANLITTLE) {
		memcpy(&labelTable.header.headerLen, loctextPtr + labelDataOffset + 4 + dataOffs, sizeof(int32_t));
		memcpy(&labelTable.header.entryTotal, loctextPtr + labelDataOffset + 8 + dataOffs, sizeof(int32_t));
		memcpy(&labelTable.header.stringTableOffset, loctextPtr + labelDataOffset + 0xC + dataOffs, sizeof(int32_t));
		memcpy(&labelTable.header.tagTableOffset, loctextPtr + labelDataOffset + 0x10 + dataOffs, sizeof(int32_t));
		memcpy(&labelTable.header.commentTableOffset, loctextPtr + labelDataOffset + 0x14 + dataOffs, sizeof(int32_t));
		memcpy(&labelTable.header.positionTableOffset, loctextPtr + labelDataOffset + 0x18 + dataOffs, sizeof(int32_t));

		memcpy(&labelTable.stringTable.header.totalSectLen, loctextPtr + labelDataOffset + labelTable.header.stringTableOffset, sizeof(int32_t));
		memcpy(&labelTable.stringTable.header.totalCount, loctextPtr + labelDataOffset + labelTable.header.stringTableOffset + 4, sizeof(int32_t));
	}
	else if (endiannes == SRC_ENDIANBIG) {
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
		if (endiannes == SRC_ENDIANLITTLE) {
			memcpy(&labelTable.stringTable.infoEntries[i].hash, loctextPtr + strInfoOffset + (6 * i), sizeof(int16_t));
			memcpy(&labelTable.stringTable.infoEntries[i].offset, loctextPtr + strInfoOffset + (6 * i) + 2, sizeof(int32_t));
		}
		else if (endiannes == SRC_ENDIANBIG) {
			uint16_t strUnkVal; // 0x0 (Only on LSB2 W/ no Tag Table.)
			uint16_t strIdVar; // 0x0 (0x2 on LSB2 W/ no Tag Table.)
			int32_t strOffsetVar; // 0x2 (0x4 on LSB2 W/ no Tag Table.)

			if (labelTable.header.magic == LOCTEXT_LSBTWO_MAGIC) {
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
			}

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
		if (endiannes == SRC_ENDIANLITTLE) {
			while (chr != '\0') {
				memcpy(&chr, loctextPtr + offs, sizeof(char16_t));
				labelTable.stringTable.strings[i].string[idx] = chr;
				idx++;
				offs += 2;
			}
		}
		else if (endiannes == SRC_ENDIANBIG) {
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

	bool doesEndMatter = false;

	if (labelTable.header.magic == LOCTEXT_LSBTWO_MAGIC) {
		doesEndMatter = true;
	}

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

void LocTwo::ReadCommentData() {
	if (loctextPtr == NULL) {
		printf("Loctext::ReadCommentData() called while loctextPtr is null.\n");
		return;
	}

	bool doesEndMatter = false;

	if (labelTable.header.magic == LOCTEXT_LSBTWO_MAGIC) {
		doesEndMatter = true;
	}

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

void LocTwo::ReadPosData() {
	if (loctextPtr == NULL) {
		printf("Loctext::ReadPosData() called while loctextPtr is null.\n");
		return;
	}

	bool doesEndMatter = false;

	if (labelTable.header.magic == LOCTEXT_LSBTWO_MAGIC) {
		doesEndMatter = true;
	}

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
#pragma endregion

void Vehicle::WriteHeaderFile(char* fileName) {
	// Generate a header file, exclusively for Xenia compatability.
	char headerPath[1024];
	memset(headerPath, 0, 1024);

	sprintf(headerPath, "%s%s", fileName, ".header");

	char filename[0x38];
	memset(filename, 0, 0x38);

	sprintf(filename, "0x000000FF"); // for the header file.

	wchar_t vehicleName[0x100];
	memset(vehicleName, 0, 0x100);

	// If the name type is 1, we need to save the data as wchar_t. Otherwise just write it as a char string.
	if (nameType == 1) {
		swprintf(vehicleName, 0x100, L"VEHICLE: %ls", vehicleUnicodeName);
	}
	else if (nameType == 0) {
		swprintf(vehicleName, 0x100, L"VEHICLE: %s", vehicleUnicodeName);
	}

	FILE* writeHeadStrm = fopen(headerPath, "wb");

	int32_t unk1 = flipEndian(1);
	int32_t unk2 = flipEndian(1);
	int32_t unk3 = 0;

	fwrite(&unk1, sizeof(int32_t), 1, writeHeadStrm);
	fwrite(&unk2, sizeof(int32_t), 1, writeHeadStrm);

	for (int32_t i = 0; i < 0x80; i++) {
		wchar_t flippedChar = flipEndian(vehicleName[i]);
		fwrite(&flippedChar, sizeof(wchar_t), 1, writeHeadStrm);
	}

	fwrite(&filename, sizeof(char), 0x38, writeHeadStrm);

	fwrite(&VEHICLE_HEAD_GAMEID, sizeof(uint32_t), 1, writeHeadStrm);
	fwrite(&unk3, sizeof(int32_t), 1, writeHeadStrm);

	fflush(writeHeadStrm);
	fclose(writeHeadStrm);
}

char* Vehicle::WriteToArray() {
	char* buf = new char[0x7C * (0x24 * numOfParts)];
	memset(buf, 0, 0x7C * (0x24 * numOfParts));
	int32_t position = 0;

	uint16_t numOfPartsVal = flipEndian(numOfParts);
	int16_t unk1Val = flipEndian(unk1);
	float preloadPowerVal = flipEndian_f32((char*)&preloadPower, SRC_ENDIANLITTLE);
	float preloadFuelVal = flipEndian_f32((char*)&preloadFuel, SRC_ENDIANLITTLE);
	float preloadAmmoVal = flipEndian_f32((char*)&preloadAmmo, SRC_ENDIANLITTLE);
	float preloadWeightVal = flipEndian_f32((char*)&preloadWeight, SRC_ENDIANLITTLE);
	float preloadBlocksUsedVal = flipEndian_f32((char*)&preloadBlocksUsed, SRC_ENDIANLITTLE);
	int32_t unk2Val = flipEndian(unk2);
	int32_t unk3Val = flipEndian(unk3);

	memcpy(buf, &numOfPartsVal, sizeof(int16_t));
	memcpy(buf + 2, &unk1Val, sizeof(int16_t));
	memcpy(buf + 4,  &preloadPowerVal, sizeof(float));
	memcpy(buf + 8, &preloadFuelVal, sizeof(float));
	memcpy(buf + 0xC, &preloadAmmoVal, sizeof(float));
	memcpy(buf + 0x10, &preloadWeightVal, sizeof(float));
	memcpy(buf + 0x14, &preloadBlocksUsedVal, sizeof(float));
	memcpy(buf + 0x18, &unk2Val, sizeof(int32_t));
	memcpy(buf + 0x1C, &unk3Val, sizeof(int32_t));

	// If the name type is 1, we need to save the data as wchar_t. Otherwise just write it as a char string.
	if (nameType == 1) {
		for (int32_t i = 0; i < 0x20; i++) {
			wchar_t flippedChar = flipEndian(vehicleUnicodeName[i]);
			memcpy(buf + 0x20 + (i * 2), &flippedChar, sizeof(wchar_t));
		}
	}
	else if (nameType == 0) {
		memcpy(buf + 0x20, vehicleName, 0x40);
	}

	uint32_t aButtonAssignmentVal = flipEndian(aButtonAssignment);
	uint32_t bButtonAssignmentVal = flipEndian(bButtonAssignment);
	uint32_t xButtonAssignmentVal = flipEndian(xButtonAssignment);

	int32_t unk4Val = flipEndian(unk4);
	int32_t unk5Val = flipEndian(unk5);
	int32_t unk6Val = flipEndian(unk6);

	memcpy(buf + 0x60, &aButtonAssignmentVal, sizeof(int32_t));
	memcpy(buf + 0x64, &bButtonAssignmentVal, sizeof(int32_t));
	memcpy(buf + 0x68, &xButtonAssignmentVal, sizeof(int32_t));
	memcpy(buf + 0x6C, &unk4Val, sizeof(int32_t));
	memcpy(buf + 0x70, &unk5Val, sizeof(int32_t));
	memcpy(buf + 0x74, &unk6Val, sizeof(int32_t));
	memcpy(buf + 0x78, &nameType, sizeof(char));
	memcpy(buf + 0x79, &unk7, sizeof(char));
	memcpy(buf + 0x7A, &unk8, sizeof(char));
	memcpy(buf + 0x7B, &unk9, sizeof(char));

	for (int32_t i = 0; i < numOfParts; i++) {
		memcpy(buf + 0x7C + (0x24 * i), &parts[i].xPos, sizeof(char));
		memcpy(buf + 0x7C + (0x24 * i) + 1, &parts[i].yPos, sizeof(char));
		memcpy(buf + 0x7C + (0x24 * i) + 2, &parts[i].zPos, sizeof(char));
		memcpy(buf + 0x7C + (0x24 * i) + 3, &parts[i].isChallengePart, sizeof(char));
		memcpy(buf + 0x7C + (0x24 * i) + 4, &parts[i].isPainted, sizeof(char));
		memcpy(buf + 0x7C + (0x24 * i) + 5, &parts[i].unk1, sizeof(char));
		memcpy(buf + 0x7C + (0x24 * i) + 6, &parts[i].unk2, sizeof(char));
		memcpy(buf + 0x7C + (0x24 * i) + 7, &parts[i].unk3, sizeof(char));

		uint32_t partIdxVar = flipEndian(parts[i].partIdx);

		float yawVar = flipEndian_f32((char*)&parts[i].yaw, SRC_ENDIANLITTLE);
		float pitchVar = flipEndian_f32((char*)&parts[i].pitch, SRC_ENDIANLITTLE);
		float rollVar = flipEndian_f32((char*)&parts[i].roll, SRC_ENDIANLITTLE);

		int32_t unk2PartVar = flipEndian(parts[i].unk4);
		int32_t unk3PartVar = flipEndian(parts[i].unk5);

		memcpy(buf + 0x7C + (0x24 * i) + 8, &partIdxVar, sizeof(uint32_t));
		memcpy(buf + 0x7C + (0x24 * i) + 0xC, &yawVar, sizeof(float));
		memcpy(buf + 0x7C + (0x24 * i) + 0x10, &pitchVar, sizeof(float));
		memcpy(buf + 0x7C + (0x24 * i) + 0x14, &rollVar, sizeof(float));
		memcpy(buf + 0x7C + (0x24 * i) + 0x18, &parts[i].color, sizeof(uint32_t));
		memcpy(buf + 0x7C + (0x24 * i) + 0x1C, &unk2PartVar, sizeof(int32_t));
		memcpy(buf + 0x7C + (0x24 * i) + 0x20, &unk3PartVar, sizeof(int32_t));
	}

	return buf;
}

void Vehicle::WriteToFile(char* fileName, bool isSave) {
	int32_t position = 0;

	FILE* writeStrm = fopen(fileName, "wb");

	// If this is to be written as a save file, write two prefixes first.
	if (isSave) {
		fwrite(&VEHICLE_SAVE_PREFIX1, sizeof(uint32_t), 1, writeStrm);
		fwrite(&VEHICLE_SAVE_PREFIX2, sizeof(uint32_t), 1, writeStrm);
	}

	uint16_t numOfPartsVal = flipEndian(numOfParts);
	int16_t unk1Val = flipEndian(unk1);
	float preloadPowerVal = flipEndian_f32((char*)&preloadPower, SRC_ENDIANLITTLE);
	float preloadFuelVal = flipEndian_f32((char*)&preloadFuel, SRC_ENDIANLITTLE);
	float preloadAmmoVal = flipEndian_f32((char*)&preloadAmmo, SRC_ENDIANLITTLE);
	float preloadWeightVal = flipEndian_f32((char*)&preloadWeight, SRC_ENDIANLITTLE);
	float preloadBlocksUsedVal = flipEndian_f32((char*)&preloadBlocksUsed, SRC_ENDIANLITTLE);
	int32_t unk2Val = flipEndian(unk2);
	int32_t unk3Val = flipEndian(unk3);

	fwrite(&numOfPartsVal, sizeof(uint16_t), 1, writeStrm);
	fwrite(&unk1Val, sizeof(int16_t), 1, writeStrm);
	fwrite(&preloadPowerVal, sizeof(float), 1, writeStrm);
	fwrite(&preloadFuelVal, sizeof(float), 1, writeStrm);
	fwrite(&preloadAmmoVal, sizeof(float), 1, writeStrm);
	fwrite(&preloadWeightVal, sizeof(float), 1, writeStrm);
	fwrite(&preloadBlocksUsedVal, sizeof(float), 1, writeStrm);
	fwrite(&unk2Val, sizeof(int32_t), 1, writeStrm);
	fwrite(&unk3Val, sizeof(int32_t), 1, writeStrm);

	// If the name type is 1, we need to save the data as wchar_t. Otherwise just write it as a char string.
	if (nameType == 1) {
		for (int32_t i = 0; i < 0x20; i++) {
			wchar_t flippedChar = flipEndian(vehicleUnicodeName[i]);
			fwrite(&flippedChar, sizeof(wchar_t), 1, writeStrm);
		}
	}
	else if (nameType == 0) {
		fwrite(vehicleName, sizeof(char), 0x40, writeStrm);
	}

	uint32_t aButtonAssignmentVal = flipEndian(aButtonAssignment);
	uint32_t bButtonAssignmentVal = flipEndian(bButtonAssignment);
	uint32_t xButtonAssignmentVal = flipEndian(xButtonAssignment);

	int32_t unk4Val = flipEndian(unk4);
	int32_t unk5Val = flipEndian(unk5);
	int32_t unk6Val = flipEndian(unk6);

	fwrite(&aButtonAssignmentVal, sizeof(int32_t), 1, writeStrm);
	fwrite(&bButtonAssignmentVal, sizeof(int32_t), 1, writeStrm);
	fwrite(&xButtonAssignmentVal, sizeof(int32_t), 1, writeStrm);
	fwrite(&unk4Val, sizeof(int32_t), 1, writeStrm);
	fwrite(&unk5Val, sizeof(int32_t), 1, writeStrm);
	fwrite(&unk6Val, sizeof(int32_t), 1, writeStrm);
	fwrite(&nameType, sizeof(char), 1, writeStrm);
	fwrite(&unk7, sizeof(char), 1, writeStrm);
	fwrite(&unk8, sizeof(char), 1, writeStrm);
	fwrite(&unk9, sizeof(char), 1, writeStrm);

	for (int32_t i = 0; i < numOfParts; i++) {
		uint32_t partIdxVar = flipEndian(parts[i].partIdx);

		float yawVar = flipEndian_f32((char*)&parts[i].yaw, SRC_ENDIANLITTLE);
		float pitchVar = flipEndian_f32((char*)&parts[i].pitch, SRC_ENDIANLITTLE);
		float rollVar = flipEndian_f32((char*)&parts[i].roll, SRC_ENDIANLITTLE);

		int32_t unk2PartVar = flipEndian(parts[i].unk4);
		int32_t unk3PartVar = flipEndian(parts[i].unk5);

		fwrite(&parts[i].xPos, 1, 1, writeStrm);
		fwrite(&parts[i].yPos, 1, 1, writeStrm);
		fwrite(&parts[i].zPos, 1, 1, writeStrm);
		fwrite(&parts[i].isChallengePart, 1, 1, writeStrm);
		fwrite(&parts[i].isPainted, 1, 1, writeStrm);
		fwrite(&parts[i].unk1, 1, 1, writeStrm);
		fwrite(&parts[i].unk2, 1, 1, writeStrm);
		fwrite(&parts[i].unk3, 1, 1, writeStrm);

		fwrite(&partIdxVar, sizeof(uint32_t), 1, writeStrm);

		fwrite(&yawVar, sizeof(float), 1, writeStrm);
		fwrite(&pitchVar, sizeof(float), 1, writeStrm);
		fwrite(&rollVar, sizeof(float), 1, writeStrm);

		fwrite(&parts[i].color, sizeof(uint32_t), 1, writeStrm);

		//fwrite(&parts[i].colR, 1, 1, writeStrm);
		//fwrite(&parts[i].colG, 1, 1, writeStrm);
		//fwrite(&parts[i].colB, 1, 1, writeStrm);
		//fwrite(&parts[i].colA, 1, 1, writeStrm);

		fwrite(&unk2PartVar, sizeof(int32_t), 1, writeStrm);
		fwrite(&unk3PartVar, sizeof(int32_t), 1, writeStrm);
	}

	fflush(writeStrm);
	fclose(writeStrm);

	return;
}

void Vehicle::ReadVehicle(char* data, bool isFromSave) {
	vehiclePtr = data;

	isSaveVehicle = isFromSave;

	try {
		int32_t pos = 0;

		// Saved vehicles have an additional 8 bytes at the start
		if (isFromSave) pos = 8;

		printf("Allocating Values\n");

		// Establish the temporary values...
		uint16_t numOfPartsVal = 0;
		int16_t unk1Val = 0;
		char* preloadPowerVal = (char*)malloc(4);
		char* preloadFuelVal = (char*)malloc(4);;
		char* preloadAmmoVal = (char*)malloc(4);;
		char* preloadWeightVal = (char*)malloc(4);;
		char* preloadBlocksUsedVal = (char*)malloc(4);;
		int32_t unk2Val = 0;
		int32_t unk3Val = 0;

		memcpy_s(&nameType, sizeof(char), vehiclePtr + (pos + 0x78), sizeof(char));
		memcpy_s(&unk7, sizeof(char), vehiclePtr + (pos + 0x79), sizeof(char));
		memcpy_s(&unk8, sizeof(char), vehiclePtr + (pos + 0x7A), sizeof(char));
		memcpy_s(&unk9, sizeof(char), vehiclePtr + (pos + 0x7B), sizeof(char));

		printf("Allocating Namespace\n");

		// If this isn't from a save file, allocate space for the regular vehicle name file. Holds a total of 64 characters.
		if (nameType == 0) {
			memset(vehicleName, 0, 0x40);
		}

		// If this is from a save file, allocate space for the save name instead. Due to it being unicode, it holds a total of 32 characters.
		if (nameType == 1) {
			wmemset(vehicleUnicodeName, 0, 0x20);
		}

		uint32_t aButtonAssignmentVal = 0;
		uint32_t bButtonAssignmentVal = 0;
		uint32_t xButtonAssignmentVal = 0;

		int32_t unk4Val = 0;
		int32_t unk5Val = 0;
		int32_t unk6Val = 0;

		printf("Reading Vehicle Name\n");

		// Don't need to do anything special if we're just reading from a vehicle file.
		if (nameType == 0) {
			memcpy_s(vehicleName, 0x40, vehiclePtr + pos + 0x20, 0x40);
			printf("%s\n", vehicleName);
		}

		// We need to flip the endianness of every applicable character.
		if (nameType == 1) {
			int32_t namePos = pos + 0x20;
			wchar_t newChar = L'\n';
			for (int32_t i = 0, c = 0; i < 0x40; i += 2, c++) {
				memcpy_s(&newChar, sizeof(wchar_t), vehiclePtr + namePos + i, sizeof(wchar_t));
				newChar = flipEndian(newChar);

				if (newChar == 0) break;

				vehicleUnicodeName[c] = newChar;
				//memcpy(&vehicleSaveName + i, &newChar, sizeof(wchar_t));
				printf("%d -> %lc\n", i, newChar);
			}
			wprintf(L"Vehicle Name %s\n", vehicleUnicodeName);
			//memcpy(vehicleName, vehiclePtr + pos + 0x20, 0x40);
		}

		printf("Reading Vehicle Aspects\n");

		//...and now we read the data.
		memcpy(&numOfPartsVal, vehiclePtr + pos, sizeof(uint16_t));
		memcpy(&unk1Val, vehiclePtr + pos + 2, sizeof(int16_t));
		memcpy(preloadPowerVal, vehiclePtr + pos + 4, sizeof(float));
		memcpy(preloadFuelVal, vehiclePtr + pos + 8, sizeof(float));
		memcpy(preloadAmmoVal, vehiclePtr + pos + 0xC, sizeof(float));
		memcpy(preloadWeightVal, vehiclePtr + pos + 0x10, sizeof(float));
		memcpy(preloadBlocksUsedVal, vehiclePtr + pos + 0x14, sizeof(float));
		memcpy(&unk2Val, vehiclePtr + pos + 0x18, sizeof(int32_t));
		memcpy(&unk3Val, vehiclePtr + pos + 0x1C, sizeof(int32_t));

		memcpy(&aButtonAssignmentVal, vehiclePtr + pos + 0x60, sizeof(int32_t));
		memcpy(&bButtonAssignmentVal, vehiclePtr + pos + 0x64, sizeof(int32_t));
		memcpy(&xButtonAssignmentVal, vehiclePtr + pos + 0x68, sizeof(int32_t));

		memcpy(&unk4Val, vehiclePtr + pos + 0x6C, sizeof(int32_t));
		memcpy(&unk5Val, vehiclePtr + pos + 0x70, sizeof(int32_t));
		memcpy(&unk6Val, vehiclePtr + pos + 0x74, sizeof(int32_t));

		numOfParts = flipEndian(numOfPartsVal);
		unk1 = flipEndian(unk1Val);
		preloadPower = flipEndian_f32(preloadPowerVal, SRC_ENDIANBIG);
		preloadFuel = flipEndian_f32(preloadFuelVal, SRC_ENDIANBIG);
		preloadAmmo = flipEndian_f32(preloadAmmoVal, SRC_ENDIANBIG);
		preloadWeight = flipEndian_f32(preloadWeightVal, SRC_ENDIANBIG);
		preloadBlocksUsed = flipEndian_f32(preloadBlocksUsedVal, SRC_ENDIANBIG);
		unk2 = flipEndian(unk2Val);
		unk3 = flipEndian(unk3Val);

		aButtonAssignment = flipEndian(aButtonAssignmentVal);
		bButtonAssignment = flipEndian(bButtonAssignmentVal);
		xButtonAssignment = flipEndian(xButtonAssignmentVal);
		unk4 = flipEndian(unk4Val);
		unk5 = flipEndian(unk5Val);
		unk6 = flipEndian(unk6Val);

		printf("--------- Vehicle Info ---------\n");
		printf("Num of Parts: %d\n", numOfParts);
		printf("Cached Power Dial: %f\n", preloadPower);
		printf("Cached Fuel Dial: %f\n", preloadFuel);
		printf("Cached Ammo Dial: %f\n", preloadAmmo);
		printf("Cached Weight Dial: %f\n", preloadWeight);
		printf("Cached Blocks Used Dial: %f\n", preloadBlocksUsed);

		printf("A Button Assignment: %u\n", aButtonAssignment);
		printf("B Button Assignment: %u\n", bButtonAssignment);
		printf("X Button Assignment: %u\n", xButtonAssignment);

		// Read the data from every part.
		parts = new VehiclePart[numOfParts]();

		int32_t partOffset = pos + 0x7C;

		for (int32_t i = 0; i < numOfParts; i++) {
			uint32_t partIdxVar; // 0x8

			float yawVar; // 0xC
			float pitchVar; // 0x10
			float rollVar; // 0x14

			uint32_t colour; // 0x8

			int32_t unk2PartVar;
			int32_t unk3PartVar;

			memcpy(&parts[i].xPos, vehiclePtr + (partOffset + (i * 0x24)), sizeof(char));
			memcpy(&parts[i].yPos, vehiclePtr + (partOffset + (i * 0x24)) + 1, sizeof(char));
			memcpy(&parts[i].zPos, vehiclePtr + (partOffset + (i * 0x24)) + 2, sizeof(char));
			memcpy(&parts[i].isChallengePart, vehiclePtr + (partOffset + (i * 0x24)) + 3, sizeof(char));
			memcpy(&parts[i].isPainted, vehiclePtr + (partOffset + (i * 0x24)) + 4, sizeof(char));
			memcpy(&parts[i].unk1, vehiclePtr + (partOffset + (i * 0x24)) + 5, sizeof(char));
			memcpy(&parts[i].unk2, vehiclePtr + (partOffset + (i * 0x24)) + 6, sizeof(char));

			memcpy(&parts[i].unk3, vehiclePtr + (partOffset + (i * 0x24)) + 7, sizeof(char));

			memcpy(&partIdxVar, vehiclePtr + (partOffset + (i * 0x24)) + 8, sizeof(uint32_t));

			memcpy(&colour, vehiclePtr + (partOffset + (i * 0x24)) + 0x18, sizeof(uint32_t));

			//memcpy(&parts[i].colR, vehiclePtr + (partOffset + (i * 0x24)) + 0x18, sizeof(char));
			//memcpy(&parts[i].colG, vehiclePtr + (partOffset + (i * 0x24)) + 0x19, sizeof(char));
			//memcpy(&parts[i].colB, vehiclePtr + (partOffset + (i * 0x24)) + 0x1A, sizeof(char));
			//memcpy(&parts[i].colA, vehiclePtr + (partOffset + (i * 0x24)) + 0x1B, sizeof(char));

			memcpy(&unk2PartVar, vehiclePtr + (partOffset + (i * 0x24)) + 0x1C, sizeof(uint32_t));
			memcpy(&unk3PartVar, vehiclePtr + (partOffset + (i * 0x24)) + 0x20, sizeof(uint32_t));

			parts[i].color = colour;
			parts[i].partIdx = flipEndian(partIdxVar);
			parts[i].unk4 = flipEndian(unk2PartVar);
			parts[i].unk5 = flipEndian(unk3PartVar);

			parts[i].yaw = flipEndian_f32(vehiclePtr + (partOffset + (i * 0x24)) + 0xC, SRC_ENDIANBIG);
			parts[i].pitch = flipEndian_f32(vehiclePtr + (partOffset + (i * 0x24)) + 0x10, SRC_ENDIANBIG);
			parts[i].roll = flipEndian_f32(vehiclePtr + (partOffset + (i * 0x24)) + 0x14, SRC_ENDIANBIG);

			printf("Part %d - {%f %f %f}\n", i, parts[i].yaw, parts[i].pitch, parts[i].roll);
		}
	}
	catch (std::exception e) {
		printf("Vehicle::ReadVehicle() - An error occured while attempting to read the given file.\n Error %s\n", e.what());
	}
}

Manifest::~Manifest() {
	free(manifestPtr);

	delete[] aidTable;
}

void Manifest::ReadManifest(char* data) {
	manifestPtr = data;

	uint32_t magicVar = 0;
	uint32_t hashVar = 0;

	int32_t aidTableOffsetVar = 0;
	int32_t aidTableCountVar = 0;

	int32_t referenceTableOffsetVar = 0;
	int32_t referenceTableCountVar = 0;

	int32_t xcueRefTableOffsetVar = 0;
	int32_t xcueRefTableCountVar = 0;

	memcpy(&magicVar, manifestPtr, sizeof(uint32_t));
	memcpy(&hashVar, manifestPtr + 4, sizeof(uint32_t));
	memcpy(&aidTableOffsetVar, manifestPtr + 8, sizeof(int32_t));
	memcpy(&aidTableCountVar, manifestPtr + 0xC, sizeof(int32_t));
	memcpy(&referenceTableOffsetVar, manifestPtr + 0x10, sizeof(int32_t));
	memcpy(&referenceTableCountVar, manifestPtr + 0x14, sizeof(int32_t));
	memcpy(&xcueRefTableOffsetVar, manifestPtr + 0x18, sizeof(int32_t));
	memcpy(&xcueRefTableCountVar, manifestPtr + 0x1C, sizeof(int32_t));

	magic = flipEndian(magicVar);
	timestamp = flipEndian(hashVar);

	aidTableOffset = flipEndian(aidTableOffsetVar);
	aidTableCount = flipEndian(aidTableCountVar);

	referenceTableOffset = flipEndian(referenceTableOffsetVar);
	referenceTableCount = flipEndian(referenceTableCountVar);

	xcueRefTableOffset = flipEndian(xcueRefTableOffsetVar);
	xcueRefTableCount = flipEndian(xcueRefTableCountVar);

	printf("Manifest::ReadManifest() - Magic = %08X, Timestamp = %d.\n", magic, timestamp);

	if (aidTableCount != 0) {
		aidTable = new AidEntry[aidTableCount]();

		for (int32_t i = 0; i < aidTableCount; i++) {
			AidEntry entry;

			int32_t aidOffset = aidTableOffset + (sizeof(AidEntry) * i);

			uint32_t aidEntryHash = 0;
			int32_t aidEntryID = 0;

			memcpy(&aidEntryHash, manifestPtr + aidOffset, sizeof(uint32_t));
			memcpy(&aidEntryID, manifestPtr + (aidOffset + 4), sizeof(int32_t));

			entry.hash = flipEndian(aidEntryHash);
			entry.id = flipEndian(aidEntryID);

			aidTable[i] = entry;

			//printf("%08X %d\n", aidTable[i].hash, aidTable[i].id);
		}
	}

	if (referenceTableCount != 0) {
		referenceTable = (int32_t*)malloc(sizeof(int32_t) * referenceTableCount);
	}

	if (xcueRefTableCount != 0) {
		xcueRefTable = (int32_t*)malloc(sizeof(int32_t) * xcueRefTableCount);
	}
}

void Texture::ReadTextureInfo(char* data) {
	textureHeaderPtr = data;

	memset(headerSect.magic, 0, sizeof(headerSect.magic));
	memset(headerSect.version, 0, sizeof(headerSect.version));

	strcpy(headerSect.magic, textureHeaderPtr);
	strcpy(headerSect.version, textureHeaderPtr + 8);

	printf("%s %s\n", headerSect.magic, headerSect.version);

	headerSect.unk_0x18 = *(textureHeaderPtr + 0x18);
	headerSect.unk_0x19 = *(textureHeaderPtr + 0x19);
	headerSect.isSwizzled = *(textureHeaderPtr + 0x1A);
	headerSect.textureType = (unsigned char)*(textureHeaderPtr + 0x1B);

	int16_t width = 0;
	int16_t height = 0;

	memcpy(&width, textureHeaderPtr + 0x24, sizeof(int16_t));
	memcpy(&height, textureHeaderPtr + 0x26, sizeof(int16_t));

	width = flipEndian(width);
	height = flipEndian(height);

	printf("Is Swizzled %d - Texture Type 0x%02x\n", headerSect.isSwizzled, headerSect.textureType);
	printf("Width %d - Height %d\n", width, height);

	headerSect.width = width;
	headerSect.height = height;

	int32_t frameCount = 0;

	memcpy(&frameCount, textureHeaderPtr + 0x38, sizeof(int32_t));

	frameCount = flipEndian(frameCount);

	if (frameCount == 0) {
		headerSect.frameCount = 1;
	}
	else {
		headerSect.frameCount = frameCount;
	}

	int32_t gpuOffsTableOffset = 0;

	memcpy(&gpuOffsTableOffset, textureHeaderPtr + 0x3C, sizeof(int32_t));

	gpuOffsTableOffset = flipEndian(gpuOffsTableOffset);
	headerSect.gpuOffsTablePos = gpuOffsTableOffset;

	if (frameCount != 0) {
		headerSect.gpuOffsTable = new int32_t[frameCount];
		
		for (int32_t i = 0; i < frameCount; i++) {
			int32_t off = 0;

			memcpy(&off, textureHeaderPtr + gpuOffsTableOffset + (i * 4), sizeof(int32_t));

			off = flipEndian(off);
			headerSect.gpuOffsTable[i] = off;
		}
	}
	else {
		headerSect.gpuOffsTable = new int32_t[1];
		headerSect.gpuOffsTable[0] = 0;
	}
}