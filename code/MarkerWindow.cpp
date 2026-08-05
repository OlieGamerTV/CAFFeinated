#include <iostream>
#include "imgui_includes.h"
#include "CaffFileTypes.h"
#include "BaseMarker.h"
#include "CommonReader.h"
#include <nfd.h>

#ifndef MARKER_WINDOW
#include "MarkerWindow.h"
#endif

static MarkerWindowParams markerWindowParameters;


/// <summary>
/// Handles the functionality of the Vehicle Editor window.
/// </summary>
void DisplayMarkerEditorBaseWindow() {
	bool openNewPartPopup = false;
	bool generateHeaderPopup = false;
	bool openRemovePartPopup = false;
	bool openReplaceColorPopup = false;
	bool openReplacePartPopup = false;

	if (ImGui::Begin("Marker Editor", &markerWindowParameters.showMarkerEditor, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load From File")) {
					openLoadMarkerFile();
				}

				if (ImGui::MenuItem("Save", NULL, false, markerWindowParameters.isFileActive && !markerWindowParameters.cantSave)) {
				}
				if (ImGui::MenuItem("Save as New", NULL, false, markerWindowParameters.isFileActive)) {
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Clear", NULL, false, markerWindowParameters.isFileActive)) {
					if (markerWindowParameters.activeMarker.markerEntries != nullptr) {
						delete[] markerWindowParameters.activeMarker.markerEntries;
						markerWindowParameters.activeMarker.markerEntries = nullptr;
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (markerWindowParameters.isFileActive) {
			ImGui::Text("Num. of Entries: %d", markerWindowParameters.activeMarker.numOfMarkerEntries);

			ImGui::Separator();

			// Vehicle Parts List
			if (ImGui::BeginChild("Marker Entries")) {
				for (int i = 0; i < markerWindowParameters.activeMarker.numOfMarkerEntries; i++) {
					ImGui::PushID(i);
					char string[256];
					memset(string, 0, 256);

					sprintf(string, "Entry %d (%s)", i, dbMarkerNames[markerWindowParameters.activeMarker.markerEntries[i]->markerId]);

					if (ImGui::TreeNode(string)) {
						if (ImGui::TreeNode("Common")) {
							float positionalVal[3]{ 0,0,0 };
							positionalVal[0] += markerWindowParameters.activeMarker.markerEntries[i]->pos.x;
							positionalVal[1] += markerWindowParameters.activeMarker.markerEntries[i]->pos.y;
							positionalVal[2] += markerWindowParameters.activeMarker.markerEntries[i]->pos.z;

							ImGui::InputFloat3("Position", positionalVal);
							markerWindowParameters.activeMarker.markerEntries[i]->pos.x = positionalVal[0];
							markerWindowParameters.activeMarker.markerEntries[i]->pos.y = positionalVal[1];
							markerWindowParameters.activeMarker.markerEntries[i]->pos.z = positionalVal[2];

							if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
								ImGui::Text("The position of the entry.");
								ImGui::EndTooltip();
							}

							float rotationVal[3]{ 0,0,0 };
							// Rotation of Yaw - Pitch - Roll
							rotationVal[0] += markerWindowParameters.activeMarker.markerEntries[i]->pyr.p;
							rotationVal[1] += markerWindowParameters.activeMarker.markerEntries[i]->pyr.y;
							rotationVal[2] += markerWindowParameters.activeMarker.markerEntries[i]->pyr.r;

							ImGui::InputFloat3("Rotation", rotationVal);
							markerWindowParameters.activeMarker.markerEntries[i]->pyr.p = rotationVal[0];
							markerWindowParameters.activeMarker.markerEntries[i]->pyr.y = rotationVal[1];
							markerWindowParameters.activeMarker.markerEntries[i]->pyr.r = rotationVal[2];

							if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
								ImGui::Text("The rotation of the entry.");
								ImGui::EndTooltip();
							}

							ImGui::InputScalar("Challenge ID", ImGuiDataType_U32, &markerWindowParameters.activeMarker.markerEntries[i]->challengeAid, 0, 0, "%08X");
							if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
								ImGui::Text("The assigned challenge ID of the entry.");
								ImGui::EndTooltip();
							}

							ImGui::InputScalar("Flags", ImGuiDataType_U32, &markerWindowParameters.activeMarker.markerEntries[i]->flags, 0, 0, "%08X");
							if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
								ImGui::Text("The assigned flags of the entry.");
								ImGui::EndTooltip();
							}

							if (markerWindowParameters.activeMarker.markerEntries[i]->flags != 0) {

							}
							ImGui::TreePop();
						}
						if (IsSpecialMarkerEntry(markerWindowParameters.activeMarker.markerEntries[i]->markerId) && ImGui::TreeNode("Special")) {
							if (markerWindowParameters.activeMarker.markerEntries[i]->markerId == marker_ComponentCrate) {
								MarkerComponentCrate* compMark = static_cast<MarkerComponentCrate*>(markerWindowParameters.activeMarker.markerEntries[i]);

								ImGui::InputScalar("Params Aid", ImGuiDataType_U32, &compMark->objParamAid, 0, 0, "%08X");
								if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
									ImGui::Text("The aid of the \"objparams\" file that determines the crates appearance.");
									ImGui::EndTooltip();
								}

								ImGui::InputScalar("Block Set Aid", ImGuiDataType_U32, &compMark->blockSetAid, 0, 0, "%08X");
								if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
									ImGui::Text("The aid of the \"blockset\" file that determines the crate contents.");
									ImGui::EndTooltip();
								}

								ImGui::InputText("Collected Crate Flag", compMark->collectedCrate, 64);
								if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
									ImGui::Text("The flag that determines if the crate is collected.");
									ImGui::EndTooltip();
								}

								ImGui::InputText("Unlocked Crate Flag", compMark->unlockedCrate, 64);
								if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
									ImGui::Text("If this is assigned, the crate won't spawn until the flag is true.");
									ImGui::EndTooltip();
								}

								ImGui::InputScalar("Flags", ImGuiDataType_U32, &compMark->flag, 0, 0, "%08X");
								if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
									ImGui::Text("Flags for the crate. This determines the crate appearance.");
									ImGui::EndTooltip();
								}

								ImGui::InputScalar("Use Colour Palette", ImGuiDataType_U8, &compMark->useColorPalette);
								if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
									ImGui::Text("Sets whether the crate uses a different colour in the palette.");
									ImGui::EndTooltip();
								}

								ImGui::InputScalar("Colour Palette Index", ImGuiDataType_U8, &compMark->colorPaletteId);
								if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
									ImGui::Text("The current colour palette index.");
									ImGui::EndTooltip();
								}
							}

							if (markerWindowParameters.activeMarker.markerEntries[i]->markerId == marker_Portal) {
								MarkerPortal* portalMark = static_cast<MarkerPortal*>(markerWindowParameters.activeMarker.markerEntries[i]);

								ImGui::InputScalar("Params Aid", ImGuiDataType_U32, &portalMark->objParamAid, 0, 0, "%08X");
								if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
									ImGui::Text("The aid of the \"objparams\" file that determines the portals appearance.");
									ImGui::EndTooltip();
								}

								ImGui::InputScalar("Block Set Aid", ImGuiDataType_U32, &portalMark->scriptAid, 0, 0, "%08X");
								if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
									ImGui::Text("The aid of the \"script\" file that determines the level that will be loaded.");
									ImGui::EndTooltip();
								}

								ImGui::InputScalar("Cutscene Aid", ImGuiDataType_U32, &portalMark->cutsceneAid, 0, 0, "%08X");
								if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
									ImGui::Text("The aid of a \"cutscene\" file.");
									ImGui::EndTooltip();
								}

								ImGui::InputText("Game Flag", portalMark->gameFlag, 64);
								if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
									ImGui::Text("The flag that determines its activity.");
									ImGui::EndTooltip();
								}

								ImGui::InputScalar("Jiggy Requirement", ImGuiDataType_U32, &portalMark->jiggyRequirement);
								if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
									ImGui::Text("The amount of jiggies required to use the portal.");
									ImGui::EndTooltip();
								}

								ImGui::InputScalar("Flag List Aid", ImGuiDataType_U32, &portalMark->flagListAid, 0, 0, "%08X");
								if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
									ImGui::Text("The aid of a connected \"flaglist\" file.");
									ImGui::EndTooltip();
								}

								ImGui::InputText("Scene Indicator", portalMark->sceneIndicator, 64);
								if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
									ImGui::Text("The scene indicator.");
									ImGui::EndTooltip();
								}
							}
							ImGui::TreePop();
						}
						ImGui::TreePop();
					}
					ImGui::PopID();
				}
				ImGui::EndChild();
			}
		}

		ImGui::End();
	}
}

static void openLoadMarkerFile() {
	markerWindowParameters.vehicleFilePath = NULL;

	if (NFD_OpenDialog(&markerWindowParameters.vehicleFilePath, NULL, 0, NULL) == NFD_OKAY) {
		FILE* file = fopen(markerWindowParameters.vehicleFilePath, "rb");

		fseek(file, 0L, SEEK_END);
		int len = ftell(file) + 1;
		fseek(file, 0L, SEEK_SET);

		char* fileData = (char*)malloc(len);

		fread(fileData, sizeof(char), len, file);

		fclose(file);

		markerWindowParameters.activeMarker.ReadMarkerFile(fileData);

		SetupMarkerEditorWindow(true);

		markerWindowParameters.isFileActive = true;

		free(fileData);
	}
	else {
	}
}

MarkerWindowParams* GetMarkerEditorWindowParameters() {
	return &markerWindowParameters;
}

void SetupMarkerEditorWindow(bool canSave) {
	if (!canSave) {
		markerWindowParameters.cantSave = true;
	}

	if (canSave) {
		markerWindowParameters.cantSave = false;
	}

	markerWindowParameters.showMarkerEditor = true;
}