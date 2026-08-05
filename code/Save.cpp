#include "Save.h"
#include "CommonReader.h"
#include "FlagCounterCommons.h"

void SaveData::LoadSaveFile(char* data) {
	float unk1Var;
	int unk2Var;

	int gameFlagNormalCountVar;
	int gameFlagGlobalCountVar;
	int gameFlagVolatileCountVar;
	int gameCounterCountVar;
	int count5Var;
	int count6Var;

	memcpy(&unk1Var, data + 8, 4);
	memcpy(&unk2Var, data + 0xC, 4);

	memcpy(&gameFlagNormalCountVar, data + 0x10, 4);
	memcpy(&gameFlagGlobalCountVar, data + 0x14, 4);
	memcpy(&gameFlagVolatileCountVar, data + 0x18, 4);
	memcpy(&gameCounterCountVar, data + 0x1C, 4);
	memcpy(&count5Var, data + 0x20, 4);
	memcpy(&count6Var, data + 0x24, 4);

	unk1 = flipEndian_f32((char*)&unk1Var, SRC_ENDIANBIG);
	unk2 = flipEndian(unk2Var);

	gameFlagNormalByteCount = flipEndian(gameFlagNormalCountVar);
	gameFlagGlobalByteCount = flipEndian(gameFlagGlobalCountVar);
	gameFlagVolatileByteCount = flipEndian(gameFlagVolatileCountVar);
	gameCounterCount = flipEndian(gameCounterCountVar);
	byteCount5 = flipEndian(count5Var);
	byteCount6 = flipEndian(count6Var);

	gameFlagNormalArr = new char[gameFlagNormalByteCount];
	gameFlagGlobalArr = new char[gameFlagGlobalByteCount];
	GameFlagVolatileArr = new char[gameFlagVolatileByteCount];
	gameCountersArr = new unsigned int[gameCounterCount];
	flag4Arr = new char[byteCount5];
	flag5Arr = new char[byteCount6];

	int position = DATA_START;

	bool flag1 = false;
	bool flag2 = false;
	bool flag3 = false;
	bool flag4 = false;
	bool flag5 = false;
	bool flag6 = false;
	bool flag7 = false;
	bool flag8 = false;

	int nameId = 0;
	for (int i = 0; i < gameFlagNormalByteCount; i++) {
		gameFlagNormalArr[i] = *(data + position);
		position++;
	}

	position = DATA_START + gameFlagNormalByteCount;

	for (int i = 0; i < gameFlagGlobalByteCount; i++) {
		gameFlagGlobalArr[i] = *(data + position);

		flag1 = (gameFlagGlobalArr[i] & 0x1) != 0;
		flag2 = (gameFlagGlobalArr[i] & 0x2) != 0;
		flag3 = (gameFlagGlobalArr[i] & 0x4) != 0;
		flag4 = (gameFlagGlobalArr[i] & 0x8) != 0;
		flag5 = (gameFlagGlobalArr[i] & 0x10) != 0;
		flag6 = (gameFlagGlobalArr[i] & 0x20) != 0;
		flag7 = (gameFlagGlobalArr[i] & 0x40) != 0;
		flag8 = (gameFlagGlobalArr[i] & 0x80) != 0;
		//printf("Global Flag %d - DATA %02x (%d %d %d %d %d %d %d %d)\n", i, gameFlagGlobalArr[i], flag1, flag2, flag3, flag4, flag5, flag6, flag7, flag8);
		position++;
	}

	position = DATA_START + gameFlagNormalByteCount + gameFlagGlobalByteCount;

	for (int i = 0; i < gameFlagVolatileByteCount; i++) {
		GameFlagVolatileArr[i] = *(data + position);

		flag1 = (GameFlagVolatileArr[i] & 0x1) != 0;
		flag2 = (GameFlagVolatileArr[i] & 0x2) != 0;
		flag3 = (GameFlagVolatileArr[i] & 0x4) != 0;
		flag4 = (GameFlagVolatileArr[i] & 0x8) != 0;
		flag5 = (GameFlagVolatileArr[i] & 0x10) != 0;
		flag6 = (GameFlagVolatileArr[i] & 0x20) != 0;
		flag7 = (GameFlagVolatileArr[i] & 0x40) != 0;
		flag8 = (GameFlagVolatileArr[i] & 0x80) != 0;
		//printf("Volatile Flag %d - DATA %02x (%d %d %d %d %d %d %d %d)\n", i, GameFlagVolatileArr[i], flag1, flag2, flag3, flag4, flag5, flag6, flag7, flag8);
		position++;
	}

	position = DATA_START + gameFlagNormalByteCount + gameFlagGlobalByteCount + gameFlagVolatileByteCount;

	for (int i = 0; i < gameCounterCount; i++) {
		unsigned int counterVal = 0;
		memcpy(&counterVal, data + position + (i * 4), 4);
		gameCountersArr[i] = flipEndian(counterVal);

		//printf("Counter %d - %s (%u)\n", i, GameCounterNames[i], gameCountersArr[i]);
	}

	position = DATA_START + gameFlagNormalByteCount + gameFlagGlobalByteCount + gameFlagVolatileByteCount + (gameCounterCount * 4);

	for (int i = 0; i < byteCount5; i++) {
		flag4Arr[i] = *(data + position);
		position++;
	}

	position = DATA_START + gameFlagNormalByteCount + gameFlagGlobalByteCount + gameFlagVolatileByteCount + (gameCounterCount * 4) + byteCount5;

	for (int i = 0; i < byteCount6; i++) {
		flag5Arr[i] = *(data + position);
		position++;
	}
}

char* SaveData::CompileToDataArray() {
	char* data = (char*)malloc(0x28 + gameFlagNormalByteCount + gameFlagGlobalByteCount + gameFlagVolatileByteCount + (gameCounterCount * 4) + byteCount5 + byteCount6);
	memcpy(data, &SAVE_PREFIX1, 4);
	memcpy(data + 4, &SAVE_PREFIX2, 4);

	float unk1Var = flipEndian_f32((char*)&unk1, SRC_ENDIANLITTLE);
	int unk2var = flipEndian(unk2);

	memcpy(data + 8, &unk1Var, 4);
	memcpy(data + 0xC, &unk2var, 4);

	int gameFlagNormalCountVar = flipEndian(gameFlagNormalByteCount);
	int gameFlagGlobalCountVar = flipEndian(gameFlagGlobalByteCount);
	int gameFlagVolatileCountVar = flipEndian(gameFlagVolatileByteCount);
	int gameCounterCountVar = flipEndian(gameCounterCount);
	int count5Var = flipEndian(byteCount5);
	int count6Var = flipEndian(byteCount6);

	memcpy(data + 0x10, &gameFlagNormalCountVar, 4);
	memcpy(data + 0x14, &gameFlagGlobalCountVar, 4);
	memcpy(data + 0x18, &gameFlagVolatileCountVar, 4);
	memcpy(data + 0x1C, &gameCounterCountVar, 4);
	memcpy(data + 0x20, &count5Var, 4);
	memcpy(data + 0x24, &count6Var, 4);

	int position = DATA_START;

	for (int i = 0; i < gameFlagNormalByteCount; i++) {
		memcpy(data + position, &gameFlagNormalArr[i], 1);
		position++;
	}

	position = DATA_START + gameFlagNormalByteCount;

	for (int i = 0; i < gameFlagGlobalByteCount; i++) {
		memcpy(data + position, &gameFlagGlobalArr[i], 1);
		position++;
	}

	position = DATA_START + gameFlagNormalByteCount + gameFlagGlobalByteCount;

	for (int i = 0; i < gameFlagVolatileByteCount; i++) {
		memcpy(data + position, &GameFlagVolatileArr[i], 1);
		position++;
	}

	position = DATA_START + gameFlagNormalByteCount + gameFlagGlobalByteCount + gameFlagVolatileByteCount;

	for (int i = 0; i < gameFlagVolatileByteCount; i++) {
		memcpy(data + position, &GameFlagVolatileArr[i], 1);
		position++;
	}

	return data;
}