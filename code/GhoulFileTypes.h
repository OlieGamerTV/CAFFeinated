#pragma once

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

enum GhouliesDBType : int {
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

struct GhoulTextureHeader {
	int format;
	int headerSize;
	short width, height;
	int flags;
	char framerate;
	char frameCount;
	short sUnk1;
	int tileCount;
	int chunkSize;
};

struct TextureGPUHeader {
	int size;
	int iunk2;
	int offset;
	int chunkSize;
};

struct TextureGPUEntry {
	char* textureData;
};

struct TextureFile {
	char* texFileData;
	GhoulTextureHeader header;
	TextureGPUHeader gpuHeader;
	TextureGPUEntry* entries;
	int framePos = 0;

	bool refresh = true;

	void ParseTextureHeader(char* data);
	void ParseTextureEntries(char* data);
};