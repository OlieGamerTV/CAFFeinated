#ifndef __LOCTEXTWINDOW_H__
#define __LOCTEXTWINDOW_H__

//Loctext Editor
void DisplayLoctextEditorBaseWindow();
static void openLoadLoctextFile();

struct LoctextWindowParams {
	bool showLoctextEditor;
	bool ready = false;
	Loctext* activeLoctext = nullptr;
	nfdchar_t* loctextFilePath;

	char originalFilename[128] = { 0 };
	bool hasAssignedFileName = false;
};

LoctextWindowParams loctextWindowParameters;

void ClearWindowParams() {
	memset(loctextWindowParameters.originalFilename, 0, 128);
	loctextWindowParameters.hasAssignedFileName = false;
	loctextWindowParameters.ready = false;
}

void readExternalLoctextFile(char* data);

void AssignLoctextFilename(char* filename) {
	printf("%s\n", filename);
	memset(loctextWindowParameters.originalFilename, 0, 128);
	strcpy_s(loctextWindowParameters.originalFilename, 128, filename);
	loctextWindowParameters.hasAssignedFileName = true;
}

#endif