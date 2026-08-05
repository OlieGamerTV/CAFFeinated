#include <iostream>
#include "imgui_includes.h"
#include "CaffFileTypes.h"
#include "BaseScript.h"
#include "CommonReader.h"
#include <nfd.h>

#ifndef SCRIPT_WINDOW
#include "ScriptWindow.h"
#endif

static ScriptWindowParams scriptWindowParameters;


/// <summary>
/// Handles the functionality of the Vehicle Editor window.
/// </summary>
void DisplayScriptEditorBaseWindow() {
	bool openNewPartPopup = false;
	bool generateHeaderPopup = false;
	bool openRemovePartPopup = false;
	bool openReplaceColorPopup = false;
	bool openReplacePartPopup = false;

	if (ImGui::Begin("Script Editor", &scriptWindowParameters.showScriptEditor, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load From File")) {
					openLoadScriptFile();
				}

				if (ImGui::MenuItem("Save", NULL, false, scriptWindowParameters.isFileActive && !scriptWindowParameters.cantSave)) {
				}
				if (ImGui::MenuItem("Save as New", NULL, false, scriptWindowParameters.isFileActive)) {
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Clear", NULL, false, scriptWindowParameters.isFileActive)) {
					if (scriptWindowParameters.activeScript.scriptEntries != nullptr) {
						delete[] scriptWindowParameters.activeScript.scriptEntries;
						scriptWindowParameters.activeScript.scriptEntries = nullptr;
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (scriptWindowParameters.isFileActive) {
			ImGui::Text("Num. of Entries: %d", scriptWindowParameters.activeScript.numOfScriptEntries);

			ImGui::Separator();

			float treeIndentation = 0;
			// Vehicle Parts List
			if (ImGui::BeginChild("Script Tree")) {
				for (int i = 0; i < scriptWindowParameters.activeScript.numOfScriptEntries; i++) {
					ImGui::PushID(i);
					char string[256];
					memset(string, 0, 256);

					sprintf(string, "%s", dbScriptNames[scriptWindowParameters.activeScript.scriptEntries[i]->entryType]);

					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + treeIndentation);
					ImGui::BulletText(string);
					ImGui::PopID();

					int indentID = HandleIndention(scriptWindowParameters.activeScript.scriptEntries[i]->entryType);
					if (indentID == 1) {
						ImGui::Indent();
					}
					if (indentID == -1) {
						ImGui::Unindent();
					}
				}
				ImGui::EndChild();
			}
		}

		ImGui::End();
	}
}

int HandleIndention(int type) {
	// Conditions
	if (type >= dbScript_Condition_TimeOut && type <= dbScript_Condition_NumberOfKnockdowns) return 1;
	if (type >= dbScript_Condition_ActorEntersRegion && type <= dbScript_Condition_PlayerLeavesRegion) return 1;
	if (type >= dbScript_Condition_SpecificGhouliesAlive && type <= dbScript_Condition_SpecificGhouliesKilled) return 1;
	if (type >= dbScript_Condition_ObjectRemoved && type <= dbScript_Condition_GhoulyExists) return 1;
	if (type >= dbScript_Condition_PlayerTalksToNPC && type <= dbScript_Condition_PlayerEntersVehicle) return 1;
	if (type == dbScript_Condition_IfGameFlag) return 1;
	if (type == dbScript_Condition_VehicleIsUpright) return 1;
	if (type >= dbScript_Condition_ObjectEntersRegion && type <= dbScript_Condition_CounterGreaterThan) return 1;

	// Logics
	if (type == dbScript_Logic_IfGameflag) return 1;
	if (type >= dbScript_Logic_IfChallengeResult && type <= dbScript_Logic_IfDialogResponse) return 1;
	if (type == dbScript_Logic_IfNumObjectsFound) return 1;
	if (type == dbScript_Logic_IfNoVehiclesInWorld) return 1;
	if (type == dbScript_Logic_IfRandomFloatLessThanOrEqualsProb) return 1;

	// Cancels
	if (type == dbScript_Condition_End) return -1;
	if (type == dbScript_Logic_Endif) return -1;

	return 0;
}

static void openLoadScriptFile() {
	scriptWindowParameters.vehicleFilePath = NULL;

	if (NFD_OpenDialog(&scriptWindowParameters.vehicleFilePath, NULL, 0, NULL) == NFD_OKAY) {
		FILE* file = fopen(scriptWindowParameters.vehicleFilePath, "rb");

		fseek(file, 0L, SEEK_END);
		int len = ftell(file) + 1;
		fseek(file, 0L, SEEK_SET);

		char* fileData = (char*)malloc(len);

		fread(fileData, sizeof(char), len, file);

		fclose(file);

		scriptWindowParameters.activeScript.ReadScriptFile(fileData);

		SetupScriptEditorWindow(true);

		scriptWindowParameters.isFileActive = true;

		free(fileData);
	}
	else {
	}
}

ScriptWindowParams* GetScriptEditorWindowParameters() {
	return &scriptWindowParameters;
}

void SetupScriptEditorWindow(bool canSave) {
	if (!canSave) {
		scriptWindowParameters.cantSave = true;
	}

	if (canSave) {
		scriptWindowParameters.cantSave = false;
	}

	scriptWindowParameters.showScriptEditor = true;
}