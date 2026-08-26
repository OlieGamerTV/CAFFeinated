#include "imgui_includes.h"
#include "CaffFileTypes.h"
#include "CommonReader.h"
#include "AssetCommons.h"

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
						if (loctextWindowParameters.activeLoctext->currentlyLoadedLoctext == Loc1) {
							loctextWindowParameters.activeLoctext->loc1File.WriteLoctext(loctextWindowParameters.loctextFilePath);
						}
					}
				}
				if (ImGui::MenuItem("Save as New", NULL, false, clearActive)) {
					if (NFD_SaveDialogU8(&loctextWindowParameters.loctextFilePath, NULL, 0, "", "default.str") == NFD_OKAY) {
						if (loctextWindowParameters.activeLoctext->currentlyLoadedLoctext == Loc1) {
							loctextWindowParameters.activeLoctext->loc1File.WriteLoctext(loctextWindowParameters.loctextFilePath);
						}
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
				if (ImGui::MenuItem("Import", NULL, false, !clearActive)) {
					nfdu8char_t* outPath = new char[MAX_PATH];

					if (NFD_OpenDialogU8(&outPath, NULL, 0, NULL) == NFD_OKAY) {
						FILE* data = fopen(outPath, "r");

						if (loctextWindowParameters.activeLoctext != nullptr) {
							delete loctextWindowParameters.activeLoctext;
							loctextWindowParameters.activeLoctext = new LoctextFile();
						}

						if (loctextWindowParameters.activeLoctext == nullptr) {
							loctextWindowParameters.activeLoctext = new LoctextFile();
						}

						if (data != nullptr) {
							char str[8192] = { 0 };
							char tag[256] = { 0 };
							char text[2048] = { 0 };
							int32_t strCount = -1;
							int32_t commentCount = 0;

							fseek(data, 0, SEEK_SET);

							while (!feof(data)) {
								fgets(str, 8192, data);
								if (str[0] != '\0' && str[0] != ';') {
									strCount++;
								}
								if (str[0] != '\0' && str[0] == ';') {
									commentCount++;
								}
							}

							fseek(data, 0, SEEK_SET);

							loctextWindowParameters.activeLoctext->loc1File.usesTags = true;
							loctextWindowParameters.activeLoctext->loc1File.usesComments = commentCount != 0;
							loctextWindowParameters.activeLoctext->loc1File.usesPos = true;

							loctextWindowParameters.activeLoctext->loc1File.labelTable.stringTable.header.totalCount = strCount;
							loctextWindowParameters.activeLoctext->loc1File.labelTable.tagTable.header.totalCount = strCount;
							loctextWindowParameters.activeLoctext->loc1File.labelTable.commentTable.header.totalCount = commentCount;
							loctextWindowParameters.activeLoctext->loc1File.labelTable.posTable.header.totalCount = strCount;

							loctextWindowParameters.activeLoctext->loc1File.labelTable.stringTable.infoEntries = new LabelStrInfoEntry[strCount];
							loctextWindowParameters.activeLoctext->loc1File.labelTable.stringTable.strings = new LabelStrEntry[strCount];
							loctextWindowParameters.activeLoctext->loc1File.labelTable.tagTable.infoEntries = new TagInfo[strCount];
							loctextWindowParameters.activeLoctext->loc1File.labelTable.tagTable.tags = new TagStr[strCount];
							loctextWindowParameters.activeLoctext->loc1File.labelTable.commentTable.comments = new CommentStr[commentCount];
							loctextWindowParameters.activeLoctext->loc1File.labelTable.commentTable.entries = new CommentEntry[commentCount];
							loctextWindowParameters.activeLoctext->loc1File.labelTable.posTable.entries = new uint16_t[strCount];

							int32_t tiedComment = -1;
							int32_t currentStr = -1;
							bool isCurrentlyReadingComment = false;

							int32_t totalStrOffs = 0;
							int32_t totalTagOffs = 0;
							int32_t totalCommentOffs = 0;

							while (!feof(data)) {
								memset(str, 0, 8192);
								memset(tag, 0, 256);
								memset(text, 0, 2048);

								fgets(str, 8192, data);
								if (str[0] == ';') {
									printf("%s\n", str);
									if (isCurrentlyReadingComment) {
										strncat(loctextWindowParameters.activeLoctext->loc1File.labelTable.commentTable.comments[tiedComment].val, str, strlen(str));
									}
									if (!isCurrentlyReadingComment) {
										isCurrentlyReadingComment = true;
										tiedComment++;
										loctextWindowParameters.activeLoctext->loc1File.labelTable.commentTable.entries[tiedComment].id = 0;
										memset(loctextWindowParameters.activeLoctext->loc1File.labelTable.commentTable.comments[tiedComment].val, 0, 8192);
										strncpy(loctextWindowParameters.activeLoctext->loc1File.labelTable.commentTable.comments[tiedComment].val, str, strlen(str));
									}
								}

								if (str[0] != '\0' && str[0] != ';') {
									char* tagEnd = strchr(str, '\t');
									if (tagEnd != nullptr) {
										strncpy(tag, str, tagEnd - str);
									}
									char* strStart = strchr(str, '\"');
									char* strEnd = strrchr(str, '\"');
									if (strStart != nullptr && strEnd != nullptr) {
										strncpy(text, strStart + 1, strEnd - strStart - 1);
									}

									currentStr++;

									uint16_t hash = flipEndian(locHashElfHash16(tag));
									printf("%04X %s = %s\n", hash, tag, text);

									memset(loctextWindowParameters.activeLoctext->loc1File.labelTable.stringTable.strings[currentStr].string, 0, sizeof(wchar_t) * 2048);
									memset(loctextWindowParameters.activeLoctext->loc1File.labelTable.tagTable.tags[currentStr].val, 0, 256);

									mbstowcs(loctextWindowParameters.activeLoctext->loc1File.labelTable.stringTable.strings[currentStr].string, text, 2048);
									strncpy(loctextWindowParameters.activeLoctext->loc1File.labelTable.tagTable.tags[currentStr].val, tag, strlen(tag));

									loctextWindowParameters.activeLoctext->loc1File.labelTable.stringTable.infoEntries[currentStr].hash = hash;

									loctextWindowParameters.activeLoctext->loc1File.labelTable.tagTable.infoEntries[currentStr].id = hash;

									loctextWindowParameters.activeLoctext->loc1File.labelTable.posTable.entries[currentStr] = hash;

									if (isCurrentlyReadingComment) {
										isCurrentlyReadingComment = false;
										loctextWindowParameters.activeLoctext->loc1File.labelTable.commentTable.entries[tiedComment].id = hash;
									}
								}
							}

							printf("Num of Text Entries %d\n", strCount + 1);
							printf("Num of Comment Entries %d\n", commentCount);
							loctextWindowParameters.ready = true;
						}
						fclose(data);

						NFD_FreePathU8(outPath);
					}
				}
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

			bool isBigEndian = loctextWindowParameters.activeLoctext->GetEndianness();
			ImGui::Checkbox("Is Big Endian", &isBigEndian);
			loctextWindowParameters.activeLoctext->SetEndianness(isBigEndian);
			bool usingTags = loctextWindowParameters.activeLoctext->GetIsUsingTags();
			bool usingPos = loctextWindowParameters.activeLoctext->GetIsUsingPositions();
			ImGui::Checkbox("Has Tags", &usingTags);
			ImGui::Checkbox("Keep Positions", &usingPos);
			loctextWindowParameters.activeLoctext->SetIsUsingTags(usingTags);
			loctextWindowParameters.activeLoctext->SetIsUsingPositions(usingPos);
			ImGui::SeparatorText("Entries");

			// Vehicle Parts List
			if (ImGui::BeginChild("Entries")) {
				for (int32_t i = 0; i < loctextWindowParameters.activeLoctext->GetStringCount(); i++) {
					ImGui::PushID(i);

					uint16_t idx = loctextWindowParameters.activeLoctext->GetPosPtr(i);

					// Check if there is any comment tied to this IDX.
					if (loctextWindowParameters.activeLoctext->IsIdxConnectedToComment(idx)) {
						// Slight cheat to get around in-text formatting.
						ImGui::TextDisabled("%s", loctextWindowParameters.activeLoctext->GetCommentPtr(loctextWindowParameters.activeLoctext->GetIdxOfConnectedComment(idx)));
					}

					//Buffer for the text.
					char conv[2048];

					if (usingTags && loctextWindowParameters.activeLoctext->IsHashConnectedToTag(idx)) {
						if (ImGui::TreeNode(loctextWindowParameters.activeLoctext->GetTagPtr(loctextWindowParameters.activeLoctext->GetIdxOfConnectedTag(idx)))) {
							//int32_t total = WideCharToMultiByte(CP_UTF8, WC_COMPOSITECHECK, loctextWindowParameters.activeLoctext->labelTable.stringTable.strings[loctextWindowParameters.activeLoctext->GetIdxOfConnectedString(idx)].string, -1, conv, 2048, NULL, NULL);
							size_t total = wcstombs(conv, loctextWindowParameters.activeLoctext->GetStringPtr(loctextWindowParameters.activeLoctext->GetIdxOfConnectedString(idx)), 2048);

							ImGui::InputText("Tag ", loctextWindowParameters.activeLoctext->GetTagPtr(loctextWindowParameters.activeLoctext->GetIdxOfConnectedTag(idx)), 256);
							ImGui::InputTextMultiline("Text", conv, 2047); // Cut it down from 2048 to 2047 so we always have a null character.
							//int32_t retTotal = MultiByteToWideChar(CP_UTF8, MB_COMPOSITE, conv, -1, loctextWindowParameters.activeLoctext->labelTable.stringTable.strings[loctextWindowParameters.activeLoctext->GetIdxOfConnectedString(idx)].string, 2048);
							int32_t retTotal = mbstowcs(loctextWindowParameters.activeLoctext->GetStringPtr(loctextWindowParameters.activeLoctext->GetIdxOfConnectedString(idx)), conv, 2048);

							ImGui::TreePop();
						}
					}
					else {
						//int32_t total = WideCharToMultiByte(CP_UTF8, WC_COMPOSITECHECK, loctextWindowParameters.activeLoctext->labelTable.stringTable.strings[loctextWindowParameters.activeLoctext->GetIdxOfConnectedString(idx)].string, -1, conv, 2048, NULL, NULL);
						size_t total = wcstombs(conv, loctextWindowParameters.activeLoctext->GetStringPtr(loctextWindowParameters.activeLoctext->GetIdxOfConnectedString(idx)), 2048);

						// Another cheat to get around in-text formatting.
						ImGui::Text("%s", conv);
					}

					ImGui::PopID();
				}

				// And run a final check to see if there is any comment tied to the max value.
				if (loctextWindowParameters.activeLoctext->IsIdxConnectedToComment(-1)) {
					ImGui::TextDisabled("%s", loctextWindowParameters.activeLoctext->GetCommentPtr(loctextWindowParameters.activeLoctext->GetIdxOfConnectedComment(-1)));
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
		int32_t len = is.tellg();
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

	loctextWindowParameters.activeLoctext = new LoctextFile();
	loctextWindowParameters.activeLoctext->ParseLoctextData(data);

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