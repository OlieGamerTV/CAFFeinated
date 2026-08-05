#pragma once

//Marker Editior
void DisplayScriptEditorBaseWindow();
static void openLoadScriptFile();

void SetupScriptEditorWindow(bool canSave);
ScriptFile** GetScriptWindowMarkerPtr();
void FreeScriptWindowMarkerMemory();
void AllocateMarkerWindowMarkerMemory(int numberOfParts);

struct ScriptWindowParams {
	bool isFileActive = false;
	bool showScriptEditor = false;
	nfdchar_t* vehicleFilePath = nullptr;
	ScriptFile activeScript;
	//ImGuiVehicleBlockParams vehicleBlockAddParams;
	bool cantSave = false;
};

int HandleIndention(int type);

ScriptWindowParams* GetScriptEditorWindowParameters();

bool IsSpecialScriptEntry(dbScript_BanjoXEnum markerId) {
	//if (markerSizes[markerId] != 0x8) return true;
	return false;
}