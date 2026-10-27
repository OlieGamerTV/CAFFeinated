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

		int32_t unk2PartVar = flipEndian(parts[i].buttonMask);
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

		int32_t unk2PartVar = flipEndian(parts[i].buttonMask);
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
		int32_t partOffset = pos + 0x7C;

		for (int32_t i = 0; i < numOfParts; i++) {
			parts.emplace_back();
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
			parts[i].buttonMask = flipEndian(unk2PartVar);
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

	if (magicVar == 0xB65E2D43) { // V0040
		memcpy(&aidTableOffsetVar, manifestPtr + 0x10, sizeof(int32_t));
		memcpy(&aidTableCountVar, manifestPtr + 0x14, sizeof(int32_t));
		memcpy(&referenceTableOffsetVar, manifestPtr + 0x18, sizeof(int32_t));
		memcpy(&referenceTableCountVar, manifestPtr + 0x1C, sizeof(int32_t));
		memcpy(&xcueRefTableOffsetVar, manifestPtr + 0x20, sizeof(int32_t));
		memcpy(&xcueRefTableCountVar, manifestPtr + 0x24, sizeof(int32_t));
	}

	if (magicVar == 0x7CB48C43) { // V0036
		memcpy(&aidTableOffsetVar, manifestPtr + 8, sizeof(int32_t));
		memcpy(&aidTableCountVar, manifestPtr + 0xC, sizeof(int32_t));
		memcpy(&referenceTableOffsetVar, manifestPtr + 0x10, sizeof(int32_t));
		memcpy(&referenceTableCountVar, manifestPtr + 0x14, sizeof(int32_t));
		memcpy(&xcueRefTableOffsetVar, manifestPtr + 0x18, sizeof(int32_t));
		memcpy(&xcueRefTableCountVar, manifestPtr + 0x1C, sizeof(int32_t));
	}

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

	memset(headerSect.identifier, 0, sizeof(headerSect.identifier));
	memset(headerSect.version, 0, sizeof(headerSect.version));

	strcpy(headerSect.identifier, textureHeaderPtr);
	strcpy(headerSect.version, textureHeaderPtr + 8);

	printf("%s %s\n", headerSect.identifier, headerSect.version);

	memcpy(&headerSect.format, textureHeaderPtr + 0x18, sizeof(int32_t));
	memcpy(&headerSect.type, textureHeaderPtr + 0x1C, sizeof(int32_t));
	memcpy(&headerSect.flags, textureHeaderPtr + 0x20, sizeof(int32_t));
	memcpy(&headerSect.image, textureHeaderPtr + 0x28, sizeof(int32_t));
	memcpy(&headerSect.mipOffset, textureHeaderPtr + 0x2C, sizeof(int32_t));
	
	headerSect.type = flipEndian(headerSect.type);
	headerSect.flags = flipEndian(headerSect.flags);
	headerSect.image = flipEndian(headerSect.image);
	headerSect.mipOffset = flipEndian(headerSect.mipOffset);

	int16_t width = 0;
	int16_t height = 0;

	memcpy(&width, textureHeaderPtr + 0x24, sizeof(int16_t));
	memcpy(&height, textureHeaderPtr + 0x26, sizeof(int16_t));

	width = flipEndian(width);
	height = flipEndian(height);

	printf("Texture Type 0x%08x\n", headerSect.format);
	printf("Width %d - Height %d\n", width, height);

	headerSect.width = width;
	headerSect.height = height;

	int32_t gpuOffsTableOffset = 0;

	memcpy(&gpuOffsTableOffset, textureHeaderPtr + 0x3C, sizeof(int32_t));

	gpuOffsTableOffset = flipEndian(gpuOffsTableOffset);
	headerSect.gpuOffsTablePos = gpuOffsTableOffset;

	if (headerSect.type == 4) {
		memcpy(&headerSect.numFrames, textureHeaderPtr + 0x28, sizeof(int32_t));
		headerSect.numFrames = flipEndian(headerSect.numFrames);
		headerSect.gpuOffsTable = new int32_t[headerSect.numFrames];

		for (int32_t i = 0; i < headerSect.numFrames; i++) {
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