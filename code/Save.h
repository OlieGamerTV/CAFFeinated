#pragma once

static const int NORMAL_GAME_FLAG_COUNT = 0x719;
static const int GLOBAL_GAME_FLAG_COUNT = 0x112;
static const int VOLATILE_GAME_FLAG_COUNT = 0x2;

static const int DATA_START = 0x28;
const unsigned int SAVE_PREFIX1 = 0x48E19A3F;
const unsigned int SAVE_PREFIX2 = 0xE17A5440;

struct BitFlag {
	int value;

	bool isFlagSet(int pos) {

	}
};

struct SaveData {
	float unk1;
	int unk2;

	int gameFlagNormalByteCount;
	int gameFlagGlobalByteCount;
	int gameFlagVolatileByteCount;
	int gameCounterCount;
	int byteCount5;
	int byteCount6;

	char* gameFlagNormalArr;
	char* gameFlagGlobalArr;
	char* GameFlagVolatileArr;
	unsigned int* gameCountersArr;
	char* flag4Arr;
	char* flag5Arr;

	void LoadSaveFile(char* data);
	char* CompileToDataArray();

	bool GetNormalGameFlag(int idx) {
		int remainIdx = idx % 8;
		int flagPos = idx / 8;

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

	void SetNormalGameFlag(bool val, int idx) {
		int remainIdx = idx % 8;
		int flagPos = idx / 8;

		char newVal = 0;

		for (int i = 0; i < 8; i++) {
			if (i == remainIdx) {
				newVal = newVal | idx << remainIdx;
			}
			else {
				newVal = newVal | gameFlagNormalArr[flagPos] << i;
			}
		}

		gameFlagNormalArr[flagPos] = newVal;
	}

	int GetGameCounter(int idx) {
		if (idx < 0 || idx >= gameCounterCount) return 0;
		return gameCountersArr[idx];
	}

	void SetGameCounter(int val, int idx) {
		if (idx < 0 || idx >= gameCounterCount) return;
		gameCountersArr[idx] = val;
	}
};