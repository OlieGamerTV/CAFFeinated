#include "imgui_includes.h"
#include "LoadingProcess.h"

LoadingPrompt loadingWidgetParams;
LoadingBarPrompt loadingBarWidgetParams;

// Loading Prompt

bool IsLoadingPromptWidgetOpen() {
	return loadingWidgetParams.showLoadingPrompt;
}

void OpenLoadingPromptWidget() {
	ImGui::OpenPopup("Open Progress", ImGuiPopupFlags_NoOpenOverExistingPopup);
}

/// <summary>
/// Sets up the Loading Popup parameters.
/// </summary>
void SetupLoadingPromptWidget(const char* message) {

	loadingWidgetParams.showLoadingPrompt = true;
	strcpy_s(loadingWidgetParams.loadingMessage, 1024, message);
}

/// <summary>
/// Clears the parameters set for the Loading Popup.
/// </summary>
void CloseLoadingPromptWidget() {
	loadingWidgetParams.showLoadingPrompt = false;
	memset(loadingBarWidgetParams.loadingMessage, 0, 1024);
}

void DisplayLoadingPromptWindow() {
	if (ImGui::BeginPopupModal("Open Progress", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) {
		ImGui::Text(loadingWidgetParams.loadingMessage);
		//ImGui::Text("Currently opening the file. Please wait.");
		ImGui::SameLine();

		DrawSpinnerWheel();

		if (!loadingWidgetParams.showLoadingPrompt) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}


// Loading Bar Prompt

bool IsLoadingBarPromptWidgetOpen() {
	return loadingBarWidgetParams.showLoadingBarPrompt;
}

void OpenLoadingBarPromptWidget() {
	ImGui::OpenPopup("Current Progress", ImGuiPopupFlags_NoOpenOverExistingPopup);
}

/// <summary>
/// Sets up the Loading Bar Popup parameters.
/// </summary>
/// <param name="message">The message that appears on the popup.</param>
/// <param name="totalAmount"></param>
void SetupLoadingBarPromptWidget(const char* message, int totalAmount) {

	loadingBarWidgetParams.showLoadingBarPrompt = true;
	strcpy_s(loadingBarWidgetParams.loadingMessage, 1024, message);
	loadingBarWidgetParams.totalAmount = totalAmount;
	loadingBarWidgetParams.currentSaved = 0;
}

/// <summary>
/// Clears the parameters set for the Loading Bar Popup.
/// </summary>
void CloseLoadingBarPromptWidget() {

	loadingBarWidgetParams.showLoadingBarPrompt = false;
	memset(loadingBarWidgetParams.loadingMessage, 0, 1024);
	loadingBarWidgetParams.totalAmount = 0;
	loadingBarWidgetParams.currentSaved = 0;
}

void IncreaseCurrentSavedOnLoadingWidget() {
	loadingBarWidgetParams.currentSaved++;
}

/// <summary>
/// The Loading Bar Popup Code for ImGui
/// </summary>
void DisplayLoadingBarPromptWidget() {
	if (ImGui::BeginPopupModal("Current Progress", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
		ImVec2 size = ImGui::CalcTextSize(loadingBarWidgetParams.loadingMessage);

		ImGui::ProgressBar(((float)loadingBarWidgetParams.currentSaved / (float)loadingBarWidgetParams.totalAmount));

		DrawSpinnerWheel();
		ImGui::SameLine();

		ImGui::Text(loadingBarWidgetParams.loadingMessage);

		if (!loadingBarWidgetParams.showLoadingBarPrompt) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

/// <summary>
/// Draws an ImGui ASCII representation of a spinner wheel.
/// </summary>
void DrawSpinnerWheel() {
	// Loading Wheel
	if (loadingBarWidgetParams.loadWheel >= 0 && loadingBarWidgetParams.loadWheel <= 3) {
		ImGui::Text("|");
	}
	if (loadingBarWidgetParams.loadWheel >= 4 && loadingBarWidgetParams.loadWheel <= 8) {
		ImGui::Text("/");
	}
	if (loadingBarWidgetParams.loadWheel >= 9 && loadingBarWidgetParams.loadWheel <= 11) {
		ImGui::Text("-");
	}
	if (loadingBarWidgetParams.loadWheel >= 12 && loadingBarWidgetParams.loadWheel <= 15) {
		ImGui::Text("\\");
	}

	loadingBarWidgetParams.loadWheel++;
	if (loadingBarWidgetParams.loadWheel > 15) loadingBarWidgetParams.loadWheel = 0;
}