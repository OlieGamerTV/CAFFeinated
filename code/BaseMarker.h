#pragma once
#include "CommonReader.h"
#include "MLTypes.h"
#include "AssetId.h"

const int markerSizes[] = {
	0x34,
0x34,
0x40,
0x38,
0x7C,
0x8C,
0x12C,
0x7C,
0x8C,
0x5C,
0x34,
0x48,
0x38,
0x8C,
0x7C,
0x3C,
0x34,
0x38,
0x3C,
0x40,
0x80,
0x5C,
0x68,
0x80,
0x88,
0x4C,
0x34,
0x44,
0xD0,
0x50,
0x38,
0x4C,
0x48,
0x80,
0x78,
0x40,
0x148,
0x40,
0x44,
0x44,
0x3C,
0x44,
0x78,
0x78,
0x78,
0x40,
0x54,
0x54,
0x40,
0x40,
0x40,
0x34
};

static const char* dbMarkerNames[] = {
	"Null",
	"Dummy",
	"Live Group",
	"Variable Speed Path",
	"Player",
	"Avatar",
	"Actor",
	"Sound Source",
	"Region",
	"Emitter",
	"Walk In Camera",
	"Destination",
	"Camera Start",
	"Region Box",
	"Avatar Parameters",
	"Fake Target",
	"Throw To",
	"Evade Destination",
	"Collectable",
	"Climb Pole",
	"Camera Move To",
	"Vehicle",
	"Gate",
	"Vehicle Block",
	"Destructible Prop",
	"Target Group",
	"LIVE Starting Position",
	"Rope",
	"Portal",
	"Water Event",
	"Particle Effect",
	"Wind Area",
	"Proximity Detector",
	"Bolt Head",
	"Fixed Indicator",
	"Weight Platform",
	"Component Crate",
	"Warp Pad",
	"Dialog Emitter",
	"Enclosed Area",
	"Tight Rope",
	"Flock Region",
	"Jinjo House",
	"Jinjo Lockup",
	"Minjo Hiding Place",
	"Police Spawn Point",
	"Prop Spawn Location",
	"Townsfolk Culling Area",
	"Signpost",
	"Random Spawn Point",
	"Off Mesh Connection",
	"Multiplayer Camera"
};

enum dbMarkerGame : unsigned char {
	markerGhoulies,
	markerPinata,
	markerBanjo
};

enum dbMarkerEnum : short {
	marker_Null,
	marker_Dummy,
	marker_LiveGroup,
	marker_VariableSpeedPath,
	marker_Player,
	marker_Avatar,
	marker_Actor,
	marker_SoundSource,
	marker_Region,
	marker_Emitter,
	marker_WalkInCamera,
	marker_Destination,
	marker_CameraStart,
	marker_RegionBox,
	marker_AvatarParams,
	marker_FakeTarget,
	marker_ThrowTo,
	marker_EvadeDestination,
	marker_Collectable,
	marker_ClimbPole,
	marker_CameraMoveTo,
	marker_Vehicle,
	marker_Gate,
	marker_VehicleBlock,
	marker_DestructibleProp,
	marker_TargetGroup,
	marker_Live_StartingPos,
	marker_Rope,
	marker_Portal,
	marker_WaterEvent,
	marker_ParticleEffect,
	marker_WindArea,
	marker_ProximityDetector,
	marker_BoltHead,
	marker_FixedIndicator,
	marker_WeightPlatform,
	marker_ComponentCrate,
	marker_WarpPad,
	marker_DialogEmitter,
	marker_EnclosedArea,
	marker_TightRope,
	marker_FlockRegion,
	marker_JinjoHouse,
	marker_JinjoLockup,
	marker_MinjoHidingPlace,
	marker_PoliceSpawnPoint,
	marker_PropSpawnLocation,
	marker_TownsfolkCullingArea,
	marker_Signpost,
	marker_RandomSpawnPoint,
	marker_OffMeshConnection,
	marker_MultiPlayerCamera
};

const unsigned int ResetForChallengeFlag = 0x10000;

class MarkerBase
{
public:
	char* markerData;

	int sizeOf;
	dbMarkerEnum markerId;
	short uniqueId;
	short childId;
	short parentId;
	int challengeAid;

	char unk1;
	char unk2;
	char unk3;
	char isPosModelBase;

	MLVec pos;
	MLRot pyr;

	float scale;
	unsigned int flags;

	void readCommonMarkerData(char* data) {
		markerData = data;

		int entrySizeVar = 0;
		short typeVar = 0;
		short idxVar = 0;
		short childIdVar = 0;
		short parentIdVar = 0;
		int challengeAidVar = 0;

		float xPosVar;
		float yPosVar;
		float zPosVar;
		float xRotVar;
		float yRotVar;
		float zRotVar;

		float scaleVar;
		unsigned int flagsVar;

		memcpy(&entrySizeVar, markerData, sizeof(int));
		memcpy(&typeVar, markerData + 4, sizeof(short));
		memcpy(&idxVar, markerData + 6, sizeof(short));
		memcpy(&childIdVar, markerData + 8, sizeof(short));
		memcpy(&parentIdVar, markerData + 0xA, sizeof(short));
		memcpy(&challengeAidVar, markerData + 0xC, sizeof(int));

		memcpy(&xPosVar, markerData + 0x14, sizeof(float));
		memcpy(&yPosVar, markerData + 0x18, sizeof(float));
		memcpy(&zPosVar, markerData + 0x1C, sizeof(float));
		memcpy(&xRotVar, markerData + 0x20, sizeof(float));
		memcpy(&yRotVar, markerData + 0x24, sizeof(float));
		memcpy(&zRotVar, markerData + 0x28, sizeof(float));
		memcpy(&scaleVar, markerData + 0x2C, sizeof(float));
		memcpy(&flagsVar, markerData + 0x30, sizeof(int));

		unk1 = markerData[0x10];
		unk2 = markerData[0x11];
		unk3 = markerData[0x12];
		isPosModelBase = markerData[0x13];

		sizeOf = flipEndian(entrySizeVar);
		markerId = (dbMarkerEnum)flipEndian(typeVar);
		uniqueId = flipEndian(idxVar);
		childId = flipEndian(childIdVar);
		parentId = flipEndian(parentIdVar);
		challengeAid = flipEndian(challengeAidVar);

		pos.x = flipEndian_f32((char*)&xPosVar, SRC_ENDIANBIG);
		pos.y = flipEndian_f32((char*)&yPosVar, SRC_ENDIANBIG);
		pos.z = flipEndian_f32((char*)&zPosVar, SRC_ENDIANBIG);
		pyr.p = flipEndian_f32((char*)&xRotVar, SRC_ENDIANBIG);
		pyr.y = flipEndian_f32((char*)&yRotVar, SRC_ENDIANBIG);
		pyr.r = flipEndian_f32((char*)&zRotVar, SRC_ENDIANBIG);
		scale = flipEndian_f32((char*)&scaleVar, SRC_ENDIANBIG);
		flags = flipEndian(flagsVar);
	}
};

class MarkerAvatar : public MarkerBase {
public:
	float unk1;
	float unk2;

	void readMarkerData(char* data) {
		readCommonMarkerData(data);
	}
};

class MarkerLiveGroup : public MarkerBase {
public:
	void readMarkerData(char* data) {
		readCommonMarkerData(data);
	}
};

class MarkerVariableSpeedPath : public MarkerBase {
public:
	void readMarkerData(char* data) {
		readCommonMarkerData(data);
	}
};

// Tied to an entry type of 0x4. Defines a player spawn.
class MarkerPlayer : public MarkerBase {
public:
	int controllerId;

	void readMarkerData(char* data) {
		readCommonMarkerData(data);
	}
};

// Tied to an entry type of 0x12
class MarkerCollectible : public MarkerBase {
public:
	unsigned int objParamAid = 0;
	int collectibleID = 0;

	void readMarkerData(char* data) {
		readCommonMarkerData(data);

		memcpy(&objParamAid, markerData + 0x34, sizeof(int));
		memcpy(&collectibleID, markerData + 0x38, sizeof(int));

		objParamAid = flipEndian(objParamAid);
		collectibleID = flipEndian(collectibleID);
	}
};

class MarkerWeapon : public MarkerBase {
public:
	unsigned int parameters;
	unsigned int spawnActorAid;
	unsigned int attributes;
	unsigned int aidModelExtraAvatar;
	int droneAction;
	int spawnId;
	int probability;
	float proximityRadius;
	int proximityProb;
	int proximityOnceOnly;
	int cannotPickup;
	int cannotAttack;
	float scareScale;
	int respawn;
	float respawnDelay;
	int insideBreakable;

	void readMarkerData(char* data) {
		readCommonMarkerData(data);
	}
};

class MarkerVehicle : public MarkerBase {
public:
	int playerId;
	unsigned int aid;
	unsigned int addPlayer;

	void readMarkerData(char* data) {
		readCommonMarkerData(data);

		int playerIdVar;
		unsigned int aidVar;
		unsigned int addPlayerVar;

		memcpy(&playerIdVar, markerData + 0x3C, sizeof(int));
		memcpy(&aidVar, markerData + 0x40, sizeof(int));
		memcpy(&addPlayerVar, markerData + 0x44, sizeof(int));

		playerId = flipEndian(playerIdVar);
		aid = flipEndian(aidVar);
		addPlayer = flipEndian(addPlayerVar);
	}
};

class MarkerPortal : public MarkerBase {
public:
	unsigned int objParamAid;
	unsigned int scriptAid;
	unsigned int cutsceneAid;
	char gameFlag[0x40];
	int jiggyRequirement;
	unsigned int flagListAid;
	char sceneIndicator[0x40];

	void readMarkerData(char* data) {
		readCommonMarkerData(data);

		memset(gameFlag, 0, 0x40);
		memset(sceneIndicator, 0, 0x40);

		unsigned int objParamAidVar;
		unsigned int scriptAidVar;
		unsigned int cutsceneAidVar;
		unsigned int flagListAidVar;

		memcpy(&objParamAid, markerData + 0x3C, sizeof(int));
		memcpy(&scriptAid, markerData + 0x40, sizeof(int));
		memcpy(&cutsceneAid, markerData + 0x44, sizeof(int));
		strncpy(gameFlag, markerData + 0x48, 0x40);
		memcpy(&jiggyRequirement, markerData + 0x88, sizeof(int));
		memcpy(&flagListAid, markerData + 0x8C, sizeof(int));
		strncpy(sceneIndicator, markerData + 0x90, 0x40);

		objParamAid = flipEndian(objParamAid);
		scriptAid = flipEndian(scriptAid);
		cutsceneAid = flipEndian(cutsceneAid);
		flagListAid = flipEndian(flagListAid);
		jiggyRequirement = flipEndian(jiggyRequirement);
	}
};

class MarkerComponentCrate : public MarkerBase {
public:
	unsigned int objParamAid;
	unsigned int blockSetAid;
	char collectedCrate[0x40];
	char unlockedCrate[0x40];
	int flag;
	char useColorPalette;
	char colorPaletteId;
	char unk1;
	char unk2;

	char sceneIndicator[0x40];

	unsigned int blockageAid;
	char blockadeGameFlag[0x40];

	void readMarkerData(char* data) {
		readCommonMarkerData(data);

		memset(collectedCrate, 0, 0x40);
		memset(unlockedCrate, 0, 0x40);
		memset(sceneIndicator, 0, 0x40);
		memset(blockadeGameFlag, 0, 0x40);

		unsigned int objParamAidVar = 0;
		unsigned int blockSetAidVar = 0;
		int flagVar = 0;

		memcpy(&objParamAidVar, markerData + 0x34, sizeof(int));
		memcpy(&blockSetAidVar, markerData + 0x38, sizeof(int));
		strncpy(collectedCrate, markerData + 0x3C, 0x40);
		strncpy(unlockedCrate, markerData + 0x7C, 0x40);

		objParamAid = flipEndian(objParamAidVar);
		blockSetAid = flipEndian(blockSetAidVar);
	}
};

MarkerBase* CreateMarkerData(dbMarkerEnum markerType) {
	switch (markerType) {
	case marker_Collectable: { printf("COLLECTIBLE\n"); return new MarkerCollectible(); } break;
	case marker_Portal: { printf("PORTAL\n"); return new MarkerPortal(); } break;
	case marker_ComponentCrate: { printf("COMPONENT CRATE\n"); return new MarkerComponentCrate(); } break;
	default: { printf("BASE\n"); return new MarkerBase(); } break;
	}
}

class MarkerFile {
public:
	char* fileData;

	bool isReady = false;

	int numOfMarkerEntries = 0;
	MarkerBase** markerEntries;

	void ReadMarkerFile(char* data) {
		if (data == nullptr) return;
		fileData = data;

		bool hasInitialEntryStarted = false;
		int offs = 0;

		// settle the marker count.
		while (true) {
			int size = 0;
			short id = 0;

			memcpy(&size, fileData + offs, 4);
			memcpy(&id, fileData + offs + 6, 2);

			size = flipEndian(size);
			id = flipEndian(id);

			printf("Entry %d\t->\t%d\n", id, size);

			char* markerData = (char*)malloc(size);

			numOfMarkerEntries++;
			if (id == 1 && hasInitialEntryStarted == true) {
				break;
			}

			if (id == 1 && hasInitialEntryStarted == false) {
				hasInitialEntryStarted = true;
			}

			offs += size;
		}

		offs = 0;
		markerEntries = new MarkerBase*[numOfMarkerEntries];

		// set up the markers.
		for (int i = 0; i < numOfMarkerEntries; i++) {
			int size = 0;
			short type = 0;

			memcpy(&size, fileData + offs, 4);
			memcpy(&type, fileData + offs + 4, 2);

			size = flipEndian(size);
			type = flipEndian(type);

			char* markerData = (char*)malloc(size);
			memcpy(markerData, fileData + offs, size);
			markerEntries[i] = CreateMarkerData((dbMarkerEnum)type);

			markerEntries[i]->readCommonMarkerData(markerData);

			printf("Entry %05d\t->\t[%s ", markerEntries[i]->uniqueId, dbMarkerNames[markerEntries[i]->markerId]);
			printf("Position (X %.04f, Y %.04f, Z %.04f) ", markerEntries[i]->pos.x, markerEntries[i]->pos.y, markerEntries[i]->pos.z);
			printf("Rotation (P %.04f, Y %.04f, R %.04f) ", markerEntries[i]->pyr.p, markerEntries[i]->pyr.y, markerEntries[i]->pyr.r);
			printf("Scale %.04f ", markerEntries[i]->scale);

			printf("Child %d / Parent %d ", markerEntries[i]->childId, markerEntries[i]->parentId);
			printf("Challenge IDX %08x ", markerEntries[i]->challengeAid);

			if (markerEntries[i]->flags != 0) {
				printf("Active Flags [ ");
				if ((markerEntries[i]->flags & ResetForChallengeFlag) == 0) {
					printf("Reset for Challenge ");
				}

				printf("] ");
			}

			printf("]\n");

			switch (markerEntries[i]->markerId) {
			    case marker_ComponentCrate: {
					((MarkerComponentCrate*)markerEntries[i])->readMarkerData(markerData);
			    }
				break;
				case marker_Portal: {
					((MarkerPortal*)markerEntries[i])->readMarkerData(markerData);
				}
			    break;
			}

			offs += size;
			free(markerData);
		}
	}
};