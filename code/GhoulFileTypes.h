#ifndef _GHOULFILETYPES
#define _GHOULFILETYPES
#include <vector>

#ifdef _WIN32
#pragma once
#endif

/// <summary>
/// A list of the available asset types the game supports.
/// </summary>
static const char* ghoulies_AssetArray[] = {
	"UNKNOWN", // 0x00
	"texture", // 0x01
	"anim",
	"UNKNOWN",
	"model",
	"animevents",
	"UNKNOWN",
	"cutscene",
	"cutsceneevents",
	"UNKNOWN",
	"misc", // 0x0A
	"actorgoals",
	"marker",
	"callout",
	"aidlist",
	"UNKNOWN",
	"loctext", // 0x10
	"UNKNOWN",
	"xsoundbank",
	"xdsp",
	"xcuelist",
	"font",
	"ghoulybox",
	"ghoulyspawn",
	"script",
	"actorattributes",
	"fxemitter",
	"fxparticle",
	"fxrumble",
	"fxcamshake",
};

enum GhouliesDBType : int32_t {
	ghoulDB_Texture = 1,
	ghoulDB_Anim = 2,
	ghoulDB_Model = 4,
	ghoulDB_AnimEvents = 5,
	ghoulDB_Cutscene = 7,
	ghoulDB_CutsceveEvents = 8,
	ghoulDB_Misc = 10,
	ghoulDB_ActorGoals = 11,
	ghoulDB_Marker = 12,
	ghoulDB_Callout = 13,
	ghoulDB_AidList = 14,
	ghoulDB_Loctext = 16,
	ghoulDB_XSoundBank = 18,
	ghoulDB_XDSP = 19,
	ghoulDB_XCueList = 20,
	ghoulDB_Font = 21,
	ghoulDB_GhoulyBox = 22,
	ghoulDB_GhoulySpawn = 23,
	ghoulDB_Script = 24,
	ghoulDB_ActorAttributes = 25,
	ghoulDB_FxEmitter = 26,
	ghoulDB_FxParticle = 27,
	ghoulDB_FxRumble = 28,
	ghoulDB_FxCamShape = 29
};

namespace Ghoulies {
	static const char* AssetInventoryVERSION = "version,1\n";
	static const char* AssetInventoryTABLEHEADER = "AID,VERSION,TIMESTAMP,FILEPATH,BINARY FORMAT VERSION\n";
	static const char* AssetInventoryTABLEENTRYFORMAT = "%s,%d,%u,%s,%.2f\n";

	struct AssetInventoryEntry {
	public:
		char aid[0x80] = { 0 };
		char filePath[0x108] = { 0 };
		uint32_t timeStamp = 0;
		float binaryFormatVersion = 0;
		int32_t aidVersion = 0;
	};

	struct AssetInventory {
	public:
		char* fileData = nullptr;

		float version;
		int32_t entryCount;
		int32_t unk1;
		std::vector<AssetInventoryEntry> entries;

		void ReadStandaloneFile(char* fileName);
		void ReadFromData(char* data);

		void ExportToFile(char* fileName);
	};
}

struct GhoulTextureHeader {
	int32_t format;
	int32_t headerSize;
	int16_t width, height;
	int32_t flags;
	char framerate;
	char frameCount;
	int16_t sUnk1;
	int32_t tileCount;
	int32_t chunkSize;
};

struct TextureGPUHeader {
	int32_t size;
	int32_t iunk2;
	int32_t offset;
	int32_t chunkSize;
};

struct TextureGPUEntry {
	char* textureData;

	~TextureGPUEntry();
};

struct TextureFile {
	char* texFileData;
	GhoulTextureHeader header;
	TextureGPUHeader gpuHeader;
	TextureGPUEntry* entries;
	int32_t framePos = 0;

	bool refresh = true;

	void ParseTextureHeader(char* data);
	void ParseTextureEntries(char* data);

	void FreeTextureData();
};

#endif