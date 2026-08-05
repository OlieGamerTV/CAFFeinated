#pragma once

enum ErrorType : unsigned char {
	ErrorType_None,
	ErrorType_Info,
	ErrorType_Warn,
	ErrorType_Error,
	ErrorType_Critical
};

struct ErrorPrompt {
public:
	bool showErrorPrompt = false;
	char errorMessage[1024] = { 0 };
	ErrorType errorSeverity;
};

void FireMessage(const char* message, ErrorType severity);
void ResetMessage();
void CloseMessage();
void DisplayMessageWindow();
bool IsMessagePromptShowing();
char* GetMessagePromptMessage();
ErrorType GetMessagePromptSeverity();