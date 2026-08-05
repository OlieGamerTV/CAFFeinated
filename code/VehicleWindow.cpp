#include <iostream>
#include "imgui_includes.h"
#include "CaffFileTypes.h"
#include "VehicleParts.h"
#include "CommonReader.h"
#include <nfd.h>

#ifndef VEHICLE_WINDOW
#include "VehicleWindow.h"
#endif

VehicleWindowParams markerWindowParameters;

/// <summary>
/// Handles the functionality of the Vehicle Editor window.
/// </summary>
void DisplayVehicleEditorBaseWindow() {
	bool openNewPartPopup = false;
	bool generateHeaderPopup = false;
	bool openRemovePartPopup = false;
	bool openReplaceColorPopup = false;
	bool openReplacePartPopup = false;

	if (ImGui::Begin("Vehicle Editor", &markerWindowParameters.showVehicleEditor, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
		if (ImGui::BeginMenuBar())
		{
			bool clearActive = markerWindowParameters.activeVehicle != nullptr;

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load From File")) {
					openLoadVehicleFile();
				}

				if (ImGui::MenuItem("Save", NULL, false, clearActive && !markerWindowParameters.cantSave)) {
					if (markerWindowParameters.activeVehicle != nullptr) {
						markerWindowParameters.activeVehicle->WriteToFile(markerWindowParameters.vehicleFilePath, markerWindowParameters.activeVehicle->isSaveVehicle);
					}
				}
				if (ImGui::MenuItem("Save as New", NULL, false, clearActive)) {

					if (NFD_SaveDialogU8(&markerWindowParameters.vehicleBlockAddParams.outputPath, NULL, 0, "", "000000FF") == NFD_OKAY) {
						if (markerWindowParameters.activeVehicle != nullptr) {
							markerWindowParameters.activeVehicle->WriteToFile(markerWindowParameters.vehicleBlockAddParams.outputPath, markerWindowParameters.activeVehicle->isSaveVehicle);
							generateHeaderPopup = true;
						}
					}
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Clear", NULL, false, clearActive)) {
					if (markerWindowParameters.activeVehicle != nullptr) {
						free(markerWindowParameters.activeVehicle);
						markerWindowParameters.activeVehicle = nullptr;
					}
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Append", NULL, false, clearActive)) {
					nfdu8char_t* appendVehPath;
					if (NFD_OpenDialogU8(&appendVehPath, NULL, 0, "") == NFD_OKAY) {

						Vehicle* appendVeh;

						FILE* file = fopen(appendVehPath, "rb");

						fseek(file, 0L, SEEK_END);
						int len = ftell(file) + 1;
						fseek(file, 0L, SEEK_SET);

						char* fileData = (char*)malloc(len);

						fread(fileData, sizeof(char), len, file);

						fclose(file);

						// Need to quickly grab the number of parts first in order to allocate memory
						unsigned short numOfParts = 0;
						unsigned int saveCheck = 0;

						memcpy(&saveCheck, fileData, sizeof(unsigned int));

						bool isSave = false;

						if (saveCheck == 0x48E19A3F) {
							isSave = true;
						}

						if (isSave) {
							memcpy(&numOfParts, fileData + 8, sizeof(unsigned short));
						}
						else {
							memcpy(&numOfParts, fileData, sizeof(unsigned short));
						}

						numOfParts = flipEndian(numOfParts);

						printf("Loading vehicle from file \"%s\" to append to the current vehicle.\nTotal Vehicle Filesize = %d.\n", appendVehPath, numOfParts);
						appendVeh = (Vehicle*)malloc(sizeof(Vehicle) + (sizeof(VehiclePart) * numOfParts));
						appendVeh->ReadVehicle(fileData, isSave);

						free(fileData);

						for (int i = 0; i < appendVeh->numOfParts; i++) {
							if (markerWindowParameters.activeVehicle->IsPositionTaken(appendVeh->parts[i].xPos, appendVeh->parts[i].yPos, appendVeh->parts[i].zPos)) {
								printf("Part %d on the vehicle we're appending from is at the same position as a part on the vehicle we're appending to. Skipping this part.\n", i);
								continue;
							}
							markerWindowParameters.activeVehicle->AddEntry(appendVeh->parts[i]);
						}

						free(appendVeh);
						NFD_FreePathU8(appendVehPath);
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::PushID("ColorSect");
				ImGui::MenuItem("Color", NULL, false, false);
				if (ImGui::MenuItem("Replace", NULL, false, clearActive)) {
					openReplaceColorPopup = true;
				}
				ImGui::PopID();

				ImGui::MenuItem("Parts", NULL, false, false);
				ImGui::PushID("PartSect");
				if (ImGui::MenuItem("Add New Part", NULL, false, clearActive)) {
					openNewPartPopup = true;
				}
				if (ImGui::MenuItem("Remove Part", NULL, false, clearActive)) {

					openRemovePartPopup = true;
				}
				if (ImGui::MenuItem("Replace", NULL, false, clearActive)) {
					openReplacePartPopup = true;
				}
				ImGui::PopID();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (markerWindowParameters.activeVehicle != nullptr) {
			ImGui::SeparatorText("Vehicle File");

			if (ImGui::BeginCombo("Filetype", markerWindowParameters.activeVehicle->isSaveVehicle == true ? "Save" : "Premade")) {

				if (ImGui::Selectable("Save", markerWindowParameters.activeVehicle->isSaveVehicle == true)) {
					markerWindowParameters.activeVehicle->isSaveVehicle = true;
				}
				if (ImGui::Selectable("Premade", markerWindowParameters.activeVehicle->isSaveVehicle == false)) {
					markerWindowParameters.activeVehicle->isSaveVehicle = false;
				}
				ImGui::EndCombo();
			}

			if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
				ImGui::Text("Controls how the tool saves the vehicle file.");
				ImGui::EndTooltip();
			}

			const char* nameTypeStr = "Undefined";

			if (markerWindowParameters.activeVehicle->nameType == 1) {
				nameTypeStr = "String";
			}
			if (markerWindowParameters.activeVehicle->nameType == 0) {
				nameTypeStr = "Tag";
			}

			if (ImGui::BeginCombo("Name Type", nameTypeStr)) {

				if (ImGui::Selectable("String", markerWindowParameters.activeVehicle->nameType == 1)) {
					markerWindowParameters.activeVehicle->nameType = 1;
					if (markerWindowParameters.activeVehicle->vehicleName != nullptr) {
						//vehicleWindowParameters.activeVehicle->vehicleUnicodeName = compileSToWS(vehicleWindowParameters.activeVehicle->vehicleName, 0x40);
						wmemcpy_s(markerWindowParameters.activeVehicle->vehicleUnicodeName, 0x20, compileSToWS(markerWindowParameters.activeVehicle->vehicleName, 0x40), 0x20);
					}
				}
				if (ImGui::Selectable("Tag", markerWindowParameters.activeVehicle->nameType == 0)) {
					markerWindowParameters.activeVehicle->nameType = 0;
					if (markerWindowParameters.activeVehicle->vehicleUnicodeName != nullptr) {
						memcpy_s(markerWindowParameters.activeVehicle->vehicleName, 0x40, compileWSToS(markerWindowParameters.activeVehicle->vehicleUnicodeName, 0x40), 0x40);
						//vehicleWindowParameters.activeVehicle->vehicleName = compileWSToS(vehicleWindowParameters.activeVehicle->vehicleUnicodeName, 0x40);
					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
				ImGui::Text("Controls how the game reads the \"Name\" entry.");
				ImGui::Text("\tString - Used as-is for the displayed name.");
				ImGui::Text("\tTag    - Should correspond to a tag in \"aid_loctext_banjox_vehicles\".");
				ImGui::EndTooltip();
			}

			if (markerWindowParameters.activeVehicle->nameType == 1) {
				char* name = compileWSToS(markerWindowParameters.activeVehicle->vehicleUnicodeName, 0x40);
				ImGui::InputText("Name", name, 0x20);
				wmemcpy_s(markerWindowParameters.activeVehicle->vehicleUnicodeName, 0x20, compileSToWS(name, 0x40), 0x20);
				//vehicleWindowParameters.activeVehicle->vehicleUnicodeName = compileSToWS(name, 0x40);
			}
			else if (markerWindowParameters.activeVehicle->nameType == 0) {
				ImGui::InputText("Name", markerWindowParameters.activeVehicle->vehicleName, 0x40);
			}

			if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
				ImGui::Text("The name of the vehicle.");
				ImGui::EndTooltip();
			}

			ImGui::Text("Num. of Parts: %d", markerWindowParameters.activeVehicle->numOfParts);

			// A Button Assignment
			if (ImGui::BeginCombo("A Button Assignment", getPartNameFromAid(markerWindowParameters.activeVehicle->aButtonAssignment))) {
				for (int k = 0; k < getCountOfAssignmentTable(); k++) {
					ImGui::PushID(k);
					if (vehicleAssignments[k].isComment) {
						ImGui::SeparatorText(vehicleAssignments[k].vehicleName);
					}
					else {
						if (ImGui::Selectable(vehicleAssignments[k].vehicleName, markerWindowParameters.activeVehicle->aButtonAssignment == vehicleAssignments[k].aid)) {
							markerWindowParameters.activeVehicle->aButtonAssignment = vehicleAssignments[k].aid;
						}
					}
					ImGui::PopID();
				}
				ImGui::EndCombo();
			}

			if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
				ImGui::Text("The component that is assigned to the A Button.");
				ImGui::EndTooltip();
			}

			// B Button Assignment
			if (ImGui::BeginCombo("B Button Assignment", getPartNameFromAid(markerWindowParameters.activeVehicle->bButtonAssignment))) {
				for (int k = 0; k < getCountOfAssignmentTable(); k++) {
					ImGui::PushID(k);
					if (vehicleAssignments[k].isComment) {
						ImGui::SeparatorText(vehicleAssignments[k].vehicleName);
					}
					else {
						if (ImGui::Selectable(vehicleAssignments[k].vehicleName, markerWindowParameters.activeVehicle->bButtonAssignment == vehicleAssignments[k].aid)) {
							markerWindowParameters.activeVehicle->bButtonAssignment = vehicleAssignments[k].aid;
						}
					}
					ImGui::PopID();
				}
				ImGui::EndCombo();
			}

			if (ImGui::IsItemHovered()) {
				if (ImGui::BeginTooltip()) {
					ImGui::Text("The component that is assigned to the B Button.");
					ImGui::EndTooltip();
				}
			}

			// X Button Assignment
			if (ImGui::BeginCombo("X Button Assignment", getPartNameFromAid(markerWindowParameters.activeVehicle->xButtonAssignment))) {
				for (int k = 0; k < getCountOfAssignmentTable(); k++) {
					ImGui::PushID(k);
					if (vehicleAssignments[k].isComment) {
						ImGui::SeparatorText(vehicleAssignments[k].vehicleName);
					}
					else {
						if (ImGui::Selectable(vehicleAssignments[k].vehicleName, markerWindowParameters.activeVehicle->xButtonAssignment == vehicleAssignments[k].aid)) {
							markerWindowParameters.activeVehicle->xButtonAssignment = vehicleAssignments[k].aid;
						}
					}
					ImGui::PopID();
				}
				ImGui::EndCombo();
			}

			if (ImGui::IsItemHovered()) {
				if (ImGui::BeginTooltip()) {
					ImGui::Text("The component that is assigned to the X Button.");
					ImGui::EndTooltip();
				}
			}

			ImGui::Separator();

			// Vehicle Parts List
			if (ImGui::BeginChild("Vehicle Parts")) {
				for (int i = 0; i < markerWindowParameters.activeVehicle->numOfParts; i++) {
					ImGui::PushID(i);
					char string[256];
					memset(string, 0, 256);

					sprintf(string, "Part %d (%s)", i, getPartNameFromAid(markerWindowParameters.activeVehicle->parts[i].partIdx));
					if (ImGui::TreeNode(string)) {
						int* valuesInt = compileChar3ToIntArray(markerWindowParameters.activeVehicle->parts[i].xPos, markerWindowParameters.activeVehicle->parts[i].yPos, markerWindowParameters.activeVehicle->parts[i].zPos);
						ImGui::InputInt3("Position", valuesInt);
						char* valuesChar = compileIntArrayToChar3(valuesInt);
						markerWindowParameters.activeVehicle->parts[i].xPos = valuesChar[0];
						markerWindowParameters.activeVehicle->parts[i].yPos = valuesChar[1];
						markerWindowParameters.activeVehicle->parts[i].zPos = valuesChar[2];

						if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
							ImGui::Text("The position of the part.");
							ImGui::EndTooltip();
						}

						float rotationVal[3]{ 0,0,0 };
						// Rotation of Yaw - Pitch - Roll
						rotationVal[0] += markerWindowParameters.activeVehicle->parts[i].yaw;
						rotationVal[1] += markerWindowParameters.activeVehicle->parts[i].pitch;
						rotationVal[2] += markerWindowParameters.activeVehicle->parts[i].roll;

						ImGui::InputFloat3("Rotation", rotationVal);
						markerWindowParameters.activeVehicle->parts[i].yaw = rotationVal[0];
						markerWindowParameters.activeVehicle->parts[i].pitch = rotationVal[1];
						markerWindowParameters.activeVehicle->parts[i].roll = rotationVal[2];

						if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
							ImGui::Text("The rotation of the part.");
							ImGui::EndTooltip();
						}

						ImGui::Checkbox("Is Challenge Bonus", (bool*)&markerWindowParameters.activeVehicle->parts[i].isChallengePart);

						ImGui::Checkbox("Is Part Painted", (bool*)&markerWindowParameters.activeVehicle->parts[i].isPainted);
						if (markerWindowParameters.activeVehicle->parts[i].isPainted) {
							DisplayColorBlock(&markerWindowParameters.activeVehicle->parts[i].color);
						}
						else {
							ImGui::Text("[Using Default Colour]");
						}

						DisplayPartTable(&markerWindowParameters.activeVehicle->parts[i].partIdx, "Part");
						ImGui::TreePop();
					}
					ImGui::PopID();
				}
				ImGui::EndChild();
			}
		}

		ImGui::End();
	}

	// Popup Starts
	if (generateHeaderPopup) {
		ImGui::OpenPopup("Create Header File?", ImGuiPopupFlags_NoOpenOverExistingPopup);
	}

	if (openReplacePartPopup) {
		ImGui::OpenPopup("Replace Part(s)", ImGuiPopupFlags_NoOpenOverExistingPopup);
		markerWindowParameters.vehicleBlockAddParams.partsChanged = 0;
		markerWindowParameters.vehicleBlockAddParams.partSearch = 0x1F4BB5E1;
		markerWindowParameters.vehicleBlockAddParams.partToReplace = 0x1F4BB5E1;
	}

	if (openReplaceColorPopup) {
		ImGui::OpenPopup("Replace Color", ImGuiPopupFlags_NoOpenOverExistingPopup);
		markerWindowParameters.vehicleBlockAddParams.partsChanged = 0;
		markerWindowParameters.vehicleBlockAddParams.colorSearch = ImGui::ColorConvertU32ToFloat4(0xFFFFFFFF);
		markerWindowParameters.vehicleBlockAddParams.colorToReplace = ImGui::ColorConvertU32ToFloat4(0xFFFFFFFF);
	}

	if (openNewPartPopup) {
		ImGui::OpenPopup("Add Part", ImGuiPopupFlags_NoOpenOverExistingPopup);
		markerWindowParameters.vehicleBlockAddParams.idx = 0x1F4BB5E1;
		markerWindowParameters.vehicleBlockAddParams.color = ImGui::ColorConvertU32ToFloat4(0xFFFFFFFF);
	}

	if (openRemovePartPopup) {
		ImGui::OpenPopup("Remove Part", ImGuiPopupFlags_NoOpenOverExistingPopup);
		markerWindowParameters.vehicleBlockAddParams.idToRemove = 0;
	}

	// Create Header File Popup
	if (ImGui::BeginPopupModal("Create Header File?", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) {
		ImGui::Text("Do you also want to generate a header file?");
		if (ImGui::Button("Yes")) {
			markerWindowParameters.activeVehicle->WriteHeaderFile(markerWindowParameters.vehicleBlockAddParams.outputPath);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("No")) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	// Replace Colour Popup
	if (ImGui::BeginPopupModal("Replace Color", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) {
		ImGui::PushID("find");
		ImGui::Text("Find what    ");
		ImGui::SameLine();

		DisplayColorBlock(&markerWindowParameters.vehicleBlockAddParams.colorSearch);
		ImGui::PopID();

		ImGui::PushID("replace");
		ImGui::Text("Replace with ");
		ImGui::SameLine();

		DisplayColorBlock(&markerWindowParameters.vehicleBlockAddParams.colorToReplace);

		ImGui::PopID();

		if (markerWindowParameters.vehicleBlockAddParams.partsChanged != 0) {
			ImGui::Text("%d parts were affected.", markerWindowParameters.vehicleBlockAddParams.partsChanged);
		}

		if (ImGui::Button("Replace Matched Colours")) {
			markerWindowParameters.vehicleBlockAddParams.partsChanged = 0;
			for (int i = 0; i < markerWindowParameters.activeVehicle->numOfParts; i++) {
				if (markerWindowParameters.activeVehicle->parts[i].color == ImGui::ColorConvertFloat4ToU32(markerWindowParameters.vehicleBlockAddParams.colorSearch)) {
					markerWindowParameters.activeVehicle->parts[i].color = ImGui::ColorConvertFloat4ToU32(markerWindowParameters.vehicleBlockAddParams.colorToReplace);
					markerWindowParameters.vehicleBlockAddParams.partsChanged++;
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Replace All Colours")) {
			markerWindowParameters.vehicleBlockAddParams.partsChanged = 0;
			for (int i = 0; i < markerWindowParameters.activeVehicle->numOfParts; i++) {
				markerWindowParameters.activeVehicle->parts[i].color = ImGui::ColorConvertFloat4ToU32(markerWindowParameters.vehicleBlockAddParams.colorToReplace);
				markerWindowParameters.vehicleBlockAddParams.partsChanged++;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}

	// Replace Parts Popup
	if (ImGui::BeginPopupModal("Replace Part(s)", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) {
		ImGui::PushID("find");
		ImGui::Text("Find what    ");
		ImGui::SameLine();

		DisplayPartTable(&markerWindowParameters.vehicleBlockAddParams.partSearch, "##xx");
		ImGui::PopID();

		ImGui::PushID("replace");
		ImGui::Text("Replace with ");
		ImGui::SameLine();

		DisplayPartTable(&markerWindowParameters.vehicleBlockAddParams.partToReplace, "##xx");

		ImGui::PopID();

		if (markerWindowParameters.vehicleBlockAddParams.partsChanged != 0) {
			ImGui::Text("%d parts were affected.", markerWindowParameters.vehicleBlockAddParams.partsChanged);
		}

		if (ImGui::Button("Replace Matched Parts")) {
			markerWindowParameters.vehicleBlockAddParams.partsChanged = 0;
			for (int i = 0; i < markerWindowParameters.activeVehicle->numOfParts; i++) {
				if (markerWindowParameters.activeVehicle->parts[i].partIdx == markerWindowParameters.vehicleBlockAddParams.partSearch) {
					markerWindowParameters.activeVehicle->parts[i].partIdx = markerWindowParameters.vehicleBlockAddParams.partToReplace;
					markerWindowParameters.vehicleBlockAddParams.partsChanged++;
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}

	// Add Part Popup
	if (ImGui::BeginPopupModal("Add Part")) {
		DisplayColorBlock(&markerWindowParameters.vehicleBlockAddParams.color);

		DisplayPartTable(&markerWindowParameters.vehicleBlockAddParams.idx, "Part To Add");

		if (ImGui::Button("Add Part")) {
			VehiclePart newPart;
			newPart.partIdx = markerWindowParameters.vehicleBlockAddParams.idx;
			newPart.color = ImGui::ColorConvertFloat4ToU32(markerWindowParameters.vehicleBlockAddParams.color);

			if (markerWindowParameters.activeVehicle != nullptr) {
				markerWindowParameters.activeVehicle->AddEntry(newPart);
			}
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}

	// Remove Part Popup
	if (ImGui::BeginPopupModal("Remove Part")) {
		Vehicle* activeVehicleToRemoveFrom = nullptr;

		if (markerWindowParameters.activeVehicle != nullptr) {
			activeVehicleToRemoveFrom = markerWindowParameters.activeVehicle;
		}

		char string[256];
		memset(string, 0, 256);

		sprintf(string, "Part %d (%s)", markerWindowParameters.vehicleBlockAddParams.idToRemove, getPartNameFromAid(activeVehicleToRemoveFrom->parts[markerWindowParameters.vehicleBlockAddParams.idToRemove].partIdx));

		if (ImGui::BeginCombo("Part to Remove", string)) {
			for (int i = 0; i < activeVehicleToRemoveFrom->numOfParts; i++) {
				ImGui::PushID(i);
				memset(string, 0, 256);

				sprintf(string, "Part %d (%s)", i, getPartNameFromAid(activeVehicleToRemoveFrom->parts[i].partIdx));
				if (ImGui::Selectable(string, markerWindowParameters.vehicleBlockAddParams.idToRemove == i)) {
					markerWindowParameters.vehicleBlockAddParams.idToRemove = i;
				}
				ImGui::PopID();
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Remove Selected Part")) {
			activeVehicleToRemoveFrom->RemoveEntry(markerWindowParameters.vehicleBlockAddParams.idToRemove);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}

/// <summary>
/// Handles drawing an ImGui block that allows picking from either default colours or a custom colour.
/// </summary>
/// <param name="refCol"></param>
static void DisplayColorBlock(ImVec4* refCol) {
	unsigned int inputCol = ImGui::ColorConvertFloat4ToU32(*refCol);

	// Base-Game Colours
	if (GetColorMatch(inputCol) != 0) {
		ImGui::Text("[");
		ImGui::SameLine();
	}
	ImGui::ColorButton("Base Colour", ImGui::ColorConvertU32ToFloat4(COL_ARR[GetColIdx(inputCol)]), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	ImGui::SameLine();
	if (ImGui::BeginCombo(COLNAME_ARR[GetColIdx(inputCol)], COLNAME_ARR[GetColIdx(inputCol)], ImGuiComboFlags_NoPreview)) {
		for (int k = 0; k < getCountOfColorTable(); k++) {
			if (ImGui::Selectable(COLNAME_ARR[k], inputCol == COL_ARR[k])) {
				*refCol = ImGui::ColorConvertU32ToFloat4(COL_ARR[k]);
			}
		}
		ImGui::EndCombo();
	}
	ImGui::SameLine();
	if (GetColorMatch(inputCol) != 0) {
		ImGui::Text("]");
		ImGui::SameLine();
	}

	// or
	ImGui::Text("or");
	ImGui::SameLine();

	// Custom Colours
	if (GetColorMatch(inputCol) == 0) {
		ImGui::Text("[");
		ImGui::SameLine();
	}
	ImGui::ColorEdit3("Custom Colour", (float*)refCol, ImGuiColorEditFlags_NoInputs);
	if (GetColorMatch(inputCol) == 0) {
		ImGui::SameLine();
		ImGui::Text("]");
	}
}

/// <summary>
/// Handles drawing an ImGui block that allows picking from either default colours or a custom colour.
/// </summary>
/// <param name="refCol"></param>
static void DisplayColorBlock(unsigned int* refCol) {
	ImVec4 inputCol = ImGui::ColorConvertU32ToFloat4(*refCol);

	DisplayColorBlock(&inputCol);

	*refCol = ImGui::ColorConvertFloat4ToU32(inputCol);
}

// Handles drawing an ImGui block that allows picking from a list of available parts.
static void DisplayPartTable(unsigned int* refUUID, const char* name) {
	if (ImGui::BeginCombo(name, getPartNameFromAid(*refUUID))) {
		for (int k = 0; k < getCountOfPartsTable(); k++) {
			if (vehicleParts[k].isComment) {
				ImGui::SeparatorText(vehicleParts[k].vehicleName);
			}
			else {
				if (ImGui::Selectable(vehicleParts[k].vehicleName, *refUUID == vehicleParts[k].aid)) {
					*refUUID = vehicleParts[k].aid;
				}
			}
		}
		ImGui::EndCombo();
	}
}

static void openLoadVehicleFile() {
	markerWindowParameters.vehicleFilePath = NULL;

	if (NFD_OpenDialog(&markerWindowParameters.vehicleFilePath, NULL, 0, NULL) == NFD_OKAY) {
		FILE* file = fopen(markerWindowParameters.vehicleFilePath, "rb");

		fseek(file, 0L, SEEK_END);
		int len = ftell(file) + 1;
		fseek(file, 0L, SEEK_SET);

		char* fileData = (char*)malloc(len);

		fread(fileData, sizeof(char), len, file);

		fclose(file);

		// Need to quickly grab the number of parts first in order to allocate memory
		unsigned short numOfParts = 0;
		unsigned int saveCheck = 0;

		memcpy(&saveCheck, fileData, sizeof(unsigned int));

		bool isSave = false;

		if (saveCheck == 0x48E19A3F) {
			isSave = true;
		}

		if (isSave) {
			memcpy(&numOfParts, fileData + 8, sizeof(unsigned short));
		}
		else {
			memcpy(&numOfParts, fileData, sizeof(unsigned short));
		}

		numOfParts = flipEndian(numOfParts);

		printf("Loading vehicle from file \"%s\"\nTotal Vehicle Filesize = %d.\n", markerWindowParameters.vehicleFilePath, numOfParts);
		AllocateVehicleWindowVehicleMemory(numOfParts);
		GetVehicleWindowVehiclePtr()->ReadVehicle(fileData, isSave);

		SetupVehicleEditorWindow(true, NULL);

		free(fileData);
	}
	else {
	}
}

VehicleWindowParams* GetVehicleEditorWindowParameters() {
	return &markerWindowParameters;
}

void SetupVehicleEditorWindow(bool canSave, char* fileName) {
	if (!canSave) {
		markerWindowParameters.cantSave = true;
		strcpy(markerWindowParameters.vehicleFileName, fileName);
	}

	if (canSave) {
		markerWindowParameters.cantSave = false;
		memset(markerWindowParameters.vehicleFileName, 0, 128);
	}

	markerWindowParameters.showVehicleEditor = true;
}

Vehicle* GetVehicleWindowVehiclePtr() {
	return markerWindowParameters
		.activeVehicle;
}

void FreeVehicleWindowVehicleMemory() {
	if (markerWindowParameters.activeVehicle != nullptr) {
		free(markerWindowParameters.activeVehicle);
		markerWindowParameters.activeVehicle = nullptr;
	}
}

void AllocateVehicleWindowVehicleMemory(int numberOfParts) {
	markerWindowParameters.activeVehicle = (Vehicle*)malloc(0x7C + (0x24 * numberOfParts));
}