#ifndef __LOADINGPROCESS_H__
#define __LOADINGPROCESS_H__

static const char* PROMPTLOADINGFILE_GENERIC = "Currently opening the file. Please wait.";


struct LoadingPrompt {
public:
	bool showLoadingPrompt = false;
	char loadingMessage[1024];

	int loadWheel = 0;
};

struct LoadingBarPrompt {
public:
	bool showLoadingBarPrompt = false;
	char loadingMessage[1024];
	int totalAmount = 0;
	int currentSaved = 0;

	int loadWheel = 0;
};

// Loading Prompt
bool IsLoadingPromptWidgetOpen();
void OpenLoadingPromptWidget();
void DisplayLoadingPromptWindow();

void SetupLoadingPromptWidget(const char* message);
void CloseLoadingPromptWidget();

// Loading Bar Prompt
void DrawSpinnerWheel();
void OpenLoadingBarPromptWidget();
bool IsLoadingBarPromptWidgetOpen();
void SetupLoadingBarPromptWidget(const char* message, int totalAmount);
void CloseLoadingBarPromptWidget();
void IncreaseCurrentSavedOnLoadingWidget();
void DisplayLoadingBarPromptWidget();

#endif