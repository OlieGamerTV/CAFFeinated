#pragma once

//Marker Editior
void DisplayMarkerEditorBaseWindow();
static void openLoadMarkerFile();

void SetupMarkerEditorWindow(bool canSave);
MarkerFile** GetMarkerWindowMarkerPtr();
void FreeMarkerWindowMarkerMemory();
void AllocateMarkerWindowMarkerMemory(int numberOfParts);

struct MarkerWindowParams {
	bool isFileActive = false;
	bool showMarkerEditor = false;
	nfdchar_t* vehicleFilePath = nullptr;
	MarkerFile activeMarker;
	//ImGuiVehicleBlockParams vehicleBlockAddParams;
	bool cantSave = false;
};

MarkerWindowParams* GetMarkerEditorWindowParameters();

bool IsSpecialMarkerEntry(dbMarkerEnum markerId) {
	if (markerSizes[markerId] != 0x34) return true;
	return false;
}