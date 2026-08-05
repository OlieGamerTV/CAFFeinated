#pragma once

//Vehicle Editior
void DisplayVehicleEditorBaseWindow();
static void openLoadVehicleFile();

//helpers
static void DisplayColorBlock(ImVec4* refCol);
static void DisplayColorBlock(unsigned int* refCol);
static void DisplayPartTable(unsigned int* refUUID, const char* name);

void SetupVehicleEditorWindow(bool canSave, char* fileName);
Vehicle* GetVehicleWindowVehiclePtr();
void FreeVehicleWindowVehicleMemory();
void AllocateVehicleWindowVehicleMemory(int numberOfParts);

struct ImGuiVehicleBlockParams {
	unsigned int idx = 0;
	ImVec4 color;

	int idToRemove = 0;

	ImVec4 colorSearch;
	ImVec4 colorToReplace;

	unsigned int partSearch = 0;
	unsigned int partToReplace = 0;

	int partsChanged = 0;

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