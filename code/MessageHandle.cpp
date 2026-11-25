#include "imgui_includes.h"
#ifdef _WIN32 
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <cstdio>

#ifdef __APPLE__
#include <AudioToolbox/AudioServices.h>
#endif

#include <string.h>
#include "MessageHandle.h"

ErrorPrompt errorPromptParams;

void FireMessageF(const char* message, ErrorType severity, ...) {
	int count = 0;
	char cset[] = "%%";
	const char* pch = strpbrk(message, cset);
	while (pch != NULL) {
		count++;
		pch = strpbrk(pch + 1, cset);
	}

	va_list ap;
	va_start(ap, count);

	char buffer[1024] = { 0 };
	vsnprintf(buffer, 1024, message, ap);

	FireMessage(buffer, severity);

	va_end(ap);
}

void FireMessage(const char* message, ErrorType severity) {
	ImGui::DebugLog("FIREMESSAGE - %s\n", message);

	errorPromptParams.promptEntries.emplace_back();

	if (strlen(message) <= 1024) {
		strcpy(errorPromptParams.promptEntries[errorPromptParams.promptsActive].message, message);
	}
	
	errorPromptParams.promptEntries[errorPromptParams.promptsActive].severity = severity;

	memset(errorPromptParams.promptEntries[errorPromptParams.promptsActive].title, 0, 256);

	if (severity == ErrorType_None) {
		CloseMessage(errorPromptParams.promptsActive);
		return;
	}

#ifdef __APPLE__
	if (severity == ErrorType_Info) {
		AudioServicesPlayAlertSound(kSystemSoundID_UserPreferredAlert);
	}
	if (severity == ErrorType_Warn || severity == ErrorType_Error || severity == ErrorType_Critical) {
		AudioServicesPlayAlertSound(kSystemSoundID_UserPreferredAlert);
		AudioServicesPlayAlertSound(kSystemSoundID_FlashScreen);
	}
#endif

#if _WIN32
	if (severity == ErrorType_Info) {
		MessageBeep(MB_ICONINFORMATION);
	}

	if (severity == ErrorType_Warn) {
		MessageBeep(MB_ICONEXCLAMATION);
	}

	if (severity == ErrorType_Error || severity == ErrorType_Critical) {
		MessageBeep(MB_ICONERROR);
	}
#endif

	if (severity == ErrorType_Info) {
		strcpy(errorPromptParams.promptEntries[errorPromptParams.promptsActive].title, "Informational Message");
	}

	if (severity == ErrorType_Warn) {
		strcpy(errorPromptParams.promptEntries[errorPromptParams.promptsActive].title, "Warning Message");
	}

	if (severity == ErrorType_Error) {
		strcpy(errorPromptParams.promptEntries[errorPromptParams.promptsActive].title, "Error Message");
	}

	if (severity == ErrorType_Critical) {
		strcpy(errorPromptParams.promptEntries[errorPromptParams.promptsActive].title, "Critical Message");
	}

	errorPromptParams.promptsActive++;
}

void ResetMessage(int idx) {
	memset(errorPromptParams.promptEntries[idx].title, 0, 256);
	memset(errorPromptParams.promptEntries[idx].message, 0, 1024);
	errorPromptParams.promptEntries[idx].severity = ErrorType_None;
}

void CloseMessage(int idx) {
	ResetMessage(idx);

	errorPromptParams.promptEntries.erase(errorPromptParams.promptEntries.begin() + idx);
	errorPromptParams.promptsActive--;
}

void DisplayMessageWindow() {

	//ImGui::DebugLog("DISPLAY - %s\n", errorPromptParams.errorMessage);

	for (int i = 0; i < errorPromptParams.promptsActive; i++) {
		bool isOpen = true;
		if (ImGui::Begin(errorPromptParams.promptEntries[i].title, &isOpen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) {
			ImGui::Text(errorPromptParams.promptEntries[i].message);
			ImGui::Spacing();
			if (ImGui::Button("Okay") || !isOpen) {
				CloseMessage(i);
				ImGui::CloseCurrentPopup();
			}
			ImGui::End();
		}
	}
}

bool IsMessagePromptShowing() { return errorPromptParams.promptsActive != 0; }
char* GetMessagePromptMessage(int idx) { return errorPromptParams.promptEntries[idx].message; }
ErrorType GetMessagePromptSeverity(int idx) { return errorPromptParams.promptEntries[idx].severity; }