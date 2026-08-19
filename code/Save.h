#pragma once
#include <cstdint>

static const int32_t NORMAL_GAME_FLAG_COUNT = 0x719;
static const int32_t GLOBAL_GAME_FLAG_COUNT = 0x112;
static const int32_t VOLATILE_GAME_FLAG_COUNT = 0x2;

static const int32_t DATA_START = 0x28;
const int32_t SAVE_PREFIX1 = 0x48E19A3F;
const int32_t SAVE_PREFIX2 = 0xE17A5440;

struct BitFlag {
	uint32_t value;

	bool isFlagSet(int32_t pos) {

	}
};

struct SaveData {
	float unk1;
	int32_t unk2;

	int32_t gameFlagNormalByteCount;
	int32_t gameFlagGlobalByteCount;
	int32_t gameFlagVolatileByteCount;
	int32_t gameCounterCount;
	int32_t byteCount5;
	int32_t byteCount6;

	char* gameFlagNormalArr;
	char* gameFlagGlobalArr;
	char* GameFlagVolatileArr;
	uint32_t* gameCountersArr;
	char* flag4Arr;
	char* flag5Arr;

	void LoadSaveFile(char* data);
	char* CompileToDataArray();

	bool GetNormalGameFlag(int32_t idx) {
		int32_t remainIdx = idx % 8;
		int32_t flagPos = idx / 8;

		bool val = false;

		switch (remainIdx) {
		case 0:
			val = (gameFlagNormalArr[flagPos] & 0x1) != 0;
			break;
		case 1:
			val = (gameFlagNormalArr[flagPos] & 0x2) != 0;
			break;
		case 2:
			val = (gameFlagNormalArr[flagPos] & 0x4) != 0;
			break;
		case 3:
			val = (gameFlagNormalArr[flagPos] & 0x8) != 0;
			break;
		case 4:
			val = (gameFlagNormalArr[flagPos] & 0x10) != 0;
			break;
		case 5:
			val = (gameFlagNormalArr[flagPos] & 0x20) != 0;
			break;
		case 6:
			val = (gameFlagNormalArr[flagPos] & 0x40) != 0;
			break;
		case 7:
			val = (gameFlagNormalArr[flagPos] & 0x80) != 0;
			break;
		}

		return val;
	}

	void SetNormalGameFlag(bool val, int32_t idx) {
		int32_t remainIdx = idx % 8;
		int32_t flagPos = idx / 8;

		char newVal = 0;

		for (int32_t i = 0; i < 8; i++) {
			if (i == remainIdx) {
				newVal = newVal | idx << remainIdx;
			}
			else {
				newVal = newVal | gameFlagNormalArr[flagPos] << i;
			}
		}

		gameFlagNormalArr[flagPos] = newVal;
	}

	int32_t GetGameCounter(int32_t idx) {
		if (idx < 0 || idx >= gameCounterCount) return 0;
		return gameCountersArr[idx];
	}

	void SetGameCounter(int32_t val, int32_t idx) {
		if (idx < 0 || idx >= gameCounterCount) return;
		gameCountersArr[idx] = val;
	}
};