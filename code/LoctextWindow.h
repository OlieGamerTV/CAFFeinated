#ifndef __LOCTEXTWINDOW_H__
#define __LOCTEXTWINDOW_H__

//Loctext Editor
void DisplayLoctextEditorBaseWindow();
static void openLoadLoctextFile();

struct LoctextWindowParams {
	bool showLoctextEditor;
	bool ready = false;
	LoctextFile* activeLoctext = nullptr;
	nfdchar_t* loctextFilePath;

	char originalFilename[128] = { 0 };
	bool hasAssignedFileName = false;
};

LoctextWindowParams* getLoctextWindowParams();

void ClearWindowParams();
void readExternalLoctextFile(char* data);
void AssignLoctextFilename(char* filename);

#endif