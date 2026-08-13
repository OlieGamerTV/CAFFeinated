#include "imgui_includes.h"
#include "CaffFileTypes.h"
#include "CommonReader.h"

#include <nfd.h>
#include <thread>

#include "LoctextWindow.h"

#include "LoadingProcess.h"

LoctextWindowParams loctextWindowParameters;

LoctextWindowParams* getLoctextWindowParams() { return &loctextWindowParameters; }

/// <summary>
/// Handles the functionality of the Loctext Editor window.
/// </summary>
void DisplayLoctextEditorBaseWindow() {
	if (ImGui::Begin("Loctext Editor", &loctextWindowParameters.showLoctextEditor, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
		if (ImGui::BeginMenuBar())
		{
			bool clearActive = loctextWindowParameters.activeLoctext != nullptr;

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load From File")) {
					openLoadLoctextFile();
				}

				if (ImGui::MenuItem("Save", NULL, false, clearActive)) {
					if (loctextWindowParameters.loctextFilePath != nullptr) {
						loctextWindowParameters.activeLoctext->WriteLoctext(loctextWindowParameters.loctextFilePath);
					}
				}
				if (ImGui::MenuItem("Save as New", NULL, false, clearActive)) {
					if (NFD_SaveDialogU8(&loctextWindowParameters.loctextFilePath, NULL, 0, "", "default.str") == NFD_OKAY) {
						loctextWindowParameters.activeLoctext->WriteLoctext(loctextWindowParameters.loctextFilePath);
					}
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Clear", NULL, false, clearActive)) {
					if (loctextWindowParameters.activeLoctext != nullptr) {
						delete loctextWindowParameters.activeLoctext;
						loctextWindowParameters.activeLoctext = nullptr;
					}
					ClearWindowParams();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Export", NULL, false, clearActive)) {
					nfdu8char_t* outPath;
					nfdu8filteritem_t filters[1] = { { "Setup Information File", "inf" } };
					char filename[134];
					memset(filename, 0, 134);

					if (loctextWindowParameters.hasAssignedFileName) {
						sprintf(filename, "%s.inf", loctextWindowParameters.originalFilename);
					}
					else {
						strcpy(filename, "default.inf");
					}
					
					printf("Exporting file \"%s\"\n", filename);

					if (NFD_SaveDialogU8(&outPath, filters, 1, NULL, filename) == NFD_OKAY) {
						loctextWindowParameters.activeLoctext->ExportToFileRaw(outPath);
						NFD_FreePathU8(outPath);
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (loctextWindowParameters.activeLoctext != nullptr && loctextWindowParameters.ready == true) {

			ImGui::SeparatorText("Loctext File");
			bool isBigEndian = loctextWindowParameters.activeLoctext->endianness;
			ImGui::Checkbox("Is Big Endian", &isBigEndian);
			loctextWindowParameters.activeLoctext->endianness = isBigEndian;
			ImGui::SeparatorText("Entries");

			// Vehicle Parts List
			if (ImGui::BeginChild("Entries")) {
				for (int i = 0; i < loctextWindowParameters.activeLoctext->labelTable.stringTable.header.totalCount; i++) {
					ImGui::PushID(i);

					unsigned short idx = 0;

					// Check if the position table is present. If it is, get the value from that. If not then fallback on the tag table entries.
					if (loctextWindowParameters.activeLoctext->labelTable.header.positionTableOffset != 0) {
						idx = loctextWindowParameters.activeLoctext->labelTable.posTable.entries[i];
					}
					else {
						idx = loctextWindowParameters.activeLoctext->labelTable.stringTable.infoEntries[i].hash;
					}

					// Check if there is any comment tied to this IDX.
					if (loctextWindowParameters.activeLoctext->IsIdxConnectedToComment(idx)) {
						// Slight cheat to get around in-text formatting.
						ImGui::Text("%s", loctextWindowParameters.activeLoctext->labelTable.commentTable.comments[loctextWindowParameters.activeLoctext->GetIdxOfConnectedComment(idx)].val);
					}

					//Buffer for the text.
					char conv[2048];

					if (loctextWindowParameters.activeLoctext->labelTable.header.tagTableOffset != 0 && loctextWindowParameters.activeLoctext->IsHashConnectedToTag(idx)) {
						if (ImGui::TreeNode(loctextWindowParameters.activeLoctext->labelTable.tagTable.tags[loctextWindowParameters.activeLoctext->GetIdxOfConnectedTag(idx)].val)) {
							//int total = WideCharToMultiByte(CP_UTF8, WC_COMPOSITECHECK, loctextWindowParameters.activeLoctext->labelTable.stringTable.strings[loctextWindowParameters.activeLoctext->GetIdxOfConnectedString(idx)].string, -1, conv, 2048, NULL, NULL);
							size_t total = wcstombs(conv, loctextWindowParameters.activeLoctext->labelTable.stringTable.strings[loctextWindowParameters.activeLoctext->GetIdxOfConnectedString(idx)].string, 2048);

							ImGui::InputText("Tag ", loctextWindowParameters.activeLoctext->labelTable.tagTable.tags[loctextWindowParameters.activeLoctext->GetIdxOfConnectedTag(idx)].val, 256);
							ImGui::InputTextMultiline("Text", conv, 2047); // Cut it down from 2048 to 2047 so we always have a null character.
							//int retTotal = MultiByteToWideChar(CP_UTF8, MB_COMPOSITE, conv, -1, loctextWindowParameters.activeLoctext->labelTable.stringTable.strings[loctextWindowParameters.activeLoctext->GetIdxOfConnectedString(idx)].string, 2048);
							int retTotal = mbstowcs(loctextWindowParameters.activeLoctext->labelTable.stringTable.strings[loctextWindowParameters.activeLoctext->GetIdxOfConnectedString(idx)].string, conv, 2048);

							ImGui::TreePop();
						}
					}
					else {
						//int total = WideCharToMultiByte(CP_UTF8, WC_COMPOSITECHECK, loctextWindowParameters.activeLoctext->labelTable.stringTable.strings[loctextWindowParameters.activeLoctext->GetIdxOfConnectedString(idx)].string, -1, conv, 2048, NULL, NULL);
						size_t total = wcstombs(conv, loctextWindowParameters.activeLoctext->labelTable.stringTable.strings[loctextWindowParameters.activeLoctext->GetIdxOfConnectedString(idx)].string, 2048);

						// Another cheat to get around in-text formatting.
						ImGui::Text("%s", conv);
					}

					ImGui::PopID();
				}

				// And run a final check to see if there is any comment tied to the max value.
				if (loctextWindowParameters.activeLoctext->IsIdxConnectedToComment(-1)) {
					ImGui::Text(loctextWindowParameters.activeLoctext->labelTable.commentTable.comments[loctextWindowParameters.activeLoctext->GetIdxOfConnectedComment(-1)].val);
				}

				ImGui::EndChild();
			}
		}
		ImGui::End();
	}
}

static void openLoadLoctextFile() {
	if (NFD_OpenDialog(&loctextWindowParameters.loctextFilePath, NULL, 0, NULL) == NFD_OKAY) {
		std::ifstream is(loctextWindowParameters.loctextFilePath, std::ifstream::binary);

		if (!is.is_open()) {
			printf("An error occured while trying to open the file for reading.\n");
			return;
		}

		is.seekg(0, is.end);
		int len = is.tellg();
		is.seekg(0, is.beg);

		char* fileData = (char*)malloc(len);

		is.read(fileData, len);

		is.close();

		OpenLoadingPromptWidget();

		printf("Loading loctext from file \"%s\"\nTotal Loctext Filesize = %d.\n", loctextWindowParameters.loctextFilePath, len);
		std::thread(&readExternalLoctextFile, fileData).detach();
	}
	else {
	}
}

void ClearWindowParams() {
	memset(loctextWindowParameters.originalFilename, 0, 128);
	loctextWindowParameters.hasAssignedFileName = false;
	loctextWindowParameters.ready = false;
}

void readExternalLoctextFile(char* data) {
	loctextWindowParameters.ready = false;

	if (loctextWindowParameters.activeLoctext != nullptr) {
		delete loctextWindowParameters.activeLoctext;
		loctextWindowParameters.activeLoctext = nullptr;
	}

	loctextWindowParameters.activeLoctext = new Loctext();
	loctextWindowParameters.activeLoctext->ReadLoctext(data);

	loctextWindowParameters.ready = true;

	free(data);

	CloseLoadingPromptWidget();
}

void AssignLoctextFilename(char* filename) {
	printf("%s\n", filename);
	memset(loctextWindowParameters.originalFilename, 0, 128);
	strcpy_s(loctextWindowParameters.originalFilename, 128, filename);
	loctextWindowParameters.hasAssignedFileName = true;
	printf("%s\n", loctextWindowParameters.originalFilename);
}