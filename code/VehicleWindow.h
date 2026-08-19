#pragma once

//Vehicle Editior
void DisplayVehicleEditorBaseWindow();
static void openLoadVehicleFile();

//helpers
static void DisplayColorBlock(ImVec4* refCol);
static void DisplayColorBlock(uint32_t* refCol);
static void DisplayPartTable(uint32_t* refUUID, const char* name);

void SetupVehicleEditorWindow(bool canSave, char* fileName);
Vehicle* GetVehicleWindowVehiclePtr();
void FreeVehicleWindowVehicleMemory();
void AllocateVehicleWindowVehicleMemory(int32_t numberOfParts);

struct ImGuiVehicleBlockParams {
	uint32_t idx = 0;
	ImVec4 color;

	int32_t idToRemove = 0;

	ImVec4 colorSearch;
	ImVec4 colorToReplace;

	uint32_t partSearch = 0;
	uint32_t partToReplace = 0;

	int32_t partsChanged = 0;

	char* outputPath = nullptr;
};

struct VehicleWindowParams {
	bool showVehicleEditor = false;
	char vehicleFileName[128] = { 0 };
	nfdchar_t* vehicleFilePath = nullptr;
    Vehicle* activeVehicle = nullptr;
	ImGuiVehicleBlockParams vehicleBlockAddParams;
	bool cantSave = false;
};

VehicleWindowParams* GetVehicleEditorWindowParameters();