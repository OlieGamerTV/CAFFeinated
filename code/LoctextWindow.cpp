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
				if (ImGui::MenuItem("Import", NULL, false, !clearActive)) {
					nfdu8char_t* outPath = new char[MAX_PATH];

					if (NFD_OpenDialogU8(&outPath, NULL, 0, NULL) == NFD_OKAY) {
						FILE* data = fopen(outPath, "r");

						if (loctextWindowParameters.activeLoctext != nullptr) {
							delete loctextWindowParameters.activeLoctext;
							loctextWindowParameters.activeLoctext = new Loctext();
						}

						if (loctextWindowParameters.activeLoctext == nullptr) {
							loctextWindowParameters.activeLoctext = new Loctext();
						}

						if (data != nullptr) {
							char str[8192] = { 0 };
							char tag[256] = { 0 };
							char text[2048] = { 0 };
							int strCount = -1;
							int commentCount = 0;

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

							loctextWindowParameters.activeLoctext->usesTags = true;
							loctextWindowParameters.activeLoctext->usesComments = commentCount != 0;
							loctextWindowParameters.activeLoctext->usesPos = true;

							loctextWindowParameters.activeLoctext->labelTable.stringTable.header.totalCount = strCount;
							loctextWindowParameters.activeLoctext->labelTable.tagTable.header.totalCount = strCount;
							loctextWindowParameters.activeLoctext->labelTable.commentTable.header.totalCount = commentCount;
							loctextWindowParameters.activeLoctext->labelTable.posTable.header.totalCount = strCount;

							loctextWindowParameters.activeLoctext->labelTable.stringTable.infoEntries = new LabelStrInfoEntry[strCount];
							loctextWindowParameters.activeLoctext->labelTable.stringTable.strings = new LabelStrEntry[strCount];
							loctextWindowParameters.activeLoctext->labelTable.tagTable.infoEntries = new TagInfo[strCount];
							loctextWindowParameters.activeLoctext->labelTable.tagTable.tags = new TagStr[strCount];
							loctextWindowParameters.activeLoctext->labelTable.commentTable.comments = new CommentStr[commentCount];
							loctextWindowParameters.activeLoctext->labelTable.commentTable.entries = new CommentEntry[commentCount];
							loctextWindowParameters.activeLoctext->labelTable.posTable.entries = new unsigned short[strCount];

							int tiedComment = -1;
							int currentStr = -1;
							bool isCurrentlyReadingComment = false;

							int totalStrOffs = 0;
							int totalTagOffs = 0;
							int totalCommentOffs = 0;

							while (!feof(data)) {
								memset(str, 0, 8192);
								memset(tag, 0, 256);
								memset(text, 0, 2048);

								fgets(str, 8192, data);
								if (str[0] == ';') {
									printf("%s\n", str);
									if (isCurrentlyReadingComment) {
										strncat(loctextWindowParameters.activeLoctext->labelTable.commentTable.comments[tiedComment].val, str, strlen(str));
									}
									if (!isCurrentlyReadingComment) {
										isCurrentlyReadingComment = true;
										tiedComment++;
										loctextWindowParameters.activeLoctext->labelTable.commentTable.entries[tiedComment].id = 0;
										memset(loctextWindowParameters.activeLoctext->labelTable.commentTable.comments[tiedComment].val, 0, 8192);
										strncpy(loctextWindowParameters.activeLoctext->labelTable.commentTable.comments[tiedComment].val, str, strlen(str));
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

									unsigned short hash = flipEndian(locHashElfHash16(tag));
									printf("%04X %s = %s\n", hash, tag, text);

									memset(loctextWindowParameters.activeLoctext->labelTable.stringTable.strings[currentStr].string, 0, sizeof(wchar_t) * 2048);
									memset(loctextWindowParameters.activeLoctext->labelTable.tagTable.tags[currentStr].val, 0, 256);

									mbstowcs(loctextWindowParameters.activeLoctext->labelTable.stringTable.strings[currentStr].string, text, 2048);
									strncpy(loctextWindowParameters.activeLoctext->labelTable.tagTable.tags[currentStr].val, tag, strlen(tag));

									loctextWindowParameters.activeLoctext->labelTable.stringTable.infoEntries[currentStr].hash = hash;

									loctextWindowParameters.activeLoctext->labelTable.tagTable.infoEntries[currentStr].id = hash;

									loctextWindowParameters.activeLoctext->labelTable.posTable.entries[currentStr] = hash;

									if (isCurrentlyReadingComment) {
										isCurrentlyReadingComment = false;
										loctextWindowParameters.activeLoctext->labelTable.commentTable.entries[tiedComment].id = hash;
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
					if (loctextWindowParameters.activeLoctext->usesPos) {
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

					if (loctextWindowParameters.activeLoctext->usesTags && loctextWindowParameters.activeLoctext->IsHashConnectedToTag(idx)) {
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