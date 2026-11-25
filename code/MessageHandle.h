#ifndef _MESSAGEHANDLE
#define _MESSAGEHANDLE
#include <vector>
#if _WIN32
#pragma once
#endif

static const char* EMESSAGEFORMAT_FILEREAD_EACCESS = "A permission denied error was hit while trying to read \"%s\".";
static const char* EMESSAGEFORMAT_FILEREAD_EINTR = "The signal was interrupted while trying to read \"%s\".";
static const char* EMESSAGEFORMAT_FILEREAD_EISDIR = "The given file \"%s\" is a directory.";

enum ErrorType : unsigned char {
	ErrorType_None,
	ErrorType_Info,
	ErrorType_Warn,
	ErrorType_Error,
	ErrorType_Critical
};

struct ErrorPromptEntry {
public:
	char title[256] = { 0 };
	char message[1024] = { 0 };
	ErrorType severity;
};

struct ErrorPrompt {
public:
	int promptsActive = 0;
	std::vector<ErrorPromptEntry> promptEntries;
};

void FireMessage(const char* message, ErrorType severity);
void FireMessageF(const char* message, ErrorType severity, ...);
void ResetMessage(int idx);
void CloseMessage(int idx);
void DisplayMessageWindow();
bool IsMessagePromptShowing();
char* GetMessagePromptMessage(int idx);
ErrorType GetMessagePromptSeverity(int idx);

#endif