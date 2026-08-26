#include "imgui_includes.h"
#ifdef _WIN32 
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef __APPLE__
#include <AudioToolbox/AudioServices.h>
#endif

#include <string.h>

#ifndef MSG_WINDOW
#include "MessageHandle.h"
#endif

char title[256];
ErrorPrompt errorPromptParams;

void FireMessage(const char* message, ErrorType severity) {

	ImGui::DebugLog("FIREMESSAGE - %s\n", message);

	if (strlen(message) <= 1024) {
		strcpy(errorPromptParams.errorMessage, message);
	}
	
	errorPromptParams.errorSeverity = severity;

	memset(title, 0, 256);

	if (severity == ErrorType_None) {
		CloseMessage();
		return;
	}

#ifdef __APPLE__
	AudioServicesPlayAlertSound(kSystemSoundID_UserPreferredAlert);
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
		strcpy(title, "Informational Message");
	}

	if (severity == ErrorType_Warn) {
		strcpy(title, "Warning Message");
	}

	if (severity == ErrorType_Error) {
		strcpy(title, "Error Message");
	}

	if (severity == ErrorType_Critical) {
		strcpy(title, "Critical Message");
	}

	errorPromptParams.showErrorPrompt = true;
}

void ResetMessage() {
	memset(errorPromptParams.errorMessage, 0, 1024);
	errorPromptParams.errorSeverity = ErrorType_None;

	errorPromptParams.showErrorPrompt = true;
}

void CloseMessage() {
	ResetMessage();

	errorPromptParams.showErrorPrompt = false;
}

void DisplayMessageWindow() {

	//ImGui::DebugLog("DISPLAY - %s\n", errorPromptParams.errorMessage);

	if (ImGui::Begin(title, NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) {
		ImGui::Text(errorPromptParams.errorMessage);
		ImGui::Spacing();
		if (ImGui::Button("Okay")) {
			CloseMessage();
			ImGui::CloseCurrentPopup();
		}
		ImGui::End();
	}
}

bool IsMessagePromptShowing() { return errorPromptParams.showErrorPrompt; }
char* GetMessagePromptMessage() { return errorPromptParams.errorMessage; }
ErrorType GetMessagePromptSeverity() { return errorPromptParams.errorSeverity; }