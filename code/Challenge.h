#pragma once
#ifndef _CHALLENGE
#define _CHALLENGE
#include <cstdint>

static const char* dbChallengeNames[] = {
    "NULL",
    "Common",
    "GroupActivator",
    "_UNUSED3",
    "GetThemOut",
    "RoundCourse",
    "Builder",
    "Distance",
    "Collect",
    "CountManoeuvres",
    "DestroyObjects",
    "UpInTheAir",
    "Repair",
    "ObjectInteract",
    "VehicleStats",
    "Damage",
    "Speed",
    "StayInVehicle",
    "KeepyUppy",
    "PlayDialog",
    "ActivateGameRef",
    "StayOffFloor",
    "ActivateSequence",
    "Flying",
    "TalkToNPC",
    "ResetToGameRef",
    "Skidmarks",
    "Sumo",
    "QueenOfTheHill",
    "CheckScore",
    "CountContacts",
    "PauseTimer",
    "DestructionDerby",
    "PenaltyObj",
    "Skittles",
    "AffectVehicles",
    "MultiPlayerResetPlayerRequirements",
    "MultiPlayerResetObjects",
    "MultiPlayerShowUI",
    "MultiPlayerAwardPoints",
    "AlterGravity",
    "Protect",
    "Park",
    "Quiz",
    "SetScoreData"
};

enum dbChallengeRequirement_e : int32_t {
    dbChallengeRequirement_NULL = 0,
    dbChallengeRequirement_Common = 1,
    dbChallengeRequirement_GroupActivator = 2,
    dbChallengeRequirement_UNUSED3 = 3,
    dbChallengeRequirement_GetThemOut = 4,
    dbChallengeRequirement_RoundCourse = 5,
    dbChallengeRequirement_Builder = 6,
    dbChallengeRequirement_Distance = 7,
    dbChallengeRequirement_Collect = 8,
    dbChallengeRequirement_CountManoeuvres = 9,
    dbChallengeRequirement_DestroyObjects = 0xA,
    dbChallengeRequirement_UpInTheAir = 0xB,
    dbChallengeRequirement_Repair = 0xC,
    dbChallengeRequirement_ObjectInteract = 0xD,
    dbChallengeRequirement_VehicleStats = 0xE,
    dbChallengeRequirement_Damage = 0xF,
    dbChallengeRequirement_Speed = 0x10,
    dbChallengeRequirement_StayInVehicle = 0x11,
    dbChallengeRequirement_KeepyUppy = 0x12,
    dbChallengeRequirement_PlayDialog = 0x13,
    dbChallengeRequirement_ActivateGameRef = 0x14,
    dbChallengeRequirement_StayOffFloor = 0x15,
    dbChallengeRequirement_ActivateSequence = 0x16,
    dbChallengeRequirement_Flying = 0x17,
    dbChallengeRequirement_TalkToNPC = 0x18,
    dbChallengeRequirement_ResetToGameRef = 0x19,
    dbChallengeRequirement_Skidmarks = 0x1A,
    dbChallengeRequirement_Sumo = 0x1B,
    dbChallengeRequirement_QueenOfTheHill = 0x1C,
    dbChallengeRequirement_CheckScore = 0x1D,
    dbChallengeRequirement_CountContacts = 0x1E,
    dbChallengeRequirement_PauseTimer = 0x1F,
    dbChallengeRequirement_DestructionDerby = 0x20,
    dbChallengeRequirement_PenaltyObj = 0x21,
    dbChallengeRequirement_Skittles = 0x22,
    dbChallengeRequirement_AffectVehicles = 0x23,
    dbChallengeRequirement_MultiPlayerResetPlayerRequirements = 0x24,
    dbChallengeRequirement_MultiPlayerResetObjects = 0x25,
    dbChallengeRequirement_MultiPlayerShowUI = 0x26,
    dbChallengeRequirement_MultiPlayerAwardPoints = 0x27,
    dbChallengeRequirement_AlterGravity = 0x28,
    dbChallengeRequirement_Protect = 0x29,
    dbChallengeRequirement_Park = 0x2A,
    dbChallengeRequirement_Quiz = 0x2B,
    dbChallengeRequirement_SetScoreData = 0x2C
};

class ChallengeHeader {
public:
    dbChallengeRequirement_e reqId = dbChallengeRequirement_NULL;
    int32_t unk1 = 0;
    int32_t taskID = 0;
    int32_t size = 0;
};

class ChallengeNullDef {
public:
    ChallengeHeader header;

    void ParseHeader(char* data);
};

class ChallengeCommonDef : public ChallengeNullDef
{
public:
    // Challenge Strings
    char worldNameTag[0x20] = {"\0"}; // 0x10
    char challengeTag[0x20] = { "\0" }; // 0x30
    char gameStyleTag[0x20] = { "\0" }; // 0x50

    // Challenge Flags & Counters
    char seenObjectivesFlag[0x40] = { "\0" }; // 0xD8
    char gameStyleFlag[0x40] = { "\0" }; // 0x118
    char notesOnlyFlag[0x40] = { "\0" }; // 0x158
    char beatenFlag[0x40] = { "\0" }; // 0x198
    char beatenCPlusFlag[0x40] = { "\0" }; // 0x1D8
    char jiggiesWonCounter[0x40] = { "\0" }; // 0x218
    char highScoreCounter[0x40] = { "\0" }; // 0x25C
    char specialObjectTag01[0x40] = { "\0" }; // 0x30C
    char specialObjectIndicator01[0x40] = { "\0" }; // 0x34C
    char specialObjectTag02[0x40] = { "\0" }; // 0x38C
    char specialObjectIndicator02[0x40] = { "\0" }; // 0x3CC
    char* npcReferences = nullptr; // 0x40C

    // General Settings
    uint32_t gameRefID = 0; // 0x70
    uint32_t removeGameRefID = 0; // 0x74
    uint32_t defaultVehicleUUID = 0; // 0x78
    int32_t maxPlayerCount = 0; // 0x7C
    int32_t teleportToStart = 0; // 0x80
    int32_t teleportOverrideMarker = 0; // 0x84
    int32_t mustPickVehicle = 0; // 0x88
    int32_t shouldPauseDuringCountdown = 0; // 0x8C
    int32_t challengeSfxOverride = 0; // 0xB4
    float tuneVolume = 0; // 0xB8
    int32_t numJiggiesAwarded = 0; // 0x258

    // Dialog Settings
    uint32_t challengeGreetingIntroDialogUUID = 0; // 0xBC
    uint32_t challengeBeatenIntroDialogUUID = 0; // 0xC0
    uint32_t challengeObjectivesDialogUUID = 0; // 0xC4
    uint32_t challengeWinDialogUUID = 0; // 0xC8
    uint32_t challengeLoseDialogUUID = 0; // 0xCC
    uint32_t challengeStyleDialogAid = 0; // 0xD0
    uint32_t challengeWimpOutDialogAid = 0; // 0xD4

    // Challenge-Specific Settings
    int32_t challengeRestrictionsAidUUID = 0; // 0xA0
    int32_t resultsOrdering = 0; // 0x2AC
    int32_t isRace = 0; // 0x2BC
    int32_t isHealthChallenge = 0; // 0x2C0
    uint32_t scoreIcon = 0; // 0x2C4
    int32_t shouldDisableRankings = 0; // 0x2E0
    int32_t targetScore = 0; // 0x2E4
    int32_t teamTargetScoreType = 0; // 0x2F0
    int32_t teamTargetScore = 0; // 0x2F4
    float targetTime = 0; // 0x2EC
    int32_t shouldPassWhenTargetTimeReached = 0; // 0x2F8
    int32_t hasRoundInfo = 0; // 0x300
    int32_t totalRounds = 0; // 0x304

    // UI Settings
    int32_t isJinjoChallenge = 0; // 0x2A0
    int32_t isTTChallenge = 0; // 0x2A4
    int32_t mustShowTimer = 0; // 0x2D8
    int32_t mustShowScore = 0; // 0x2DC
    
    float ttMinTime = 0; // 0x2C8
    float ttMaxJiggyMinTime = 0; // 0x2CC
    float jiggyMaxNotesMinTime = 0; // 0x2D0
    float notesMaxTime = 0; // 0x2D4

    void ParseCommon(char* data);
};

class challengeGroupActivator : public ChallengeNullDef
{
public:
    int32_t getCondition = 0; // 0x10
    int32_t onCompletionOf = 0; // 0x14
    float timeLimit = 0; // 0x18
    int32_t incompleteIsPass = 0; // 0x1C
    int32_t onPassActivate = 0; // 0x20
    int32_t onFailActivate = 0; // 0x24
    int32_t shouldSendReaction = 0; // 0x28
    int32_t shouldShowReadyGoUI = 0; // 0x2C
};

class challengeDistance : public ChallengeNullDef // 0x11
{
public:
    float targetDistance = 0; // 0x10
    int32_t regionId = 0;
    int32_t shouldRemoveControlWhenLeavingRegion = 0;
    int32_t shouldDisablePropulsionAndGadgetsWhenLeavingRegion = 0;
    int32_t sectionId = 0;
    char startGateIndicator[0x40] = { "\0" };
    int32_t shouldShowBalls = 0;
    int32_t unk_68 = 0;
    int32_t shouldShowRegion = 0;
    int32_t shouldShowTargetRegion = 0;
    float maxTimeStationary = 0;
    float speedForStationary = 0;
    float speedForStationaryInWater = 0;
    int32_t mustRemainInVehicleAfterGate = 0;
    float unk_84 = 0;
    int32_t shouldCreateScoreHolograms = 0;

    void ParseDistance(char* data);
};

class challengeStayInVehicle : public ChallengeNullDef // 0x11
{
public:
    int32_t canPlayersGetOut = 0; // 0x10
    int32_t canBaddiesGetOut = 0; // 0x14

    void ParseStayInVehicle(char* data);
};

class challengePlayDialog : public ChallengeNullDef // 0x13
{
public:
    float timeDelay = 0;
    int32_t dialogAid = 0;
    int32_t shouldPassWhenDone = 0;
    
    void ParsePlayDialog(char* data);
};

class challengeTalkToNPC : public ChallengeNullDef // 0x18
{
public:
    char npcTag[0x40]; // 0x10
    char indicatorType[0x40]; // 0x50
};

class challengeSkidmarks : public ChallengeNullDef // 0x1A
{
public:
    float distToSkid; // 0x10
    int32_t regionId; // 0x14
    int32_t shouldShowRegion; // 0x18
    char indicatorType[0x40]; // 0x1C

    void ParseSkidmarks(char* data);
};

class challengeQueenOfTheHill : public ChallengeNullDef // 0x1C
{
public:
    int32_t avatarUUID = 0;
    int32_t startCount = 0;

    char sceneIndicator[0x40] = { "\0" };

    int32_t hasNumbers = 0;
    int32_t numberModelUUIDs[10];
    float width = 0, length = 0, height = 0;
};

class challengeAffectVehicles : public ChallengeNullDef // 0x23
{
public:
    int32_t toggle = 0;

    void ParseAffectVehicles(char* data);
};

#endif