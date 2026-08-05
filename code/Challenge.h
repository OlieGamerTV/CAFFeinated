#pragma once

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

enum dbChallengeRequirement_e : int {
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
    int unk1 = 0;
    int taskID = 0;
    int size = 0;
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
    int gameRefID = 0; // 0x70
    int removeGameRefID = 0; // 0x74
    int defaultVehicleUUID = 0; // 0x78
    int mustPickVehicle = 0; // 0x88
    int maxPlayerCount = 0; // 0x7C
    int teleportToStart = 0; // 0x80
    int teleportOverrideMarker = 0; // 0x84
    int shouldPauseDuringCountdown = 0; // 0x8C
    int challengeSfxOverride = 0; // 0xB4
    float tuneVolume = 0; // 0xB8
    int numJiggiesAwarded = 0; // 0x258

    // Dialog Settings
    int challengeObjectivesDialogUUID = 0; // 0xC4
    int challengeWinDialogUUID = 0; // 0xC8
    int challengeLoseDialogUUID = 0; // 0xCC

    // Challenge-Specific Settings
    int challengeRestrictionsAidUUID = 0; // 0xA0
    int resultsOrdering = 0; // 0x2AC
    int isRace = 0; // 0x2BC
    int isHealthChallenge = 0; // 0x2C0
    int targetScore = 0; // 0x2E4
    int teamTargetScoreType = 0; // 0x2F0
    int teamTargetScore = 0; // 0x2F4
    float targetTime = 0; // 0x2EC
    int shouldPassWhenTargetTimeReached = 0; // 0x2F8
    int hasRoundInfo = 0; // 0x300
    int totalRounds = 0; // 0x304

    // UI Settings
    int isJinjoChallenge = 0; // 0x2A0
    int isTTChallenge = 0; // 0x2A4
    int mustShowTimer = 0; // 0x2D8
    int mustShowScore = 0; // 0x2DC

    void ParseCommon(char* data);
};

class challengeGroupActivator : public ChallengeNullDef
{
public:
    int getCondition = 0; // 0x10
    int onCompletionOf = 0; // 0x14
    float timeLimit = 0; // 0x18
    int incompleteIsPass = 0; // 0x1C
    int onPassActivate = 0; // 0x20
    int onFailActivate = 0; // 0x24
    int shouldSendReaction = 0; // 0x28
    int shouldShowReadyGoUI = 0; // 0x2C
};

class challengeDistance : public ChallengeNullDef // 0x11
{
public:
    float targetDistance = 0; // 0x10
    int regionId = 0;
    int shouldRemoveControlWhenLeavingRegion = 0;
    int shouldDisablePropulsionAndGadgetsWhenLeavingRegion = 0;
    int sectionId = 0;
    char startGateIndicator[0x40] = { "\0" };
    int shouldShowBalls = 0;
    int unk_68 = 0;
    int shouldShowRegion = 0;
    int shouldShowTargetRegion = 0;
    float maxTimeStationary = 0;
    float speedForStationary = 0;
    float speedForStationaryInWater = 0;
    int mustRemainInVehicleAfterGate = 0;
    float unk_84 = 0;
    int shouldCreateScoreHolograms = 0;

    void ParseDistance(char* data);
};

class challengeStayInVehicle : public ChallengeNullDef // 0x11
{
public:
    int canPlayersGetOut = 0; // 0x10
    int canBaddiesGetOut = 0; // 0x14

    void ParseStayInVehicle(char* data);
};

class challengePlayDialog : public ChallengeNullDef // 0x13
{
public:
    float timeDelay = 0;
    int dialogAid = 0;
    int shouldPassWhenDone = 0;
    
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
    int regionId; // 0x14
    int shouldShowRegion; // 0x18
    char indicatorType[0x40]; // 0x1C

    void ParseSkidmarks(char* data);
};

class challengeQueenOfTheHill : public ChallengeNullDef // 0x1C
{
public:
    int avatarUUID = 0;
    int startCount = 0;

    char sceneIndicator[0x40] = { "\0" };

    int hasNumbers = 0;
    int numberModelUUIDs[10];
    float width = 0, length = 0, height = 0;
};

class challengeAffectVehicles : public ChallengeNullDef // 0x23
{
public:
    int toggle = 0;

    void ParseAffectVehicles(char* data);
};