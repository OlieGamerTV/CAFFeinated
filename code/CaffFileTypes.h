#pragma once
#ifndef _CAFFCOMMONFILETYPES
#define _CAFFCOMMONFILETYPES

#include "BaseScript.h"
#include "Loctext.h"

#ifdef _WIN32 
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <stdlib.h>
#include "PinataTex.h"

const uint32_t VEHICLE_HEAD_GAMEID = 0xED07534D;
const uint32_t VEHICLE_SAVE_PREFIX1 = 0x48E19A3F;
const uint32_t VEHICLE_SAVE_PREFIX2 = 0xE17A5440;

#pragma region Script
struct Script {
	char* scriptPtr;

	int32_t entryCount = 0; // Not actually part of the file. Calculated on read.
	dbScript_Base* entries;

	void ReadScript(char* data);

	char* WriteToArray();
	void WriteToFile(char* fileName, bool isSave);
};
#pragma endregion


enum AssetType : char {

};

struct AidEntry {
	uint32_t hash;
	int32_t id;
};

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
	uint32_t partIdx = 0; // 0x8

	float yaw = 0; // 0xC
	float pitch = 0; // 0x10
	float roll = 0; // 0x14

	uint32_t color = 0xFFFFFFFF; // 0x18

	//unsigned char colR = 0xFF; // 0x18
	//unsigned char colG = 0xFF; // 0x19
	//unsigned char colB = 0xFF; // 0x1A
	//unsigned char colA = 0xFF; // 0x1B

	int32_t buttonMask = 0; // 0x1C
	int32_t unk5 = 0; // 0x20
};

struct Vehicle {
public:
	char* vehiclePtr;

	bool isSaveVehicle = false;

	uint16_t numOfParts = 0; // 0x0
	int8_t unk1 = 0; // 0x2
	int8_t unk1_1 = 0; // 0x3
	float preloadPower = 0; // 0x4
	float preloadFuel = 0; // 0x8
	float preloadAmmo = 0; // 0xC
	float preloadWeight = 0; // 0x10
	float preloadBlocksUsed = 0; // 0x14
	int32_t unk2 = 0; // 0x18
	int32_t unk3 = 0; // 0x1C

	// A buffer of 0x40 bytes is reserved for this.
	// A total of 64 characters will be in this.
	char vehicleName[0x40];

	// A buffer of 0x40 bytes is reserved for this.
	// A total of 32 characters will be in this.
	wchar_t vehicleUnicodeName[0x20];

	uint32_t aButtonAssignment = 0; // 0x60
	uint32_t bButtonAssignment = 0; // 0x64
	uint32_t xButtonAssignment = 0; // 0x68

	int32_t unk4 = 0; // 0x6C
	int32_t unk5 = 0; // 0x70
	int32_t unk6 = 0; // 0x74
	char nameType = 0; // 0x78
	char unk7 = 0; // 0x79
	char unk8 = 0; // 0x7A
	char unk9 = 0; // 0x7B

	std::vector<VehiclePart> parts;

	void ReadVehicle(char* data, bool isFromSave);

	char* WriteToArray();
	void WriteToFile(char* fileName, bool isSave);
	void WriteHeaderFile(char* fileName);

	bool IsPositionTaken(char x, char y, char z) {
		for (int32_t i = 0; i < numOfParts; i++) {
			if (parts[i].xPos == x && parts[i].yPos == y && parts[i].zPos == z) return true;
		}

		return false;
	}

	void AddEntry(VehiclePart part) {
		numOfParts++;

		parts.push_back(part);
	}

	void RemoveEntry(int32_t idx) {
		numOfParts--;

		parts.erase(parts.begin() + idx);
	}
};

struct Manifest {
	char* manifestPtr = nullptr;

	uint32_t magic = 0;
	uint32_t timestamp = 0;

	// Tables
	int32_t aidTableOffset = 0;
	int32_t aidTableCount = 0;

	int32_t referenceTableOffset = 0;
	int32_t referenceTableCount = 0;

	int32_t xcueRefTableOffset = 0;
	int32_t xcueRefTableCount = 0;

	AidEntry* aidTable = nullptr;
	int32_t* referenceTable = nullptr;
	int32_t* xcueRefTable = nullptr;

	void ReadManifest(char* data);

	uint32_t GetAidHash(int32_t id) {
		if (aidTable == NULL) return 0;

		for (int32_t i = 0; i < aidTableCount; i++) {
			if (aidTable[i].id == id) return aidTable[i].hash;
		}
		return 0;
	}

	~Manifest();
};

struct TextureDataSect {
public:
	char identifier[8]; // The identifier of the file.
	char version[0x10]; // The version of the file.

	// Texture Info
	uint32_t format = 0;
	int32_t type = 0;
	int32_t flags = 0;
	int16_t width = 0;
	int16_t height = 0;
	int32_t image = 1;
	int32_t mipOffset = -1;
	int8_t maxLOD = 0;
	int32_t gpuOffsTablePos = 0;
	int32_t* gpuOffsTable;
	
	int32_t numFrames = 0;
};

struct Texture {
public:
	TextureDataSect headerSect;
	Pinata::dbTexture_s pinataTexture;
	char* textureHeaderPtr;
	bool refresh = true;
	bool isPinataTex = false;

	unsigned char* textureDataPtr;

	void ReadTextureInfo(char* data);

	void SetTextureHeaderPtr(char* ptr) {
		textureHeaderPtr = ptr;
	}

	void SetTextureDataPtr(unsigned char* ptr) {
		textureDataPtr = ptr;
	}
};
#endif