#pragma once
#include "CommonReader.h"

static const char* dbScriptNames[] = {
	"dbScript_Null", // Only used for the end of the script.
	"dbScript_Setup_SetBackground",
	"dbScript_Setup_CreateSceneController",
	"dbScript_Setup_PreLoadAsset",
	"dbScript_Setup_SetFlags",
	"dbScript_Setup_SetMaxFrameRate",
	"dbScript_Setup_SetPlayerEnergy",
	"dbScript_Setup_SetCharacterVisibility",
	"dbScript_Setup_SetCharacterNoAttackTime",
	"dbScript_Setup_DisplaySceneName",
	"dbScript_Debug_PlayAs",
	"dbScript_Debug_RunSceneTests",
	"dbScript_Execute_Asset",
	"dbScript_Section_Begin",
	"dbScript_Section_End",
	"dbScript_Actors_Freeze",
	"dbScript_Play_Cutscene",
	"dbScript_Play_Dialog",
	"dbScript_Play_Movie",
	"dbScript_Play_MissingCutscene",
	"dbScript_Player_RemoveFromScene",
	"dbScript_Player_InsertIntoScene",
	"dbScript_Player_AttackableState",
	"dbScript_Player_GiveWeapon", // Leftover from Ghoulies
	"dbScript_Weapons_PickUpState",
	"dbScript_Music_StartTune",
	"dbScript_Music_StopTune",
	"dbScript_Condition_End",
	"dbScript_Condition_TimeOut",
	"dbScript_Condition_NumberGhouliesAlive", // Leftover from Ghoulies
	"dbScript_Condition_ObjectPickedUp",
	"dbScript_Condition_NumberGhouliesKilled",
	"dbScript_Condition_NumberOfKnockdowns",
	"dbScript_Player_RemoveWeapon",
	"dbScript_Condition_ActorEntersRegion",
	"dbScript_Condition_PlayerEntersRegion",
	"dbScript_Condition_PlayerLeavesRegion",
	"dbScript_Set_DynamicFlag",
	"dbScript_Clear_DynamicFlag",
	"dbScript_Set_Light_Parameters",
	"dbScript_Set_Reverb_Preset",
	"dbScript_Misc_SwapObject",
	"dbScript_Misc_SwapMarker",
	"dbScript_Actors_Activate",
	"dbScript_Setup_SetSkyDome",
	"dbScript_Music_StartAmbientTune",
	"dbScript_Actors_SetStrategy",
	"dbScript_Weapons_Activate",
	"dbScript_Condition_SpecificGhouliesAlive",
	"dbScript_Condition_SpecificGhouliesKilled",
	"dbScript_Avatar_Activate",
	"dbScript_Misc_RemoveObject",
	"dbScript_Setup_CameraParameters",
	"dbScript_Misc_MoveCameraToPoint",
	"dbScript_Condition_ObjectRemoved",
	"dbScript_Condition_PlayerDeflectionContact",
	"dbScript_Condition_PlayerHitGhouly",
	"dbScript_Condition_GhoulyTriggered",
	"dbScript_Condition_GhoulyExists",
	"dbScript_Player_WaitUntilNotDroning",
	"dbScript_Avatar_Explode",
	"dbScript_Misc_RandomiseMarkers",
	"dbScript_Misc_ActivateSceneControllers",
	"dbScript_Player_InputDisable",
	"dbScript_Player_InputEnable",
	"dbScript_Set_Shadow_DefaultLightDirection",
	"dbScript_Actors_RemoveFromScene",
	"dbScript_Player_WarpToStartMarker",
	"dbScript_Actors_ChangeDroneAction",
	"dbScript_Background_SetSwitch",
	"dbScript_Play_Rumble",
	"dbScript_Play_Camshake",
	"dbScript_Misc_SetGameFlag",
	"dbScript_Actors_SetSwitch",
	"dbScript_Player_Celebrate",
	"dbScript_Misc_SetPadHardwareFrame",
	"dbScript_Setup_CreateSceneControllerWithParams",
	"dbScript_Setup_SetBackgroundParams",
	"dbScript_Setup_SetMarkerAsset",
	"dbScript_Setup_AddWeatherComponent",
	"dbScript_Debug_Printf",
	"dbScript_Debug_Pause",
	"dbScript_Execute_ScriptAsset",
	"dbScript_Set_Fog_Parameters",
	"dbScript_Set_DepthOfField",
	"dbScript_Set_Light2_Parameters",
	"dbScript_Setup_SetAppSceneId",
	"dbScript_Actors_ScriptedActionBegin",
	"dbScript_Condition_PlayerTalksToNPC",
	"dbScript_Condition_PlayerEntersVehicle",
	"dbScript_Execute_PushScriptAsset",
	"dbScript_Execute_PopScript",
	"dbScript_Debug_EndSceneTest",
	"dbScript_Logic_IfGameflag",
	"dbScript_Logic_Else",
	"dbScript_Logic_Endif",
	"dbScript_Play_Challenge",
	"dbScript_Misc_SequenceStart",
	"dbScript_Misc_SequenceStop",
	"dbScript_Logic_IfChallengeResult",
	"dbScript_Logic_IfDialogResponse",
	"dbScript_Misc_ExecuteMarkers",
	"dbScript_Misc_RemoveMarkers",
	"dbScript_Condition_IfGameFlag",
	"dbScript_Debug_WriteHighScores",
	"dbScript_Set_Volumetric_Cloud_Parameters",
	"dbScript_Play_CutCam",
	"dbScript_Set_Rain_Intensity",
	"dbScript_Set_Glow_Parameters",
	"dbScript_Actors_MoveToMarker",
	"dbScript_Misc_SetChallengeBeaten",
	"dbScript_Actors_NPC_Data",
	"dbScript_Player_SetSelectedBlueprint",
	"dbScript_Flow_WaitForObjectsCreated",
	"dbScript_Setup_AddPollen",
	"dbScript_Setup_AddButterflies",
	"dbScript_Set_DynamicWater_Parameters",
	"dbScript_Misc_FindObjects",
	"dbScript_Logic_IfNumObjectsFound",
	"dbScript_Misc_RegisterObjects",
	"dbScript_Play_FullViewportScene",
	"dbScript_Set_Wind_Parameters",
	"dbScript_Set_Wind_Particles",
	"dbScript_Setup_PushNewScene",
	"dbScript_Setup_PopScene",
	"dbScript_Set_DynamicWater2_Parameters",
	"dbScript_Set_Reflected_Light_Parameters",
	"dbScript_Set_ReferenceAnimRot",
	"dbScript_Set_EnvCubeMap",
	"dbScript_UI_Open_InfoPoint",
	"dbScript_Set_GameWorld",
	"dbScript_Set_Volumetric_Cloud_Override_Shadow_Gain_Max_Intensity",
	"dbScript_Setup_TextureAllocator",
	"dbScript_Music_StartChannelFadeTune",
	"dbScript_Flow_LoadNewScene",
	"dbScript_Set_Shadow_Parameters",
	"dbScript_Set_Reference_Hidden",
	"dbScript_Flow_WaitOnGameFlag",
	"dbScript_Flow_WaitOnUIClosed",
	"dbScript_Actors_DroneToMarker",
	"dbScript_Condition_VehicleIsUpright",
	"dbScript_Misc_CreateGolfCart",
	"dbScript_Logic_IfNoVehiclesInWorld",
	"dbScript_Condition_ObjectEntersRegion",
	"dbScript_Condition_CounterGreaterThan",
	"dbScript_Set_HDR_And_Bloom_Params",
	"dbScript_Setup_Reverb",
	"dbScript_Play_OpenCommonShop",
	"dbScript_Set_LightingUniqueId",
	"dbScript_Logic_IfRandomFloatLessThanOrEqualsProb",
	"dbScript_Set_DynamicWater2_AnimationState",
	"dbScript_Misc_AwardNotes",
	"dbScript_Set_ReferencePhysicsMotor",
	"dbScript_Set_DynamicWater2_PhysicsSetup",
	"dbScript_Setup_LoadDigits",
	"dbScript_Set_Membrane_Params",
	"dbScript_UI_Open_ComponentInfo",
	"dbScript_Setup_ActivateEffectLocator",
	"dbScript_Set_Blobs_Liquid_Params",
	"dbScript_Set_Reference_Switch",
	"dbScript_Set_Water_Colour",
	"dbScript_Set_GlobalRPCValue",
	"dbScript_Play_PreCueMovies",
	"dbScript_Actors_ShowHide",
	"dbScript_Misc_HideObjects",
	"dbScript_Play_MovieLocalised",
	"dbScript_Set_BackgroundObjectsStatic",
	"dbScript_Play_MovieRandomSelection"
};

enum dbScript_BanjoXEnum : int {
	dbScript_Null, // Only used for the end of the script.
	dbScript_Setup_SetBackground,
	dbScript_Setup_CreateSceneController,
	dbScript_Setup_PreLoadAsset,
	dbScript_Setup_SetFlags,
	dbScript_Setup_SetMaxFrameRate,
	dbScript_Setup_SetPlayerEnergy,
	dbScript_Setup_SetCharacterVisibility,
	dbScript_Setup_SetCharacterNoAttackTime,
	dbScript_Setup_DisplaySceneName,
	dbScript_Debug_PlayAs,
	dbScript_Debug_RunSceneTests,
	dbScript_Execute_Asset,
	dbScript_Section_Begin,
	dbScript_Section_End,
	dbScript_Actors_Freeze,
	dbScript_Play_Cutscene,
	dbScript_Play_Dialog,
	dbScript_Play_Movie,
	dbScript_Play_MissingCutscene,
	dbScript_Player_RemoveFromScene,
	dbScript_Player_InsertIntoScene,
	dbScript_Player_AttackableState,
	dbScript_Player_GiveWeapon,
	dbScript_Weapons_PickUpState,
	dbScript_Music_StartTune,
	dbScript_Music_StopTune,
	dbScript_Condition_End,
	dbScript_Condition_TimeOut,
	dbScript_Condition_NumberGhouliesAlive,
	dbScript_Condition_ObjectPickedUp,
	dbScript_Condition_NumberGhouliesKilled,
	dbScript_Condition_NumberOfKnockdowns,
	dbScript_Player_RemoveWeapon,
	dbScript_Condition_ActorEntersRegion,
	dbScript_Condition_PlayerEntersRegion,
	dbScript_Condition_PlayerLeavesRegion,
	dbScript_Set_DynamicFlag,
	dbScript_Clear_DynamicFlag,
	dbScript_Set_Light_Parameters,
	dbScript_Set_Reverb_Preset,
	dbScript_Misc_SwapObject,
	dbScript_Misc_SwapMarker,
	dbScript_Actors_Activate,
	dbScript_Setup_SetSkyDome,
	dbScript_Music_StartAmbientTune,
	dbScript_Actors_SetStrategy,
	dbScript_Weapons_Activate,
	dbScript_Condition_SpecificGhouliesAlive,
	dbScript_Condition_SpecificGhouliesKilled,
	dbScript_Avatar_Activate,
	dbScript_Misc_RemoveObject,
	dbScript_Setup_CameraParameters,
	dbScript_Misc_MoveCameraToPoint,
	dbScript_Condition_ObjectRemoved,
	dbScript_Condition_PlayerDeflectionContact,
	dbScript_Condition_PlayerHitGhouly,
	dbScript_Condition_GhoulyTriggered,
	dbScript_Condition_GhoulyExists,
	dbScript_Player_WaitUntilNotDroning,
	dbScript_Avatar_Explode,
	dbScript_Misc_RandomiseMarkers,
	dbScript_Misc_ActivateSceneControllers,
	dbScript_Player_InputDisable,
	dbScript_Player_InputEnable,
	dbScript_Set_Shadow_DefaultLightDirection,
	dbScript_Actors_RemoveFromScene,
	dbScript_Player_WarpToStartMarker,
	dbScript_Actors_ChangeDroneAction,
	dbScript_Background_SetSwitch,
	dbScript_Play_Rumble,
	dbScript_Play_Camshake,
	dbScript_Misc_SetGameFlag,
	dbScript_Actors_SetSwitch,
	dbScript_Player_Celebrate,
	dbScript_Misc_SetPadHardwareFrame,
	dbScript_Setup_CreateSceneControllerWithParams,
	dbScript_Setup_SetBackgroundParams,
	dbScript_Setup_SetMarkerAsset,
	dbScript_Setup_AddWeatherComponent,
	dbScript_Debug_Printf,
	dbScript_Debug_Pause,
	dbScript_Execute_ScriptAsset,
	dbScript_Set_Fog_Parameters,
	dbScript_Set_DepthOfField,
	dbScript_Set_Light2_Parameters,
	dbScript_Setup_SetAppSceneId,
	dbScript_Actors_ScriptedActionBegin,
	dbScript_Condition_PlayerTalksToNPC,
	dbScript_Condition_PlayerEntersVehicle,
	dbScript_Execute_PushScriptAsset,
	dbScript_Execute_PopScript,
	dbScript_Debug_EndSceneTest,
	dbScript_Logic_IfGameflag,
	dbScript_Logic_Else,
	dbScript_Logic_Endif,
	dbScript_Play_Challenge,
	dbScript_Misc_SequenceStart,
	dbScript_Misc_SequenceStop,
	dbScript_Logic_IfChallengeResult,
	dbScript_Logic_IfDialogResponse,
	dbScript_Misc_ExecuteMarkers,
	dbScript_Misc_RemoveMarkers,
	dbScript_Condition_IfGameFlag,
	dbScript_Debug_WriteHighScores,
	dbScript_Set_Volumetric_Cloud_Parameters,
	dbScript_Play_CutCam,
	dbScript_Set_Rain_Intensity,
	dbScript_Set_Glow_Parameters,
	dbScript_Actors_MoveToMarker,
	dbScript_Misc_SetChallengeBeaten,
	dbScript_Actors_NPC_Data,
	dbScript_Player_SetSelectedBlueprint,
	dbScript_Flow_WaitForObjectsCreated,
	dbScript_Setup_AddPollen,
	dbScript_Setup_AddButterflies,
	dbScript_Set_DynamicWater_Parameters,
	dbScript_Misc_FindObjects,
	dbScript_Logic_IfNumObjectsFound,
	dbScript_Misc_RegisterObjects,
	dbScript_Play_FullViewportScene,
	dbScript_Set_Wind_Parameters,
	dbScript_Set_Wind_Particles,
	dbScript_Setup_PushNewScene,
	dbScript_Setup_PopScene,
	dbScript_Set_DynamicWater2_Parameters,
	dbScript_Set_Reflected_Light_Parameters,
	dbScript_Set_ReferenceAnimRot,
	dbScript_Set_EnvCubeMap,
	dbScript_UI_Open_InfoPoint,
	dbScript_Set_GameWorld,
	dbScript_Set_Volumetric_Cloud_Override_Shadow_Gain_Max_Intensity,
	dbScript_Setup_TextureAllocator,
	dbScript_Music_StartChannelFadeTune,
	dbScript_Flow_LoadNewScene,
	dbScript_Set_Shadow_Parameters,
	dbScript_Set_Reference_Hidden,
	dbScript_Flow_WaitOnGameFlag,
	dbScript_Flow_WaitOnUIClosed,
	dbScript_Actors_DroneToMarker,
	dbScript_Condition_VehicleIsUpright,
	dbScript_Misc_CreateGolfCart,
	dbScript_Logic_IfNoVehiclesInWorld,
	dbScript_Condition_ObjectEntersRegion,
	dbScript_Condition_CounterGreaterThan,
	dbScript_Set_HDR_And_Bloom_Params,
	dbScript_Setup_Reverb,
	dbScript_Play_OpenCommonShop,
	dbScript_Set_LightingUniqueId,
	dbScript_Logic_IfRandomFloatLessThanOrEqualsProb,
	dbScript_Set_DynamicWater2_AnimationState,
	dbScript_Misc_AwardNotes,
	dbScript_Set_ReferencePhysicsMotor,
	dbScript_Set_DynamicWater2_PhysicsSetup,
	dbScript_Setup_LoadDigits,
	dbScript_Set_Membrane_Params,
	dbScript_UI_Open_ComponentInfo,
	dbScript_Setup_ActivateEffectLocator,
	dbScript_Set_Blobs_Liquid_Params,
	dbScript_Set_Reference_Switch,
	dbScript_Set_Water_Colour,
	dbScript_Set_GlobalRPCValue,
	dbScript_Play_PreCueMovies,
	dbScript_Actors_ShowHide,
	dbScript_Misc_HideObjects,
	dbScript_Play_MovieLocalised,
	dbScript_Set_BackgroundObjectsStatic,
	dbScript_Play_MovieRandomSelection
};

class dbScript_Base {
public:
	int entrySize;
	int entryType;

	void readCommonScriptData(char* data) {
		int size = 0;
		int type = 0;

		memcpy(&size, data, 4);
		memcpy(&type, data + 4, 4);

		entrySize = flipEndian(size);
		entryType = flipEndian(type);
	}
};

class dbScript_DebugPrintf : public dbScript_Base {
public:
	char msg[0x100];

	void readScriptData(char* data) {
		memset(msg, 0, 0x100);

		strncpy(msg, data + 8, 0x100);
	}
};

dbScript_Base* CreateScriptData(dbScript_BanjoXEnum markerType) {
	switch (markerType) {
	case dbScript_Debug_Printf: { printf("DEBUG PRINTF\n"); return new dbScript_DebugPrintf(); } break;
	default: { printf("BASE\n"); return new dbScript_Base(); } break;
	}
}

class ScriptFile {
public:
	char* fileData;

	bool isReady = false;

	int numOfScriptEntries = 0;
	dbScript_Base** scriptEntries;

	void ReadScriptFile(char* data) {
		if (data == nullptr) return;
		fileData = data;

		bool hasInitialEntryStarted = false;
		int offs = 0;

		int index = 0;
		// settle the marker count.
		while (true) {
			int size = 0;
			int type = 0;

			memcpy(&size, fileData + offs, 4);
			memcpy(&type, fileData + offs + 4, 4);

			size = flipEndian(size);
			type = flipEndian(type);

			printf("Entry %d\t->\t%d\n", index, size);

			//char* markerData = (char*)malloc(size);

			numOfScriptEntries++;
			if (type == 0) {
				break;
			}

			offs += size;
			index++;
		}

		offs = 0;
		scriptEntries = new dbScript_Base*[numOfScriptEntries];

		// set up the markers.
		for (int i = 0; i < numOfScriptEntries; i++) {
			int size = 0;
			int type = 0;

			memcpy(&size, fileData + offs, 4);
			memcpy(&type, fileData + offs + 4, 4);

			size = flipEndian(size);
			type = flipEndian(type);

			char* markerData = (char*)malloc(size);
			memcpy(markerData, fileData + offs, size);
			scriptEntries[i] = CreateScriptData((dbScript_BanjoXEnum)type);

			scriptEntries[i]->readCommonScriptData(markerData);

			printf("Entry %05d\t->\t[%s]\n", i, dbScriptNames[scriptEntries[i]->entryType]);

			switch (scriptEntries[i]->entryType) {
			case dbScript_Debug_Printf: {
				((dbScript_DebugPrintf*)scriptEntries[i])->readScriptData(markerData);
			}
			break;
			}

			offs += size;

			free(markerData);
		}
	}
};