//
// MumboGarage.cpp : This file contains the 'main' function. Program execution begins and ends there.
// 
// Note: If you're running a LLM through this, there are so many sources to learn programming. It's worth the headache to know what you're writing is correct.
// This is my first proper C++ program. I learned what I know from scratch with a lot of documentation searching and trial and error. Not from an LLM.
//

#pragma comment(lib, "OpenGL32.lib")

#ifdef NDEBUG
#define IMGUI_DEBUG_PRINTF
#define ASSERT(fmt, ...) ((void)0)
#define PRINT(fmt, ...) ((void)0)
#else
#define ASSERT(fmt, ...) (printf("%s %s %d - "##fmt,__FILE__, __func__, __LINE__, __VA_ARGS__))
#define PRINT(fmt, ...) (printf(fmt, __VA_ARGS__))
#endif

#ifdef _WIN32 
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

//Include all the necessary ImGui things we need.
#include "imgui_includes.h"

#include <iostream>
#include <thread>
#include <atomic>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nfd.h>
#include <nfd_glfw3.h>
#include <squish.h>
#include <filesystem>
#include "../resource.h"

//Multi-import
#include "Commons.h"

// Import the custom code
#include "CommonReader.h"
#include "MumboGarage.h"
#include "Bundle.h"
#include "StreamBundle.h"

#include "DarkPackage.h"
#include "KameoDatabase.h"

#include "GhoulDemand.h"
#include "GhoulBundle.h"
#include "GhoulFileTypes.h"

#include "PinataDBBundle.h"

#include "RPK.h"

#include "CaffFileTypes.h"
#include "Save.h"
#include "FlagCounterCommons.h"
#include "BaseMarker.h"
#include "Challenge.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Conker
#include "ConkerFileTypes.h"

// Xbox-specific stuff
#include "xbox_texture.h"
#include "xenon_texture.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_NATIVE_INCLUDE_NONE
#include <GLFW/glfw3native.h>

// The Vehicle Editor Window
#ifndef VEHICLE_WINDOW
#include "VehicleWindow.h"
#endif

// The Loctext Editor Window
#include "LoctextWindow.h"

// Error Message Handler
#ifndef MSG_WINDOW
#include "MessageHandle.h"
#endif

#ifndef MARKER_WINDOW
#include "MarkerWindow.h"
#endif

#ifndef SCRIPT_WINDOW
#include "ScriptWindow.h"
#endif

#include "LoadingProcess.h"


// Permanent reference to the window so it can be fetched from anywhere.
GLFWwindow* window;

// The filename of the currently opened file.
char currentFileName[MAX_PATH];

// Pointers to either a bundle or streambundle struct.
static CaffType fileType = CaffType::NONE;
static BundleFile bundleFile;
static StreamBundle* streamBundleFile;

static GhoulDemand ghoulDemandFile;
static GhoulBundle ghoulBundleFile;

static DBBundle PinataDbBundleFile;

static RPKFile* rpkFile;

// Various file allocations;
static int assetType = -1;
static int fileIdx = -1;
static int fileId = -1;
static Texture* activeTex;
static Manifest* activeManifest;
static Script* activeScript;
static SaveData* activeSave;

// Dark Package
static DarkPackageFile activeDarkPackageFile;

// Kameo Database
static KameoDBFile activeKameoDBFile;

// Conker File Types
static ConkerTextureFile activeConkerTex;

static TextureFile* activeGhoulTex;
GLuint tempTexInt = -1;

BundleSetup bundleSetup;

bool isTexSetup = false;

GLuint RC_PNG_ANIMICON;
GLuint RC_PNG_AUDIOICON;
GLuint RC_PNG2;
GLuint RC_PNG_VEHICON;
GLuint RC_PNG_VEHBLOCKICON;
GLuint RC_PNG_LISTICON;
GLuint RC_PNG_CHALICON;
GLuint RC_PNG_HAVOKICON;
GLuint RGBA_TEST;
GLuint DXT1_TEST;
GLuint DXT3_TEST;

// The ImGuiWindow
static ImGuiGarageWindow imGuiWindowInfo;

// For release builds (Where you just need the window)
#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
	return mainWindowCode();
}
#endif

// For debug builds or any existing non-Windows platforms ig (Where we need a console to debug stuff)
int main() {
	return mainWindowCode();
}

/// <summary>
/// The main set up and loop of the GLFW Window.
/// </summary>
/// <returns></returns>
int mainWindowCode() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Set the locale
	setlocale(LC_ALL, "en_US.UTF-8");

	float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
	window = glfwCreateWindow((int)(1280 * main_scale), (int)(800 * main_scale), "CAFFeinated", NULL, NULL);
	if (window == NULL)
	{
		ASSERT("Failed to create GLFW window\n");
		//std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	if (glfwExtensionSupported("GLX_EXT_swap_control_tear")) {
		glfwSwapInterval(1); // Enable vsync
	}
	glfwSetWindowSizeLimits(window, 960 * main_scale, 600 * main_scale, GLFW_DONT_CARE, GLFW_DONT_CARE); // Set a minimum size for the window. Don't care for maximum.

	// Setup our icons for the window.
	GLFWimage images[2];
	images[0] = LoadResourceImageToGLFWImage(IDB_PNG8, L"PNG"); // Small Icon
	images[1] = LoadResourceImageToGLFWImage(IDB_PNG9, L"PNG"); // Large Icon
	glfwSetWindowIcon(window, 2, images);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		ASSERT("Failed to initialize GLAD");
		return -1;
	}

	glViewport(0, 0, 1280 * main_scale, 800 * main_scale);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	imGuiWindowInfo.defFont = io.Fonts->AddFontDefaultVector();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	// Preload all our icon PNGs so we can use them whenever.
	RC_PNG_ANIMICON = LoadResourceImage(IDB_PNG1, L"PNG");
	RC_PNG2 = LoadResourceImage(IDB_PNG2, L"PNG");
	RC_PNG_VEHICON = LoadResourceImage(IDB_PNG3, L"PNG");
	RC_PNG_VEHBLOCKICON = LoadResourceImage(IDB_PNG4, L"PNG");
	RC_PNG_AUDIOICON = LoadResourceImage(IDB_PNG5, L"PNG");
	RC_PNG_LISTICON = LoadResourceImage(IDB_PNG6, L"PNG");
	RC_PNG_CHALICON = LoadResourceImage(IDB_PNG7, L"PNG");
	RC_PNG_HAVOKICON = LoadResourceImage(IDB_PNG10, L"PNG");

	LoadResourceFont(IDR_FONT1, RT_FONT, 1.25f); // Japanese Font (NotoSansJP)
	LoadResourceFont(IDR_FONT2, RT_FONT, 1.25f); // Korean Font (NotoSansKR)

	imGuiWindowInfo.search = new char[128];
	GetVehicleEditorWindowParameters()->vehicleBlockAddParams.outputPath = (char*)malloc(MAX_PATH);
	memset(imGuiWindowInfo.search, 0, 128);
	bundleSetup.bufferedSaves = (BufferedSave*)malloc(0);
	rpkFile = new RPKFile();
	getLoctextWindowParams()->loctextFilePath = new char[MAX_PATH];

	NFD_Init();

	// Do an initial setup of the display scaling for fonts.
	ImGui::GetStyle().FontSizeBase = 13.f;
	ImGui::GetStyle().FontScaleDpi = main_scale;

	// The main loop
	while (!glfwWindowShouldClose(window))
	{
		try {
			processInput(window);

			glfwSwapBuffers(window);
			glfwPollEvents();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();

			// To accomodate for different display scaling settings
			main_scale = ImGui_ImplGlfw_GetContentScaleForWindow(window);
			ImGui::GetStyle().FontScaleDpi = main_scale;

			ImGui::NewFrame();

			buildBaseImGuiWindow();
			buildTitleBar();

			// Rendering
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			// (Your code clears your framebuffer, renders your other stuff etc.)
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
		catch (int e) {

		}
	}

	NFD_Quit();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();

	disposeAndCloseActiveFile();

	glfwTerminate();
	return 0;
}

// ImGui Windows

// Builds the base for the ImGui window.
void buildBaseImGuiWindow() {
	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context. Refer to examples app!");

	// Verify ABI compatibility between caller code and compiled version of Dear ImGui. This helps detects some build issues.
	IMGUI_CHECKVERSION();

	if (imGuiWindowInfo.db_ShowImGuiDemo) {
		ImGui::ShowDemoWindow(&imGuiWindowInfo.db_ShowImGuiDemo);
	}

	if (imGuiWindowInfo.db_showMetrics) {
		ImGui::ShowMetricsWindow(&imGuiWindowInfo.db_showMetrics);
	}

	if (GetVehicleEditorWindowParameters()->showVehicleEditor) {
		DisplayVehicleEditorBaseWindow();
	}

	if (getLoctextWindowParams()->showLoctextEditor) {
		DisplayLoctextEditorBaseWindow();
	}

	if (imGuiWindowInfo.showSaveEditor) {
		DisplaySaveEditorBaseWindow();
	}

	if (GetMarkerEditorWindowParameters()->showMarkerEditor) {
		DisplayMarkerEditorBaseWindow();
	}

	if (GetScriptEditorWindowParameters()->showScriptEditor) {
		DisplayScriptEditorBaseWindow();
	}

	if (imGuiWindowInfo.showSearch) {
		ShowSearchMenu();
	}

	if (IsLoadingPromptWidgetOpen()) {
		OpenLoadingPromptWidget();
	}
	
	if (IsLoadingBarPromptWidgetOpen()) {
		OpenLoadingBarPromptWidget();
	}

	if (IsMessagePromptShowing()) {
		DisplayMessageWindow();
	}

	buildMainWindow();

	if (imGuiWindowInfo.addRefToStreamed) {
		ImGui::OpenPopup("Add Reference");
		if (streamBundleFile->addStreamedReference(&imGuiWindowInfo.addRefToStreamed)) {
			bundleSetup.isDirty = true;
		}
	}

	if (imGuiWindowInfo.saveData.targetType != NONE) {
		// Bundle Type
		if (imGuiWindowInfo.saveData.targetType == BUNDLEV36) {
			if (bundleFile.V36Bundle->isReady) {
				fileType = CaffType::BUNDLEV36;

				PRINT("Bundle Version: %s\n", bundleFile.V36Bundle->header.versionString);
				PRINT("Bundle Header Size: %d - [%08x]\n", bundleFile.V36Bundle->headerSize(), bundleFile.V36Bundle->headerSize());
				PRINT("Bundle CRC: %u - [%08x]\n", bundleFile.V36Bundle->bundleCRC(), bundleFile.V36Bundle->bundleCRC());
				PRINT("Bundle Num. of Symbol Entries: %d - [%08x]\n", bundleFile.V36Bundle->numOfSymbols(), bundleFile.V36Bundle->numOfSymbols());
				PRINT("Bundle Num. of File Part Entries: %d - [%08x]\n", bundleFile.V36Bundle->numOfFileParts(), bundleFile.V36Bundle->numOfFileParts());

				// Specifically on the bundles for Nuts & Bolts, a manifest file will be present.
				if (bundleFile.V36Bundle->doesFileExist("manifest") != 0) {
					int manifestIdx = bundleFile.V36Bundle->getFileIdxFromSymbol("manifest");
					int manifestFileIdx = bundleFile.V36Bundle->getFileInfoIdxFromFileIdx(manifestIdx, 0);

					// Failsafe to ensure the IDX file was obtained correctly before we initialize and read the manifest file.
					if (manifestFileIdx != -1) {
						activeManifest = new Manifest();

						if (activeManifest == nullptr) {
							throw("An error occured while attempting to create the manifest file.");
						}
						else {
							PRINT("IDX %d\n", manifestFileIdx);

							char* manifestSect = 0;
							manifestSect = bundleFile.V36Bundle->getFileData(currentFileName, manifestFileIdx);
							activeManifest->ReadManifest(manifestSect);
						}
					}
					else {
						FireMessage("The manifest file exists in this file but we failed to get it.\nAid values for most entries won't appear unless this has been loaded correctly.\nReloading the file may solve this issue.\n", ErrorType_Error);
						//PRINT("[ERROR] The manifest file exists in this file but we failed to get it.\n");
						//PRINT("Aid values for most entries won't appear unless this has been loaded correctly.\n");
						//PRINT("Reloading the file may solve this issue.\n");
					}
				}

				imGuiWindowInfo.saveData.targetType = NONE;
				ImGui::CloseCurrentPopup();
			}
		}

		// Streamed Bundle Type
		if (imGuiWindowInfo.saveData.targetType == NB_STREAMBUNDLE) {
			if (streamBundleFile->isReady) {
				fileType = CaffType::NB_STREAMBUNDLE;
				imGuiWindowInfo.saveData.targetType = NONE;
			}
		}

		if (imGuiWindowInfo.saveData.targetType == BUNDLEV31) {
			if (bundleFile.V31Bundle->isReady) {
				if (bundleFile.V31Bundle->type == V31_KameoDB) {
					char* dataSection = bundleFile.V31Bundle->getSectionData(0);
					activeKameoDBFile.ReadDatabaseFile(dataSection, bundleFile.V31Bundle->header.byteswapFlags);
				}

				if (bundleFile.V31Bundle->type == V31_PDZPackage) {
					char* dataSection = bundleFile.V31Bundle->getSectionData(0);
					activeDarkPackageFile.ReadPackageFile(dataSection, bundleFile.V31Bundle->header.byteswapFlags);
				}

				fileType = CaffType::BUNDLEV31;
				imGuiWindowInfo.saveData.targetType = NONE;
			}
		}

		if (imGuiWindowInfo.saveData.targetType == BUNDLEV26) {
			if (bundleFile.V26Bundle->isReady) {
				fileType = CaffType::BUNDLEV26;
				imGuiWindowInfo.saveData.targetType = NONE;
			}
		}

		// Ghoulies Bundle Type
		if (imGuiWindowInfo.saveData.targetType == GHOUL_BUNDLE) {
			if (ghoulBundleFile.isReady == true) {
				fileType = CaffType::GHOUL_BUNDLE;

				imGuiWindowInfo.saveData.targetType = NONE;
			}

			if (ghoulBundleFile.hasErrored == true) {
				fileType = NONE;

				imGuiWindowInfo.saveData.targetType = NONE;
			}
		}

		// Ghoulies Demand Type
		if (imGuiWindowInfo.saveData.targetType == GHOUL_DEMAND) {
			if (ghoulDemandFile.isReady) {
				fileType = CaffType::GHOUL_DEMAND;

				imGuiWindowInfo.saveData.targetType = NONE;
			}
			if (ghoulDemandFile.hasErrored == true) {
				fileType = NONE;

				imGuiWindowInfo.saveData.targetType = NONE;
			}
		}

		// Viva Pinata Database Bundle Type
		if (imGuiWindowInfo.saveData.targetType == PINATA_DBBUNDLE) {
			if (PinataDbBundleFile.isReady) {
				fileType = CaffType::PINATA_DBBUNDLE;

				imGuiWindowInfo.saveData.targetType = NONE;
				CloseLoadingPromptWidget();
			}
			if (PinataDbBundleFile.hasErrored == true) {
				fileType = NONE;

				imGuiWindowInfo.saveData.targetType = NONE;
				CloseLoadingPromptWidget();
			}
		}

		// Rare Replay RPK Type
		if (imGuiWindowInfo.saveData.targetType == RR_RPK) {
			if (rpkFile->isReady) {
				fileType = CaffType::RR_RPK;

				imGuiWindowInfo.saveData.targetType = NONE;
			}
			if (rpkFile->hasErrored == true) {
				fileType = NONE;

				imGuiWindowInfo.saveData.targetType = NONE;
			}
		}

		if (imGuiWindowInfo.saveData.targetType == NONE) {
			CloseLoadingPromptWidget();
		}
	}

	DisplayLoadingPromptWindow();
	DisplayLoadingBarPromptWidget();
}

void buildTitleBar() {
	bool addRef = false;
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ShowMenuFile();
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Search", "CTRL+F", imGuiWindowInfo.showSearch, true)) {
			imGuiWindowInfo.showSearch = !imGuiWindowInfo.showSearch;
		}
		if (ImGui::BeginMenu("Bundles"))
		{
			if (ImGui::BeginMenu("Streamed", fileType == NB_STREAMBUNDLE))
			{
				if (ImGui::MenuItem("Add Reference", NULL)) { imGuiWindowInfo.addRefToStreamed = !imGuiWindowInfo.addRefToStreamed; }
				if (ImGui::MenuItem("Remove Reference", NULL)) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("V0036", fileType == BUNDLEV36))
			{
				if (ImGui::MenuItem("Add File", NULL)) { imGuiWindowInfo.addFileToBundle = !imGuiWindowInfo.addFileToBundle; }

				if (ImGui::MenuItem("Test Recompilation", NULL, false)) {
					std::thread(&TestBundleRecompilation).detach();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::MenuItem("Loctext Editor", NULL, getLoctextWindowParams()->showLoctextEditor)) {
				getLoctextWindowParams()->showLoctextEditor = !getLoctextWindowParams()->showLoctextEditor;
			}
			if (ImGui::MenuItem("Script Editor", NULL, GetScriptEditorWindowParameters()->showScriptEditor)) {
				GetScriptEditorWindowParameters()->showScriptEditor = !GetScriptEditorWindowParameters()->showScriptEditor;
			}
			if (ImGui::MenuItem("Marker Editor", NULL, GetMarkerEditorWindowParameters()->showMarkerEditor)) {
				GetMarkerEditorWindowParameters()->showMarkerEditor = !GetMarkerEditorWindowParameters()->showMarkerEditor;
			}
			ImGui::Separator();
			if (ImGui::BeginMenu("Nuts & Bolts")) {
				// Save Editor
				if (ImGui::MenuItem("Save Editor", NULL, imGuiWindowInfo.showSaveEditor)) {
					imGuiWindowInfo.showSaveEditor = !imGuiWindowInfo.showSaveEditor;
				}
				// Vehicle Editor
				if (ImGui::MenuItem("Vehicle Editor", NULL, GetVehicleEditorWindowParameters()->showVehicleEditor)) {
					GetVehicleEditorWindowParameters()->showVehicleEditor = !GetVehicleEditorWindowParameters()->showVehicleEditor;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu(); 
		}
		if (ImGui::BeginMenu("ImGui Debug Options"))
		{
			if (ImGui::MenuItem("Show Demo Menu", NULL, imGuiWindowInfo.db_ShowImGuiDemo)) {
				imGuiWindowInfo.db_ShowImGuiDemo = !imGuiWindowInfo.db_ShowImGuiDemo;
			}

			if (ImGui::MenuItem("Show Metrics", NULL, imGuiWindowInfo.db_showMetrics)) {
				imGuiWindowInfo.db_showMetrics = !imGuiWindowInfo.db_showMetrics;
			}
			ImGui::EndMenu();
		}

		imGuiWindowInfo.titleBarHeight = ImGui::GetWindowHeight();

		ImGui::EndMainMenuBar();
	}
}

static void ShowSearchMenu() {
	if (ImGui::Begin("Search", &imGuiWindowInfo.showSearch, ImGuiWindowFlags_NoCollapse)) {
		ImGui::PushID("searchStr");
		ImGui::Text("Find what: ");
		ImGui::SameLine();
		ImGui::InputText("##xx", imGuiWindowInfo.search, 128);
		ImGui::PopID();

		ImGui::PushID("searchAid");
		ImGui::Text("Find via aid: ");
		ImGui::SameLine();
		ImGui::InputScalar("##xx", ImGuiDataType_U32, &imGuiWindowInfo.aidSearch, 0, 0, "%08x", ImGuiInputTextFlags_ParseEmptyRefVal);
		ImGui::PopID();
		ImGui::End();
	}
}

static void ShowBundleComparisonMenu() {
	if (ImGui::Begin("Bundle Comparison", NULL, ImGuiWindowFlags_NoCollapse)) {
		ImGui::PushID("searchStr");
		ImGui::Text("Find what: ");
		ImGui::SameLine();
		ImGui::InputText("##xx", imGuiWindowInfo.search, 128);
		ImGui::PopID();

		ImGui::PushID("searchAid");
		ImGui::Text("Find via aid: ");
		ImGui::SameLine();
		ImGui::InputScalar("##xx", ImGuiDataType_U32, &imGuiWindowInfo.aidSearch, 0, 0, "%08x");
		ImGui::PopID();
		ImGui::End();
	}
}

static void ShowMenuFile()
{
	ImGui::MenuItem("File", NULL, false, false);
	
	if (ImGui::BeginMenu("Open")) {
		ImGui::SeparatorText("Main");
		// R1 Bundle Files
		if (ImGui::MenuItem("R1 Bundle File", "Ctrl+O")) { openMenu(); }
		if (ImGui::IsItemHovered()) { ImGui::SetTooltip("The bundle format found in many games using R1 components.\nKnown versions are: V0026, V0031, V0036."); }

		// Ghoulies Bundle Files
		if (ImGui::BeginMenu("Ghoulies Files")) {
			if (ImGui::MenuItem("Bundle", NULL)) { openGhouliesBundle(); }
			if (ImGui::IsItemHovered()) { ImGui::SetTooltip("The bundle format used by Grabbed by the Ghoulies."); }

			if (ImGui::MenuItem("Demand", NULL)) { openGhouliesDemand(); }
			if (ImGui::IsItemHovered()) { ImGui::SetTooltip("The immediate on-demand format used by Grabbed by the Ghoulies for various assets."); }
			ImGui::EndMenu();
		}

		// Misc Files
		ImGui::SeparatorText("Misc.");

		if (ImGui::MenuItem("Viva Pinata DB Files", NULL)) { openPinataDbBundle(); }
		if (ImGui::IsItemHovered()) { ImGui::SetTooltip("The debug package files for Viva Pinata (db_index.txt, debug_hash.bin & debug_pack.bin).\nThese are typically located in \"\\Beta\\debug\\\"."); }

		if (ImGui::MenuItem("Rare Package File", NULL)) { openRareRPKFile(); }
		if (ImGui::IsItemHovered()) { ImGui::SetTooltip("The package file format used by Rare Replay."); }
		ImGui::EndMenu();
	}

	if (ImGui::MenuItem("Save", "Ctrl+S", false, fileType == BUNDLEV36 || fileType == NB_STREAMBUNDLE))
	{ 
		if (fileType == BUNDLEV36) {
			SetupLoadingBarPromptWidget("Currently saving bundle file...", bundleFile.V36Bundle->header.numSections);
			std::thread(writeCaffFile, currentFileName).detach();

			//imGuiWindowInfo.saveData.saveThread.detach();
		}
		else if (fileType == NB_STREAMBUNDLE) {
			SetupLoadingBarPromptWidget("Currently saving streamed bundle file...", streamBundleFile->header.totalFileTotal);
			imGuiWindowInfo.saveData.saveThread = std::thread(writeStreamBundleFile, currentFileName);

			imGuiWindowInfo.saveData.saveThread.detach();
		}
	}
	if (ImGui::MenuItem("Save As..", NULL, false, fileType == BUNDLEV36 || fileType == NB_STREAMBUNDLE)) {
		nfdchar_t* saveFile = new char[MAX_PATH];

		nfdchar_t filename[256];
		char* end = strrchr(currentFileName, '\\');
		int strLen = strlen(currentFileName);
		int remainLeft = strLen - (end - currentFileName);

		strncpy(filename, end + 1, remainLeft);

		if (NFD_SaveDialog(&saveFile, NULL, 0, NULL, filename) == NFD_OKAY) {
			if (fileType == BUNDLEV36) {
				SetupLoadingBarPromptWidget("Currently saving bundle file...", bundleFile.V36Bundle->header.numSections);
				imGuiWindowInfo.saveData.saveThread = std::thread(writeCaffFile, saveFile);

				imGuiWindowInfo.saveData.saveThread.detach();
			}
			else if (fileType == NB_STREAMBUNDLE) {
				SetupLoadingBarPromptWidget("Currently saving streamed bundle file...", streamBundleFile->header.totalFileTotal);
				imGuiWindowInfo.saveData.saveThread = std::thread(writeStreamBundleFile, saveFile);

				imGuiWindowInfo.saveData.saveThread.detach();
			}
		}
		else {
		}
	}
	if (ImGui::MenuItem("Close", NULL, false, fileType != NONE)) { disposeAndCloseActiveFile(); }

	ImGui::Separator();
	if (ImGui::BeginMenu("Export Contents as...", fileType != NONE)) {
		if (ImGui::MenuItem("Raw", NULL, false, fileType != NONE)) {
			if (fileType == BUNDLEV36) {
				SetupLoadingBarPromptWidget("Currently exporting the raw content...", bundleFile.V36Bundle->header.numSections);
			}
			if (fileType == BUNDLEV31) {
				SetupLoadingBarPromptWidget("Currently exporting the raw content...", bundleFile.V31Bundle->header.numOfFiles);
			}
			if (fileType == BUNDLEV26) {
				SetupLoadingBarPromptWidget("Currently exporting the raw content...", bundleFile.V26Bundle->header.numOfFiles);
			}
			if (fileType == GHOUL_BUNDLE) {
				SetupLoadingBarPromptWidget("Currently exporting the raw content...", ghoulBundleFile.entryCount);
			}
			
			imGuiWindowInfo.saveData.saveThread = std::thread(exportFilesFromBundleRaw);

			imGuiWindowInfo.saveData.saveThread.detach();
		}
		if (ImGui::MenuItem("Special", NULL, false, fileType != NONE)) {
			SetupLoadingBarPromptWidget("Currently exporting the file contents...", bundleFile.V36Bundle->header.numSections);
			imGuiWindowInfo.saveData.saveThread = std::thread(exportFilesFromBundleSpecial);

			imGuiWindowInfo.saveData.saveThread.detach();
		}
		ImGui::EndMenu();
	}
	ImGui::Separator();

	if (ImGui::MenuItem("Quit", "Alt+F4")) { glfwSetWindowShouldClose(window, 1); }
}

void exportFilesFromBundleRaw() {
	char* outPath;

	if (NFD_PickFolderU8(&outPath, "fileName") == NFD_OKAY) {
		char lbl[1024];
		char type[32];

		imGuiWindowInfo.saveData.showLoadingPrompt = true;

		if (fileType == BUNDLEV36) {
			for (int i = 0; i < bundleFile.V36Bundle->header.numSections; i++) {
				int fileId = bundleFile.V36Bundle->sectionTable.fileInfos[i].ID - 1;
				int sectId = bundleFile.V36Bundle->sectionTable.fileInfos[i].section - 1;

				memset(lbl, 0, 1024);
				memset(type, 0, 32);

				// Check if the entry label we're reading contains "aid_".
				char* ptr = strstr(bundleFile.V36Bundle->sectionTable.fileLabelTable.fileLabels[fileId].label, "aid_");
				if (ptr != NULL) {
					// If so, copy the contents from the location of that and fetch the asset type.
					strcpy(lbl, ptr);

					assetGetTypeFromString(lbl + 4, type);
				}
				else {
					// Else, just copy the contents of the label to the dest.
					strcpy(lbl, bundleFile.V36Bundle->sectionTable.fileLabelTable.fileLabels[fileId].label);
				}

				//All (properly named) texture files end with "\default.rtx". Filter that out as well.
				if (strcmp(type, "texture") == 0) {
					strtok(lbl, "\\");
				}

				char* buf = (char*)malloc(1024);

				sprintf(buf, "%s\\%s%s", outPath, lbl, bundleFile.V36Bundle->sectionTable.sectionLabels[sectId].label);

				FILE* writeFile = fopen(buf, "wb");

				if (writeFile == NULL) {
					ASSERT("An error occured while creating the output file (%s).\n", buf);
					return;
				}

				char* data = bundleFile.V36Bundle->getFileData(currentFileName, i);

				size_t written = fwrite(data, sizeof(char), bundleFile.V36Bundle->sectionTable.fileInfos[i].dataSize, writeFile);

				if (written != bundleFile.V36Bundle->sectionTable.fileInfos[i].dataSize) {
					ASSERT("An error occured while writing data to output file.\n");
				}

				int flush = fflush(writeFile);

				if (flush != 0) {
					ASSERT("An error occured while flushing data to output file.\n");
				}

				fclose(writeFile);

				free((void*)buf);
				free(data);

				IncreaseCurrentSavedOnLoadingWidget();
			}
		}
		if (fileType == NB_STREAMBUNDLE) {
		}

		if (fileType == BUNDLEV31) {
			for (int i = 0; i < bundleFile.V31Bundle->header.numOfFiles; i++) {
				memset(lbl, 0, 1024);
				memset(type, 0, 32);

				// Check if the entry label we're reading contains "aid_".
				char* ptr = strrchr(bundleFile.V31Bundle->fileInfoTable.debugTable.fileNames[i], '\\');
				if (ptr != NULL) {
					strcpy(lbl, ptr);
				}
				else {
					// Else, just copy the contents of the label to the dest.
					strcpy(lbl, bundleFile.V31Bundle->fileInfoTable.debugTable.fileNames[i]);
				}

				char* buf = (char*)malloc(2048);
				sprintf(buf, "%s", outPath);

				if (strstr(bundleFile.V31Bundle->fileInfoTable.debugTable.fileNames[i], "UberRoot") != nullptr) {
					char* charBuffer = (char*)malloc(2048);
					strcpy(charBuffer, bundleFile.V31Bundle->fileInfoTable.debugTable.fileNames[i] + 0x1A);

					size_t tokCount = strlen(charBuffer);

					if (strstr(bundleFile.V31Bundle->fileInfoTable.debugTable.fileNames[i], "XENONBETA_v1") != nullptr) {
						tokCount -= strlen("XENONBETA_v1\\");
					}

					char* tok = strtok(charBuffer, "\\");

					int curLen = 0;
					while (curLen < tokCount - strlen(lbl)) {
						strcat(buf, "\\");
						strcat(buf, tok);
						std::filesystem::create_directory(buf);

						PRINT("%s\n", tok);

						tok = strtok(NULL, "\\");

						PRINT("%d - %d\n", curLen, tokCount);
						curLen += strcspn(bundleFile.V31Bundle->fileInfoTable.debugTable.fileNames[i] + 0x1A + curLen, "\\") + 1;
					}

					free(charBuffer);
				}

				strcat(buf, "\\");
				strcat(buf, lbl);

				FILE* writeFile = fopen(buf, "wb");

				if (writeFile == NULL) {
					ASSERT("An error occured while creating the output file (%s).\n", buf);
					return;
				}

				PRINT("Exporting %s.\n", buf);

				for (int s = 0; s < bundleFile.V31Bundle->header.numSectionTypes; s++) {
					int fileInfoIDX = bundleFile.V31Bundle->GetMatchingFileInfoIdx(i + 1, s + 1);
					if (fileInfoIDX == -1) break;

					char* data = bundleFile.V31Bundle->getFileData(currentFileName, fileInfoIDX);

					size_t written = fwrite(data, sizeof(char), bundleFile.V31Bundle->fileInfoTable.fileInfoEntries[fileInfoIDX].dataSize, writeFile);

					if (written != bundleFile.V31Bundle->fileInfoTable.fileInfoEntries[fileInfoIDX].dataSize) {
						ASSERT("An error occured while writing data to output file.\n");
					}

					free(data);
				}

				int flush = fflush(writeFile);

				if (flush != 0) {
					ASSERT("An error occured while flushing data to output file.\n");
				}

				fclose(writeFile);

				free((void*)buf);

				IncreaseCurrentSavedOnLoadingWidget();
			}
		}
	}

	CloseLoadingBarPromptWidget();
}

void exportFilesFromBundleSpecial() {
	char* outPath;

	if (NFD_PickFolderU8(&outPath, "fileName") == NFD_OKAY) {
		char lbl[1024];
		char type[32];

		imGuiWindowInfo.saveData.showLoadingPrompt = true;

		if (fileType == BUNDLEV36) {
			for (int i = 0; i < bundleFile.V36Bundle->header.numSections; i++) {
				int fileId = bundleFile.V36Bundle->sectionTable.fileInfos[i].ID - 1;
				int sectId = bundleFile.V36Bundle->sectionTable.fileInfos[i].section - 1;

				memset(lbl, 0, 1024);
				memset(type, 0, 32);

				// Check if the entry label we're reading contains "aid_".
				char* ptr = strstr(bundleFile.V36Bundle->sectionTable.fileLabelTable.fileLabels[fileId].label, "aid_");
				if (ptr != NULL) {
					// If so, copy the contents from the location of that and fetch the asset type.
					strcpy(lbl, ptr);

					assetGetTypeFromString(lbl + 4, type);
				}
				else {
					// Else, just copy the contents of the label to the dest.
					strcpy(lbl, bundleFile.V36Bundle->sectionTable.fileLabelTable.fileLabels[fileId].label);
				}

				// All (properly named) texture files end with "\default.rtx". Filter that out as well.
				if (strcmp(type, "texture") == 0) {
					strtok(lbl, "\\");
				}

				char* buf = (char*)malloc(1024);

				int typeId = GetAssetIDFromType(type);
				switch (typeId) {
				case 0x11:
					sprintf(buf, "%s\\%s.ini", outPath, lbl);
					break;
				default:
					sprintf(buf, "%s\\%s%s", outPath, lbl, bundleFile.V36Bundle->sectionTable.sectionLabels[sectId].label);
					break;
				}

				char* data = bundleFile.V36Bundle->getFileData(currentFileName, i);

				switch (typeId) {
				case 0x11:
					Loctext * tempLoc = new Loctext();
					tempLoc->ReadLoctext(data);

					tempLoc->ExportToFileRaw(buf);

					delete(tempLoc);
					break;
				}

				free((void*)buf);
				free(data);
				imGuiWindowInfo.saveData.currentSaved++;
			}
		}
		else if (fileType == NB_STREAMBUNDLE) {
		}
	}

	imGuiWindowInfo.saveData.showLoadingPrompt = false;
}

static void openMenu() {
	nfdchar_t* outPath = NULL;

	if (NFD_OpenDialogU8(&outPath, NULL, 0, "") == NFD_OKAY) {

		if (fileType != NONE) {
			disposeAndCloseActiveFile();
		}

		SetupLoadingPromptWidget("Currently opening the file. Please wait.");

		sprintf(currentFileName, "%s", outPath);

		PRINT("File picked: %s\n", currentFileName);

		// Note: DON'T FORGET TO DETACH THE LOAD THREAD. IT CAUSES ISSUES IF YOU DON'T (SUCH AS CRASHING).
		std::thread(&readCaffFile).detach();
	}
	else {
	}
}

static void openPinataDbBundle() {
	nfdchar_t* outPath = NULL;

	if (NFD_PickFolderU8(&outPath, "") == NFD_OKAY) {

		SetupLoadingPromptWidget("Currently opening the file. Please wait.");

		sprintf(currentFileName, "%s", outPath);
		PRINT("Folder picked: %s\n", currentFileName);

		strcpy(imGuiWindowInfo.saveData.loadingMessage, "Currently loading Viva Pinata DB Files...");
		imGuiWindowInfo.saveData.targetType = PINATA_DBBUNDLE;
		//imGuiWindowInfo.saveData.loadThread = std::thread(&DBBundle::readStandaloneDbBundleFiles, PinataDbBundleFile, currentFileName);
		//imGuiWindowInfo.saveData.loadThread.detach();
		std::thread(&readOtherSupportedFile, CaffType::PINATA_DBBUNDLE).detach();
	}
	else {
	}
}

static void openGhouliesBundle() {
	nfdchar_t* outPath = NULL;

	if (NFD_OpenDialogU8(&outPath, NULL, 0, "") == NFD_OKAY) {

		if (fileType != NONE) {
			disposeAndCloseActiveFile();
		}

		SetupLoadingPromptWidget("Currently opening the file. Please wait.");

		sprintf(currentFileName, "%s", outPath);
		PRINT("File picked: %s\n", currentFileName);

		imGuiWindowInfo.saveData.targetType = GHOUL_BUNDLE;
		std::thread(&readOtherSupportedFile, CaffType::GHOUL_BUNDLE).detach();
		/*imGuiWindowInfo.saveData.loadThread = std::thread(&GhoulBundle::readStandaloneBundleFile, ghoulBundleFile, currentFileName);
		imGuiWindowInfo.saveData.loadThread.detach();*/
	}
	else {
	}
}

static void openGhouliesDemand() {
	nfdchar_t* outPath = NULL;

	if (NFD_OpenDialogU8(&outPath, NULL, 0, "") == NFD_OKAY) {

		if (fileType != NONE) {
			disposeAndCloseActiveFile();
		}

		SetupLoadingPromptWidget("Currently opening the file. Please wait.");

		sprintf(currentFileName, "%s", outPath);
		PRINT("File picked: %s\n", currentFileName);

		imGuiWindowInfo.saveData.targetType = GHOUL_DEMAND;
		std::thread(&readOtherSupportedFile, CaffType::GHOUL_DEMAND).detach();
		/*imGuiWindowInfo.saveData.loadThread = std::thread(&GhoulDemand::readStandaloneDemandFile, ghoulDemandFile, currentFileName);
		imGuiWindowInfo.saveData.loadThread.detach();*/
	}
	else {
	}
}

static void openRareRPKFile() {
	nfdchar_t* outPath = NULL;
	nfdu8filteritem_t filters[1] = { { "RPK File", "rpk,rpk.gz" } };

	if (NFD_OpenDialogU8(&outPath, filters, 1, "") == NFD_OKAY) {

		if (fileType != NONE) {
			disposeAndCloseActiveFile();
		}

		SetupLoadingPromptWidget("Currently opening the file. Please wait.");

		sprintf(currentFileName, "%s", outPath);
		PRINT("File picked: %s\n", currentFileName);

		if (rpkFile == nullptr) {
			rpkFile = new RPKFile();
		}

		imGuiWindowInfo.saveData.targetType = RR_RPK;
		imGuiWindowInfo.saveData.loadThread = std::thread(&RPKFile::readStandaloneRPKFile, rpkFile, currentFileName);
		imGuiWindowInfo.saveData.loadThread.detach();
	}
	else {
	}
}

/// <summary>
/// Constructs the Filelist, File Info and Properties windows.
/// </summary>
void buildMainWindow() {

	displayAvailableFilesList(imGuiWindowInfo.titleBarHeight);
	displayFileInfo(imGuiWindowInfo.titleBarHeight);
	displayProperties(imGuiWindowInfo.titleBarHeight);
}

#pragma region File Info
void displayFileInfo(float barHeight) {
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	ImGui::SetNextWindowPos(ImVec2(0, barHeight));
	ImGui::SetNextWindowSize(ImVec2(width / 2.5, (height / 2) - barHeight));

	if (ImGui::Begin("Bundle File", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
		switch (fileType)
		{
		case NONE:
			ImGui::Text("No File Present.");
			break;
		case BUNDLEV36:
			displayBundleInfo();
			break;
		case BUNDLEV31:
			displayBundleV31Info();
			break;
		case NB_STREAMBUNDLE:
			displayStreamBundleInfo();
			break;
		case GHOUL_BUNDLE:
			displayGhoulBundleInfo();
			break;
		case GHOUL_DEMAND:
			displayGhoulDemandInfo();
			break;
		case RR_RPK:
			displayRPKInfo();
			break;
		}
		ImGui::End();
	}
}

void displayBundleInfo() {
	char filename[256];
	char* end = strrchr(currentFileName, '\\');
	int strLen = strlen(currentFileName);
	int remainLeft = strLen - (end - currentFileName);

	try {
		strncpy(filename, end + 1, remainLeft);
		ImGui::Text("Filename: %s", filename);
		ImGui::SeparatorText("Bundle Information");
		ImGui::Text("Bundle Version: %s", bundleFile.V36Bundle->header.versionString);
		ImGui::Text("Bundle CRC: %08x", bundleFile.V36Bundle->header.headerHash);
		if (activeManifest != nullptr) {
			time_t time = activeManifest->timestamp;

			ImGui::Text("Timestamp: %08x -> %s", activeManifest->timestamp, ctime(&time));
		}
		bool val = bundleFile.V36Bundle->header.compression;
		ImGui::Checkbox("Compressed", &val);
		//ImGui::Text("Bundle Compression Status: %s", (bundleFile.V36Bundle->header.compression == 1 ? "Compressed" : "Uncompressed"));
		ImGui::Spacing();
		ImGui::SeparatorText("Bundle Sections");
		ImGui::Text("Section Table - Uncompressed Size: %d", bundleFile.V36Bundle->header.sectionTableUncompedSize);
		ImGui::Text("Section Table - Compressed Size: %d", bundleFile.V36Bundle->header.sectionTableCompedSize);
		ImGui::Text("File Table - Uncompressed Size: %d", bundleFile.V36Bundle->header.fileTableCompedSize);
		ImGui::Text("File Table - Compressed Size: %d", bundleFile.V36Bundle->header.fileTableUncompedSize);
		ImGui::Spacing();
		ImGui::Text("Available Section(s): {");
		for (int i = 0; i < bundleFile.V36Bundle->header.numSectionTypes; i++) {
			ImGui::Text("     %s", bundleFile.V36Bundle->sectionTable.sectionLabels[i].label);
		}
		ImGui::Text("}");
	}
	catch (std::exception e) {
		PRINT("%s\n", e.what());
	}
}

void displayBundleV31Info() {
	char filename[256];
	char* end = strrchr(currentFileName, '\\');
	int strLen = strlen(currentFileName);
	int remainLeft = strLen - (end - currentFileName);

	try {
		strncpy(filename, end + 1, remainLeft);
		ImGui::Text("Filename: %s", filename);
		ImGui::SeparatorText("Bundle Information");
		ImGui::Text("Bundle Version: %s", bundleFile.V31Bundle->header.versionString);
		ImGui::Text("Bundle CRC: %08x", bundleFile.V31Bundle->header.headerHash);
		ImGui::Text("Bundle Compression Status: %s", (bundleFile.V31Bundle->header.compression == 1 ? "Compressed" : "Uncompressed"));
		ImGui::Spacing();
		ImGui::SeparatorText("Bundle Sections");
		ImGui::Spacing();
		ImGui::Text("Available Section(s): {");
		for (int i = 0; i < bundleFile.V31Bundle->header.numSectionTypes; i++) {
			ImGui::Text("     %s", bundleFile.V31Bundle->sectionEntries[i].sectionName);
		}
		ImGui::Text("}");
	}
	catch (int e) {
	}
}

void displayStreamBundleInfo() {
	char filename[256];
	char* end = strrchr(currentFileName, '\\');
	int strLen = strlen(currentFileName);
	int remainLeft = strLen - (end - currentFileName);

	try {

		time_t time = streamBundleFile->header.timestamp;

		strncpy(filename, end + 1, remainLeft);
		ImGui::Text("Filename: %s", filename);
		ImGui::SeparatorText("Streamed Bundle Information");
		ImGui::Text("Timestamp: %08x -> %s", streamBundleFile->header.timestamp, ctime(&time));
		ImGui::Spacing();
		ImGui::SeparatorText("Bundle Sections");
		ImGui::Text("TODO: fill this area.");
		ImGui::Spacing();
		ImGui::Text("References to other streamed bundles: {");
		for (int i = 0; i < streamBundleFile->header.referenceTableCount; i++) {
			int slot1 = (streamBundleFile->header.referenceTable[i] >> 24) & 0xFF;
			int slot2 = (streamBundleFile->header.referenceTable[i] >> 16) & 0xFF;
			int slot3 = (streamBundleFile->header.referenceTable[i] >> 8) & 0xFF;
			int slot4 = streamBundleFile->header.referenceTable[i] & 0xFF;

			ImGui::Text("     GAME:\\Bundle\\%02x\\%02x%02x%02x", slot1, slot2, slot3, slot4);
		}
		ImGui::Text("}");
	}
	catch (int e) {
	}
}

void displayGhoulBundleInfo() {
	char filename[256];
	char* end = strrchr(currentFileName, '\\');
	int strLen = strlen(currentFileName);
	int remainLeft = strLen - (end - currentFileName);

	try {
		strncpy(filename, end + 1, remainLeft);
		ImGui::Text("Filename: %s", filename);
		ImGui::SeparatorText("Bundle Information");
		bool isComped = ghoulBundleFile.isCompressed;
		ImGui::Checkbox("Is Compressed", &isComped);

		ImGui::Spacing();
		ImGui::SeparatorText("Bundle Sections");
		ImGui::Text("Data Section - Offset: %d", ghoulBundleFile.dataSectOffset);
		ImGui::Text("Data Section - Size: %d", ghoulBundleFile.dataSectSize);

		if (ghoulBundleFile.gpuSectOffset != 0) {
			ImGui::Text("GPU Section - Offset: %d", ghoulBundleFile.gpuSectOffset);
			ImGui::Text("GPU Section - Size: %d", ghoulBundleFile.gpuSectSize);
		}
	}
	catch (int e) {
	}
}

void displayGhoulDemandInfo() {
	char filename[256];
	char* end = strrchr(currentFileName, '\\');
	int strLen = strlen(currentFileName);
	int remainLeft = strLen - (end - currentFileName);

	try {
		strncpy(filename, end + 1, remainLeft);
		ImGui::Text("Filename: %s", filename);
		ImGui::SeparatorText("Bundle Information");

		time_t time = ghoulDemandFile.timestamp;

		ImGui::Text("Timestamp: %08x -> %s", ghoulDemandFile.timestamp, ctime(&time));

		ImGui::Spacing();
		ImGui::SeparatorText("Bundle Sections");
		ImGui::Text("Data Section - Offset: %d", ghoulDemandFile.dataSectOffset);
		ImGui::Text("Data Section - Size: %d", ghoulDemandFile.dataSectSize);

		if (ghoulDemandFile.gpuSectOffset != 0) {
			ImGui::Text("GPU Section - Offset: %d", ghoulDemandFile.gpuSectOffset);
			ImGui::Text("GPU Section - Size: %d", ghoulDemandFile.gpuSectSize);
		}
	}
	catch (int e) {
	}
}

void displayRPKInfo() {
	char filename[256];
	char* end = strrchr(currentFileName, '\\');
	int strLen = strlen(currentFileName);
	int remainLeft = strLen - (end - currentFileName);

	try {
		strncpy(filename, end + 1, remainLeft);
		ImGui::Text("Filename: %s", filename);
		ImGui::SeparatorText("File Information");

		ImGui::Spacing();
		ImGui::Text("Num. of Files - %d", rpkFile->fileCount);
	}
	catch (int e) {
	}
}
#pragma endregion

#pragma region Properties
void displayProperties(float barHeight) {
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	ImGui::SetNextWindowPos(ImVec2(0, (height / 2)));
	ImGui::SetNextWindowSize(ImVec2(width / 2.5, (height / 2) + barHeight));

	if (ImGui::Begin("Properties", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
		switch (fileType)
		{
		case BUNDLEV36:
			displayActiveFileProperty();
			break;
		case NB_STREAMBUNDLE:
			displayActiveStreamBundleFileProperty();
			break;
		case BUNDLEV31:
			displayActiveBundleV31Property();
			break;
		case BUNDLEV26:
			displayActiveBundleV26Property();
			break;
		case GHOUL_BUNDLE:
			displayActiveGhoulBundleProperty();
			break;
		case GHOUL_DEMAND:
			displayActiveGhoulDemandProperty();
			break;
		case RR_RPK:
			displayActiveRPKFileProperty();
			break;
		case PINATA_DBBUNDLE:
			displayActivePinataDbBundleFileProperty();
			break;
		}

		ImGui::End();
	}
}

void displayActiveFileProperty() {
	if (fileIdx == -1) return;

	char lbl[1024];

	int totalFileSize = 0;
	ImGui::SeparatorText("General File Properties");

	char* tStamp = { 0 };
	char* suffix = { 0 };
	memset(lbl, 0, 1024);

	if (bundleFile.V36Bundle != nullptr) {
		char* ptr = strstr(bundleFile.V36Bundle->sectionTable.fileLabelTable.fileLabels[fileIdx - 1].label, "aid_");
		if (ptr != NULL) {
			strcpy(lbl, ptr);
		}
		else {
			ptr = strrchr(bundleFile.V36Bundle->sectionTable.fileLabelTable.fileLabels[fileIdx - 1].label, '\\');
			if (ptr != NULL) {
				strcpy(lbl, ptr + 1);
			}
			else {
				strcpy(lbl, bundleFile.V36Bundle->sectionTable.fileLabelTable.fileLabels[fileIdx - 1].label);
			}
		}

		if (strchr(lbl, ',') != NULL) {
			strtok(lbl, ",");

			tStamp = strtok(NULL, ",");
			suffix = strtok(NULL, ",");
		}

		if (strchr(lbl, '\\') != NULL) {
			strtok(lbl, "\\");
		}

		float pos = ImGui::GetCursorPosY();

		ImGui::Text("Filename: ");
		ImGui::SameLine(0, -0.25f);
		ImGui::SetCursorPosY(pos - 3);
		ImGui::InputText("##xxlbl", lbl, ImGuiInputTextFlags_ReadOnly);

		if (activeManifest != nullptr) {
			unsigned int fileHash = activeManifest->GetAidHash(fileId);

			if (fileHash != 0) {
				if (fileHash != 0) {
					pos = ImGui::GetCursorPosY();

					ImGui::SetCursorPosY(pos + 3);
					ImGui::Text("Aid Hash: ");
					ImGui::SameLine(0, -0.25f);
					ImGui::SetCursorPosY(pos);
					ImGui::InputScalar("##xxhash", ImGuiDataType_U32, &fileHash, 0, 0, "%08X", ImGuiInputTextFlags_ReadOnly);
				}
			}
		}

		if (tStamp != NULL) {
			std::time_t stamp = atol(tStamp);
			//std::time_t time = std::time(&stamp);
			ImGui::Text("Timestamp: %s -> %s", tStamp, ctime(&stamp));
		}

		if (suffix != NULL) {
			ImGui::Text("Suffix: %s", suffix);
		}

		ImGui::Text("Sections: ");
		ImGui::SameLine();

		for (int s = 0; s < bundleFile.V36Bundle->header.numSections; s++) {
			if (bundleFile.V36Bundle->sectionTable.fileInfos[s].ID == fileIdx) {
				totalFileSize = totalFileSize + bundleFile.V36Bundle->sectionTable.fileInfos[s].dataSize;
				char* label = bundleFile.V36Bundle->sectionTable.sectionLabels[bundleFile.V36Bundle->sectionTable.fileInfos[s].section - 1].label;
				ImGui::Text("%s ", label);
				ImGui::SameLine();
			}
		}

		ImGui::NewLine();
		ImGui::Text("Total File Size: %d", totalFileSize);
	}

	ImGui::SeparatorText("File Options");

	int idData = 0;

	ImGui::Text("Export Available Sections:");
	ImGui::SameLine();
	char sects[96];
	memset(sects, 0, 96);
	int availableSects = 0;
	ImGui::PushID("export");
	for (int s = 0; s < bundleFile.V36Bundle->header.numSections; s++) {
		if (bundleFile.V36Bundle->sectionTable.fileInfos[s].ID == fileIdx) {
			idData = s;
			availableSects = availableSects + 1;
			char* label = bundleFile.V36Bundle->sectionTable.sectionLabels[bundleFile.V36Bundle->sectionTable.fileInfos[s].section - 1].label;
			char type[16];
			ImGui::PushID(label);
			if (ImGui::Button(label)) {
				char* activeSect = 0;
				activeSect = bundleFile.V36Bundle->getFileData(currentFileName, s);

				char file[1024];
				memset(file, 0, 1024);

				assetGetTypeFromString(lbl + 4, type);

				if (strcmp(type, "texture") == 0) {
					strtok(lbl, "\\");
				}

				strcat(file, lbl);
				strcat(file, label);

				writeDataToFile(lbl, label, activeSect, bundleFile.V36Bundle->sectionTable.fileInfos[s].dataSize);
				free(activeSect);
			}
			ImGui::PopID();
			ImGui::SameLine();
		}
	}
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::Text("Import to Available Sections:");
	ImGui::SameLine();
	memset(sects, 0, 96);
	ImGui::PushID("import");
	for (int s = 0; s < bundleFile.V36Bundle->header.numSections; s++) {
		if (bundleFile.V36Bundle->sectionTable.fileInfos[s].ID == fileIdx) {
			char* label = bundleFile.V36Bundle->sectionTable.sectionLabels[bundleFile.V36Bundle->sectionTable.fileInfos[s].section - 1].label;
			ImGui::PushID(label);
			if (ImGui::Button(label)) {
				char* importPath;

				if (NFD_OpenDialog(&importPath, NULL, 0, "") == NFD_OKAY) {
					BufferedSave savedFile;

					FILE* importedFile = fopen(importPath, "rb");

					fseek(importedFile, 0L, SEEK_END);
					int len = ftell(importedFile);
					fseek(importedFile, 0L, SEEK_SET);

					savedFile.savedData = (char*)malloc(len);

					fread(savedFile.savedData, 1, len, importedFile);
					fclose(importedFile);
					savedFile.dataSize = len;
					savedFile.fileId = bundleFile.V36Bundle->sectionTable.fileInfos[s].ID;
					savedFile.sect = bundleFile.V36Bundle->sectionTable.fileInfos[s].section;

					bundleSetup.AddToSaveBuffer(savedFile);
				}
			}
			ImGui::PopID();
			ImGui::SameLine();
		}
	}
	ImGui::PopID();

	ImGui::NewLine();

	switch (assetType)
	{
	case 0:
		if (ImGui::Button("Load Vehicle")) {
			char* activeSect = 0;
			activeSect = bundleFile.V36Bundle->getFileData(currentFileName, idData);

			FreeVehicleWindowVehicleMemory();

			// Need to quickly grab the number of parts first in order to allocate memort
			unsigned short numOfParts = 0;

			memcpy(&numOfParts, activeSect, sizeof(unsigned short));

			numOfParts = flipEndian(numOfParts);
			PRINT("Loading vehicle from file \"%s\"\nTotal Vehicle Filesize = %d.\n", lbl, numOfParts);

			AllocateVehicleWindowVehicleMemory(numOfParts);
			GetVehicleWindowVehiclePtr()->ReadVehicle(activeSect, false);

			SetupVehicleEditorWindow(false, lbl);
		}
		break;
	case 0x1:
		//Failsafe
		if (activeTex == nullptr) {
			activeTex = new Texture();
		}

		// If we've loaded up a new file, refresh the display.
		if (activeTex->refresh) {
			int sectMain = bundleFile.V36Bundle->getFileInfoIdxFromFileIdx(fileId, 0);
			char* dataSect = bundleFile.V36Bundle->getFileData(currentFileName, sectMain);

			activeTex->ReadTextureInfo(dataSect);

			free(dataSect);

			activeTex->refresh = false;
		}

		if (ImGui::Button("Export Texture")) {
			char* outPath;
			char filePath[2048];

			if (NFD_PickFolderU8(&outPath, "fileName") == NFD_OKAY) {
				try {
					int sectGpu = bundleFile.V36Bundle->getGPUFileInfoIdxFromFileIdx(fileId);
					char* gpuSect = bundleFile.V36Bundle->getFileData(currentFileName, sectGpu);

					int bpp = 4;

					int width = activeTex->headerSect.width;
					int height = activeTex->headerSect.height;

					if (activeTex->headerSect.textureType == TEXTURE_FORMAT::TEX_DXT1) {
						bpp = 2;
					}

					if (activeTex->headerSect.textureType == TEXTURE_FORMAT::TEX_DXT3) {
						bpp = 2;
					}

					if (activeTex->headerSect.textureType == TEXTURE_FORMAT::TEX_DXT5) {
						bpp = 2;
					}

					int chunkSize = (activeTex->headerSect.width * activeTex->headerSect.height) * bpp;

					if (activeTex->headerSect.gpuOffsTablePos == 0) {
						chunkSize = bundleFile.V36Bundle->sectionTable.fileInfos[sectGpu].dataSize;
					}

					for (int i = 0; i < activeTex->headerSect.frameCount; i++) {
						memset(filePath, 0, 128);
						sprintf(filePath, "%s/%s_%03d.png", outPath, lbl, i);
						PRINT("Export image %s_%03d.png to %s.\n", lbl, i, outPath);

						char* texData = (char*)malloc(chunkSize);
						memset(texData, 0, chunkSize);
						memcpy(texData, gpuSect + activeTex->headerSect.gpuOffsTable[i], chunkSize);

						unsigned char* imgData = GetRawImageData_Banjo(texData, activeTex->headerSect.width, activeTex->headerSect.height, activeTex->headerSect.textureType, activeTex->headerSect.isSwizzled);

						int success = stbi_write_png(filePath, activeTex->headerSect.width, activeTex->headerSect.height, 4, imgData, activeTex->headerSect.width * 4);

						if (success == 1) {
							PRINT("Image has been successfully exported.\n");
						}
						else {
							ASSERT("An error occured while trying to export the image. Error Code 0x%08x.\n", success);
						}

						free(texData);
						free(imgData);
					}

					free(gpuSect);
				}
				catch (int err) {
					ASSERT("An error occured while trying to export the image. Error Code 0x%08x.\n", err);
				}
			}
		}

		ImGui::SeparatorText("Texture Info");
		ImGui::Text("Format: %s (%02X)", GetXenonTextureFormatName(activeTex->headerSect.textureType), activeTex->headerSect.textureType);
		ImGui::Text("Width/Height: %d / %d", activeTex->headerSect.width, activeTex->headerSect.height);
		ImGui::Text("Frame Count: %d", activeTex->headerSect.frameCount);
		break;
	case 0xD:
		if (ImGui::Button("Load Marker File")) {
			char* activeSect = 0;
			activeSect = bundleFile.V36Bundle->getFileData(currentFileName, idData);
			PRINT("Loading marker data from file \"%s\".\n", lbl);

			SetupLoadingPromptWidget("Currently loading the marker file. Please wait.");
			std::thread(&readMarkerFile, activeSect).detach();
		}
		break;
	case 0x11:
		if (ImGui::Button("Load Localization Text")) {
			char* activeSect = 0;
			activeSect = bundleFile.V36Bundle->getFileData(currentFileName, idData);

			SetupLoadingPromptWidget("Currently loading the loctext file. Please wait.");
			std::thread(&readLoctextFile, activeSect, lbl).detach();
			AssignLoctextFilename(lbl);
			getLoctextWindowParams()->activeLoctext->startEndianness = bundleFile.V36Bundle->header.byteswapFlags;
		}
		break;
	case 0x19:
		if (ImGui::Button("Load Script")) {
			char* activeSect = 0;
			activeSect = bundleFile.V36Bundle->getFileData(currentFileName, idData);

			if (activeScript != nullptr) {
				free(activeScript);
				activeScript = nullptr;
			}

			PRINT("Loading script from file \"%s\"\n", lbl);
			activeScript = (Script*)malloc(bundleFile.V36Bundle->sectionTable.fileInfos[idData].dataSize);
			activeScript->ReadScript(activeSect);

			imGuiWindowInfo.showScriptEditor = true;
		}
		break;
	case 0x3D:
		if (ImGui::Button("Load Challenge File")) {
			char* activeSect = 0;
			activeSect = bundleFile.V36Bundle->getFileData(currentFileName, idData);

			PRINT("Loading challenge data from file \"%s\".\n", lbl);
			int count = 0;
			int offs = 0;

			PRINT("Challenge File Data -> {\n");
			ChallengeNullDef* baseData = (ChallengeNullDef*)malloc(0x2B0); // allocate for the largest possible entry
			memset(baseData, 0, 0x2B0); // clear the space

			while (count < 65535) {
				count++;
				baseData->ParseHeader(activeSect + offs);
				PRINT("Entry %05d\t->\t[%s (%d) Group %d ", count, dbChallengeNames[baseData->header.reqId], baseData->header.reqId, baseData->header.taskID);

				PRINT("Parameters [ ");

				switch (baseData->header.reqId) {
				case dbChallengeRequirement_Common: {
					ChallengeCommonDef* commonData = static_cast<ChallengeCommonDef*>(baseData);
					commonData->ParseCommon(activeSect + offs);
					PRINT("World %s, Challenge %s, Game Style %s ", commonData->worldNameTag, commonData->challengeTag, commonData->gameStyleTag);
				}
												  break;
				case dbChallengeRequirement_StayInVehicle: {
					challengeStayInVehicle* stayInVehData = static_cast<challengeStayInVehicle*>(baseData);
					stayInVehData->ParseStayInVehicle(activeSect + offs);

					PRINT("Can Player Get Out - %s, Can Baddies Get Out - %s ", stayInVehData->canPlayersGetOut == 1 ? "YES" : "NO", stayInVehData->canBaddiesGetOut == 1 ? "YES" : "NO");
				}

														 break;
				}

				PRINT("] ]\n");

				offs += baseData->header.size;

				if (baseData->header.reqId == dbChallengeRequirement_NULL) break;
			}
			PRINT("}\n");

			free(baseData);
			free(activeSect);
		}
		break;
	}
}

void displayActiveBundleV31Property() {

	ImGui::SeparatorText("Bundle Information");

	switch (bundleFile.V31Bundle->type) {
	case V31_Texture:
		ImGui::Text("Type: Texture");
		break;
	case V31_ConkerText:
		ImGui::Text("Type: Text Bank (Conker L&R)");
		break;
	case V31_Text:
		ImGui::Text("Type: Text Bank");
		break;
	case V31_PDZPackage:
		ImGui::Text("Type: Perfect Dark Zero Package");
		break;
	case V31_KameoDB:
		ImGui::Text("Type: Kameo Database");
		break;
	}

	int idData = 0;

	ImGui::Text("Export Available Sections:");

	ImGui::PushID("export");
	for (int i = 0; i < bundleFile.V31Bundle->header.numSectionTypes; i++) {
		ImGui::PushID(i);
		ImGui::SameLine();
		if (ImGui::Button(bundleFile.V31Bundle->sectionEntries[i].sectionName)) {
			char* activeSect = bundleFile.V31Bundle->getSectionData(i);

			writeDataToFile("export", bundleFile.V31Bundle->sectionEntries[i].sectionName, activeSect, bundleFile.V31Bundle->sectionEntries[i].uncompressedSize);
		}
		ImGui::PopID();
	}
	ImGui::PopID();

	if (bundleFile.V31Bundle->type == V31_Unknown) {
		return;
	}


	char filename[256];
	char* end = strrchr(currentFileName, '\\');
	int strLen = strlen(currentFileName);
	int remainLeft = strLen - (end - currentFileName);

	strncpy(filename, end + 1, remainLeft);

	switch (bundleFile.V31Bundle->type) {
	case V31_Texture:
		if (fileId != -1) {
			ImGui::SeparatorText("File Options");
			// If we've loaded up a new file, refresh the display.
			if (activeConkerTex.refresh) {
				char* dataSect = (char*)malloc(bundleFile.V31Bundle->sectionEntries[0].uncompressedSize);
				memcpy(dataSect, (bundleFile.V31Bundle->bundleData + bundleFile.V31Bundle->header.headerSize) + 0x20, bundleFile.V31Bundle->sectionEntries[0].uncompressedSize - 0x20);

				activeConkerTex.ParseTextureHeader(dataSect);

				free(dataSect);

				char* gpuSect = (char*)malloc(bundleFile.V31Bundle->sectionEntries[1].uncompressedSize);
				memcpy(gpuSect, bundleFile.V31Bundle->bundleData + bundleFile.V31Bundle->header.headerSize + bundleFile.V31Bundle->getOffsetOfSection(1), bundleFile.V31Bundle->sectionEntries[1].uncompressedSize);

				ReadConkerLiveReloadedTexture(gpuSect);

				activeConkerTex.refresh = false;

				free(gpuSect);
			}

			if (ImGui::Button("Export Texture")) {
				char* outPath;
				char filePath[2048];

				if (NFD_PickFolderU8(&outPath, "fileName") == NFD_OKAY) {
					memset(filePath, 0, 2048);

					sprintf(filePath, "%s\\exported_image.png", outPath);

					char* texData = (char*)malloc(bundleFile.V31Bundle->sectionEntries[1].uncompressedSize);
					memcpy(texData, bundleFile.V31Bundle->bundleData + bundleFile.V31Bundle->header.headerSize + bundleFile.V31Bundle->getOffsetOfSection(1), bundleFile.V31Bundle->sectionEntries[1].uncompressedSize);

					unsigned char* imgData = GetRawImageData_Base(texData, activeConkerTex.header.width, activeConkerTex.header.height, activeConkerTex.header.format);

					if (activeConkerTex.header.format == XboxTexFormat::BGRA8888) {
						stbi__bgra_to_rgba(imgData, activeConkerTex.header.width, activeConkerTex.header.height, 4);
					}

					int success = stbi_write_png(filePath, activeConkerTex.header.width, activeConkerTex.header.height, 4, imgData, activeConkerTex.header.width * 4);

					if (success != 0) {
						printf("Image has been successfully exported.\n");
					}
					else {
						printf("An error occured while trying to export the image.\n");
					}

					free(texData);
					free(imgData);
				}

			}

			ImGui::SeparatorText("Texture Info");
			ImGui::Text("Format: 0x%02X (%s)", activeConkerTex.header.format, GetXboxTextureFormatName(activeConkerTex.header.format));
			ImGui::Text("Width/Height: %d / %d", activeConkerTex.header.width, activeConkerTex.header.height);
			ImGui::SeparatorText("Texture View");

			if (tempTexInt != -1) {
				int width = activeConkerTex.header.width;
				int height = activeConkerTex.header.height;

				if (activeConkerTex.header.width > 128 || activeConkerTex.header.height > 128) {
					width = activeConkerTex.header.width / 2;
					height = activeConkerTex.header.height / 2;
				}

				if (activeConkerTex.header.width > 256 || activeConkerTex.header.height > 256) {
					width = activeConkerTex.header.width / 4;
					height = activeConkerTex.header.height / 4;
				}
				ImGui::Image(tempTexInt, ImVec2(width, height));
			}
		}
		break;
	case V31_ConkerText:
	case V31_Text:
		if (fileId != -1) {
			ImGui::SeparatorText("File Options");
			if (ImGui::Button("Load Localization Text")) {
				char* activeSect = bundleFile.V31Bundle->getSectionData(0);

				SetupLoadingPromptWidget("Currently loading the loctext file. Please wait.");
				std::thread(&readLoctextFile, activeSect, filename).detach();
				AssignLoctextFilename(filename);
				getLoctextWindowParams()->activeLoctext->startEndianness = bundleFile.V31Bundle->header.byteswapFlags;
			}
		}
		break;
	case V31_PDZPackage:
		if (fileId != -1) {
			ImGui::SeparatorText("File Options");
			char lbl[1024];
			char* ptr = strrchr(bundleFile.V31Bundle->fileInfoTable.debugTable.fileNames[fileId], '\\');
			if (ptr != NULL) {
				strcpy(lbl, ptr + 1);
			}
			else {
				strcpy(lbl, bundleFile.V31Bundle->fileInfoTable.debugTable.fileNames[fileId]);
			}

			float pos = ImGui::GetCursorPosY();

			ImGui::Text("Filename: ");
			ImGui::SameLine(0, -0.25f);
			ImGui::SetCursorPosY(pos - 3);
			ImGui::InputText("##xxlbl", lbl, ImGuiInputTextFlags_ReadOnly);

			if (ptr != NULL) {
				char path[1024];
				int len = ptr - bundleFile.V31Bundle->fileInfoTable.debugTable.fileNames[fileId];
				memset(path, 0, 1024);

				strncpy(path, bundleFile.V31Bundle->fileInfoTable.debugTable.fileNames[fileId], len);

				pos = ImGui::GetCursorPosY();

				ImGui::SetCursorPosY(pos + 3);
				ImGui::Text("Filepath: ");
				ImGui::SameLine(0, -0.25f);
				ImGui::SetCursorPosY(pos);
				ImGui::InputText("##xxpath", path, ImGuiInputTextFlags_ReadOnly);
			}

			ImGui::SeparatorText("Export Options");
			if (fileId == 0) {
				ImGui::Text("Export Dark Package Contents:");

				if (ImGui::Button("Export")) {
					int fileIDX = -1;

					char* outPath;

					if (NFD_PickFolderU8(&outPath, "fileName") == NFD_OKAY) {
						char lbl[1024];
						char type[32];

						SetupLoadingBarPromptWidget("Exporting Dark Package Contents...", activeDarkPackageFile.header.numOfFiles);

						if ((fileIDX = bundleFile.V31Bundle->GetMatchingFileInfoIdx(fileIdx, 1)), fileIDX != -1) {
							char* activeSect = bundleFile.V31Bundle->getFileData(NULL, fileIDX);

							for (int i = 0; i < activeDarkPackageFile.header.numOfFiles; i++) {
								int size = 0;
								if (activeDarkPackageFile.fileTable.entries[i].dataOffset != 0 && i + 1 >= activeDarkPackageFile.header.numOfFiles) {
									size = bundleFile.V31Bundle->fileInfoTable.fileInfoEntries[fileIDX].dataSize - activeDarkPackageFile.fileTable.entries[i].dataOffset;
								}
								if (activeDarkPackageFile.fileTable.entries[i].dataOffset != 0 && i + 1 < activeDarkPackageFile.header.numOfFiles) {
									size = activeDarkPackageFile.fileTable.entries[i + 1].nameOffset - activeDarkPackageFile.fileTable.entries[i].dataOffset;
								}

								char* buf = new char[size];

								memcpy(buf, activeSect + activeDarkPackageFile.fileTable.entries[i].dataOffset, size);

								char* nameBuffer = new char[MAX_PATH];

								//activeDarkPackageFile.fileTable.fileNames[i]

								sprintf(nameBuffer, "%s\\", outPath);

								char* charBuffer = new char[1024];
								strcpy(charBuffer, activeDarkPackageFile.fileTable.fileNames[i]);

								char* tok = strtok(charBuffer, "_");

								while (tok != NULL) {
									strcat(nameBuffer, tok);
									std::filesystem::create_directory(nameBuffer);

									strcat(nameBuffer, "\\");
									tok = strtok(NULL, "_");
								}

								strcpy(charBuffer, activeDarkPackageFile.fileTable.fileNames[i]);
								char* xmlPtr = strtok(charBuffer, "_");
								xmlPtr = strtok(NULL, "_");

								sprintf(nameBuffer, "%sdefault.%s.bin", nameBuffer, xmlPtr);

								FILE* writeFile = fopen(nameBuffer, "wb");

								if (writeFile == NULL) {
									printf("An error occured while creating the output file (%s).\n", buf);
									return;
								}

								size_t written = fwrite(buf, sizeof(char), size, writeFile);

								if (written != size) {
									printf("An error occured while writing data to output file.\n");
								}

								int flush = fflush(writeFile);

								if (flush != 0) {
									printf("An error occured while flushing data to output file.\n");
								}

								fclose(writeFile);

								delete[] charBuffer;
								delete[] nameBuffer;
								delete[] buf;
							}

							free(activeSect);
						}
					}

					CloseLoadingBarPromptWidget();
				}
			}

			ImGui::Text("Export Full File:");
			ImGui::SameLine();

			if (ImGui::Button("Export All Sections")) {
				int fullFileSize = bundleFile.V31Bundle->GetTotalSizeOfContainedFile(fileId + 1);
				char* fullSect = (char*)malloc(fullFileSize);

				int activeOffset = 0;
				for (int i = 0; i < bundleFile.V31Bundle->header.numSectionTypes; i++) {
					int idx = bundleFile.V31Bundle->GetMatchingFileInfoIdx(fileId + 1, i + 1);

					if (idx == -1) break;

					char* activeSect = bundleFile.V31Bundle->getFileData(NULL, idx);
					memcpy(fullSect + activeOffset, activeSect, bundleFile.V31Bundle->fileInfoTable.fileInfoEntries[idx].dataSize);
					activeOffset += bundleFile.V31Bundle->fileInfoTable.fileInfoEntries[idx].dataSize;

					free(activeSect);
				}

				writeDataToFile(lbl, "", fullSect, fullFileSize);

				free(fullSect);
			}

			pos = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(pos - 5);
			ImGui::Text("or");

			ImGui::Text("Export Individual Sections:");

			for (int i = 0; i < bundleFile.V31Bundle->header.numSectionTypes; i++) {
				int fileIDX = -1;
				if ((fileIDX = bundleFile.V31Bundle->GetMatchingFileInfoIdx(fileIdx, i + 1)), fileIDX == -1) {
					continue;
				}

				ImGui::PushID(i);
				ImGui::SameLine();

				if (ImGui::Button(bundleFile.V31Bundle->sectionEntries[i].sectionName)) {
					char* activeSect = bundleFile.V31Bundle->getFileData(NULL, fileIDX);

					writeDataToFile(lbl, bundleFile.V31Bundle->sectionEntries[i].sectionName, activeSect, bundleFile.V31Bundle->fileInfoTable.fileInfoEntries[fileIDX].dataSize);
				}

				ImGui::PopID();
			}
		}
		break;
	case V31_KameoDB:
		if (fileId != -1) {
			ImGui::SeparatorText("File Options");
			char lbl[1024];
			char lblpath[1024];
			char* ptr = strrchr(activeKameoDBFile.fileTable.fileNames[fileId], '\\');
			if (ptr != NULL) {
				strcpy(lbl, ptr + 1);

				int offs = ptr - activeKameoDBFile.fileTable.fileNames[fileId];
				strncpy_s(lblpath, 1024, activeKameoDBFile.fileTable.fileNames[fileId], offs);
			}
			else {
				strcpy(lbl, activeKameoDBFile.fileTable.fileNames[fileId]);
			}

			float pos = ImGui::GetCursorPosY();

			ImGui::Text("Filename: ");
			ImGui::SameLine(0, -0.25f);
			ImGui::SetCursorPosY(pos - 3);
			ImGui::InputText("##xxlbl", lbl, ImGuiInputTextFlags_ReadOnly);

			pos = ImGui::GetCursorPosY();

			ImGui::SetCursorPosY(pos + 3);
			ImGui::Text("Aid Hash: ");
			ImGui::SameLine(0, -0.25f);
			ImGui::SetCursorPosY(pos);
			ImGui::InputScalar("##xxhash", ImGuiDataType_U32, &activeKameoDBFile.fileTable.hashEntries[fileId].hash, 0, 0, "%08X", ImGuiInputTextFlags_ReadOnly);
		}
		break;
	}
}

void displayActiveBundleV26Property() {

	ImGui::SeparatorText("Bundle Information");

	switch (bundleFile.V26Bundle->type) {
	case V31_Texture:
		ImGui::Text("Type: Texture");
		break;
	case V31_ConkerText:
		ImGui::Text("Type: Text Bank (Conker L&R)");
		break;
	case V31_Text:
		ImGui::Text("Type: Text Bank");
		break;
	case V31_PDZPackage:
		ImGui::Text("Type: Perfect Dark Zero Package");
		break;
	case V31_KameoDB:
		ImGui::Text("Type: Kameo Database");
		break;
	}

	int idData = 0;

	ImGui::Text("Export Available Sections:");

	ImGui::PushID("export");
	for (int i = 0; i < bundleFile.V26Bundle->header.numSectionTypes; i++) {
		ImGui::PushID(i);
		ImGui::SameLine();
		if (ImGui::Button(bundleFile.V26Bundle->sectionEntries[i].sectionName)) {
			char* activeSect = bundleFile.V26Bundle->getSectionData(i);

			writeDataToFile("export", bundleFile.V26Bundle->sectionEntries[i].sectionName, activeSect, bundleFile.V26Bundle->sectionEntries[i].uncompressedSize);
		}
		ImGui::PopID();
	}
	ImGui::PopID();

	char filename[256];
	char* end = strrchr(currentFileName, '\\');
	int strLen = strlen(currentFileName);
	int remainLeft = strLen - (end - currentFileName);

	strncpy(filename, end + 1, remainLeft);

	if (fileId != -1) {
		ImGui::SeparatorText("File Options");

		char lbl[1024];
		char* ptr = strrchr(bundleFile.V26Bundle->fileInfoTable.debugTable.fileNames[fileId], '\\');
		if (ptr != NULL) {
			strcpy(lbl, ptr + 1);
		}
		else {
			strcpy(lbl, bundleFile.V26Bundle->fileInfoTable.debugTable.fileNames[fileId]);
		}

		float pos = ImGui::GetCursorPosY();

		ImGui::Text("Filename: ");
		ImGui::SameLine(0, -0.25f);
		ImGui::SetCursorPosY(pos - 3);
		ImGui::InputText("##xxlbl", lbl, ImGuiInputTextFlags_ReadOnly);

		ImGui::Text("Export Available Sections:");

		for (int i = 0; i < bundleFile.V26Bundle->header.numSectionTypes; i++) {
			int fileIDX = -1;
			if ((fileIDX = bundleFile.V26Bundle->GetMatchingFileInfoIdx(fileIdx, i + 1)), fileIDX == -1) {
				continue;
			}

			ImGui::PushID(i);
			ImGui::SameLine();

			if (ImGui::Button(bundleFile.V26Bundle->sectionEntries[i].sectionName)) {
				char* activeSect = bundleFile.V26Bundle->getFileData(NULL, fileIDX);

				writeDataToFile(lbl, bundleFile.V26Bundle->sectionEntries[i].sectionName, activeSect, bundleFile.V26Bundle->fileInfoTable.fileInfoEntries[fileIDX].dataSize);
			}
			ImGui::PopID();
		}
	}

	switch (bundleFile.V26Bundle->type) {
	case V31_Texture:

		// If we've loaded up a new file, refresh the display.
		if (activeConkerTex.refresh) {
			char* dataSect = (char*)malloc(bundleFile.V26Bundle->sectionEntries[0].uncompressedSize);
			memcpy(dataSect, (bundleFile.V26Bundle->bundleData + bundleFile.V26Bundle->header.headerSize) + 0x20, bundleFile.V26Bundle->sectionEntries[0].uncompressedSize - 0x20);

			activeConkerTex.ParseTextureHeader(dataSect);

			free(dataSect);

			char* gpuSect = (char*)malloc(bundleFile.V26Bundle->sectionEntries[1].uncompressedSize);
			memcpy(gpuSect, bundleFile.V26Bundle->bundleData + bundleFile.V26Bundle->header.headerSize + bundleFile.V26Bundle->getOffsetOfSection(1), bundleFile.V26Bundle->sectionEntries[1].uncompressedSize);

			ReadConkerLiveReloadedTexture(gpuSect);

			activeConkerTex.refresh = false;

			free(gpuSect);
		}

		if (ImGui::Button("Export Texture")) {
			char* outPath;
			char filePath[2048];

			if (NFD_PickFolderU8(&outPath, "fileName") == NFD_OKAY) {
				memset(filePath, 0, 2048);

				sprintf(filePath, "%s\\exported_image.png", outPath);

				char* texData = (char*)malloc(bundleFile.V26Bundle->sectionEntries[1].uncompressedSize);
				memcpy(texData, bundleFile.V26Bundle->bundleData + bundleFile.V26Bundle->header.headerSize + bundleFile.V26Bundle->getOffsetOfSection(1), bundleFile.V26Bundle->sectionEntries[1].uncompressedSize);

				unsigned char* imgData = GetRawImageData_Base(texData, activeConkerTex.header.width, activeConkerTex.header.height, activeConkerTex.header.format);

				if (activeConkerTex.header.format == XboxTexFormat::BGRA8888) {
					stbi__bgra_to_rgba(imgData, activeConkerTex.header.width, activeConkerTex.header.height, 4);
				}

				int success = stbi_write_png(filePath, activeConkerTex.header.width, activeConkerTex.header.height, 4, imgData, activeConkerTex.header.width * 4);

				if (success != 0) {
					printf("Image has been successfully exported.\n");
				}
				else {
					printf("An error occured while trying to export the image.\n");
				}

				free(texData);
				free(imgData);
			}

		}

		ImGui::SeparatorText("Texture Info");
		ImGui::Text("Format: 0x%02X (%s)", activeConkerTex.header.format, GetXboxTextureFormatName(activeConkerTex.header.format));
		ImGui::Text("Width/Height: %d / %d", activeConkerTex.header.width, activeConkerTex.header.height);
		ImGui::SeparatorText("Texture View");

		if (tempTexInt != -1) {
			int width = activeConkerTex.header.width;
			int height = activeConkerTex.header.height;

			if (activeConkerTex.header.width > 128 || activeConkerTex.header.height > 128) {
				width = activeConkerTex.header.width / 2;
				height = activeConkerTex.header.height / 2;
			}

			if (activeConkerTex.header.width > 256 || activeConkerTex.header.height > 256) {
				width = activeConkerTex.header.width / 4;
				height = activeConkerTex.header.height / 4;
			}
			ImGui::Image(tempTexInt, ImVec2(width, height));
		}
		break;
	case V31_ConkerText:
	case V31_Text:
		if (ImGui::Button("Load Localization Text")) {
			char* activeSect = bundleFile.V26Bundle->getSectionData(0);

			SetupLoadingPromptWidget("Currently loading the loctext file. Please wait.");
			std::thread(&readLoctextFile, activeSect, filename).detach();
		}
		break;
	case V31_PDZPackage:
		break;
	case V31_KameoDB:
		if (fileId != -1) {
			char lbl[1024];
			char lblpath[1024];
			char* ptr = strrchr(activeKameoDBFile.fileTable.fileNames[fileId], '\\');
			if (ptr != NULL) {
				strcpy(lbl, ptr + 1);

				int offs = ptr - activeKameoDBFile.fileTable.fileNames[fileId];
				strncpy_s(lblpath, 1024, activeKameoDBFile.fileTable.fileNames[fileId], offs);
			}
			else {
				strcpy(lbl, activeKameoDBFile.fileTable.fileNames[fileId]);
			}

			float pos = ImGui::GetCursorPosY();

			ImGui::Text("Filename: ");
			ImGui::SameLine(0, -0.25f);
			ImGui::SetCursorPosY(pos - 3);
			ImGui::InputText("##xxlbl", lbl, ImGuiInputTextFlags_ReadOnly);

			pos = ImGui::GetCursorPosY();

			ImGui::SetCursorPosY(pos + 3);
			ImGui::Text("Filepath: ");
			ImGui::SameLine(0, -0.25f);
			ImGui::SetCursorPosY(pos);
			ImGui::InputText("##xxlblpath", lblpath, ImGuiInputTextFlags_ReadOnly);

			pos = ImGui::GetCursorPosY();

			ImGui::SetCursorPosY(pos + 3);
			ImGui::Text("Aid Hash: ");
			ImGui::SameLine(0, -0.25f);
			ImGui::SetCursorPosY(pos);
			ImGui::InputScalar("##xxhash", ImGuiDataType_U32, &activeKameoDBFile.fileTable.hashEntries[fileId].hash, 0, 0, "%08X", ImGuiInputTextFlags_ReadOnly);
		}
		break;
	}
}

void displayActiveGhoulDemandProperty() {

	ImGui::Text("Asset Type: %02x (%s)", ghoulDemandFile.type, ghoulies_AssetArray[ghoulDemandFile.type]);

	ImGui::SeparatorText("File Options");

	int idData = 0;

	ImGui::Text("Export Available Sections:");
	ImGui::SameLine();

	ImGui::PushID("export");

	ImGui::PushID("data");
	if (ImGui::Button(".data")) {
		char* activeSect = ghoulDemandFile.GetDataSection();

		char file[1024];
		memset(file, 0, 1024);

		strcat(file, "export.data");

		writeDataToFile("export", ".data", activeSect, ghoulDemandFile.dataSectSize);
	}
	ImGui::PopID();

	if (ghoulDemandFile.gpuSectSize != 0) {
		ImGui::SameLine();
		ImGui::PushID("gpu");
		if (ImGui::Button(".gpu")) {
			char* activeSect = ghoulDemandFile.GetGpuSection();

			char file[1024];
			memset(file, 0, 1024);

			strcat(file, "export.gpu");

			writeDataToFile("export", ".gpu", activeSect, ghoulDemandFile.gpuSectSize);
		}
		ImGui::PopID();
	}

	ImGui::PopID();

	switch (ghoulDemandFile.type)
	{
	case ghoulDB_Texture:
		//Failsafe
		if (activeGhoulTex == nullptr) {
			activeGhoulTex = new TextureFile();
		}

		// If we've loaded up a new file, refresh the display.
		if (activeGhoulTex->refresh) {
			char* dataSect = (char*)malloc(ghoulDemandFile.dataSectSize);
			memcpy(dataSect, ghoulDemandFile.dataPtr + ghoulDemandFile.dataSectOffset, ghoulDemandFile.dataSectSize);

			activeGhoulTex->ParseTextureHeader(dataSect);

			char* gpuSect = (char*)malloc(16);
			memcpy(gpuSect, ghoulDemandFile.dataPtr + ghoulDemandFile.gpuSectOffset, 16);

			activeGhoulTex->ParseTextureEntries(gpuSect);

			free(dataSect);
			free(gpuSect);

			ReadGhoulDemandTexture();

			activeGhoulTex->refresh = false;
		}

		if (ImGui::Button("Export Current Frame")) {
			char* outPath;
			char filePath[2048];

			if (NFD_PickFolderU8(&outPath, "fileName") == NFD_OKAY) {
				memset(filePath, 0, 2048);

				sprintf(filePath, "%s\\exported_image.png", outPath);

				char* texData = (char*)malloc(activeGhoulTex->header.chunkSize);
				memcpy(texData, ghoulDemandFile.dataPtr + ghoulDemandFile.gpuSectOffset + activeGhoulTex->gpuHeader.offset + (activeGhoulTex->header.chunkSize * activeGhoulTex->framePos), activeGhoulTex->header.chunkSize);

				unsigned char* imgData = GetRawImageData_Base(texData, activeGhoulTex->header.width, activeGhoulTex->header.height, activeGhoulTex->header.format);

				if (activeGhoulTex->header.format == XboxTexFormat::BGRA8888) {
					stbi__bgra_to_rgba(imgData, activeGhoulTex->header.width, activeGhoulTex->header.height, 4);
				}

				int success = stbi_write_png(filePath, activeGhoulTex->header.width, activeGhoulTex->header.height, 4, imgData, activeGhoulTex->header.width * 4);

				if (success != 0) {
					printf("Image has been successfully exported.\n");
				}
				else {
					printf("An error occured while trying to export the image.\n");
				}

				free(texData);
				free(imgData);
			}

		}

		if (activeGhoulTex->header.frameCount != 1) {
			ImGui::SameLine();
			ImGui::Text("OR");
			ImGui::SameLine();
			if (ImGui::Button("Export All Frames")) {
				char* outPath;
				char filePath[2048];

				if (NFD_PickFolderU8(&outPath, "fileName") == NFD_OKAY) {
					for (int i = 0; i < activeGhoulTex->header.frameCount; i++) {
						memset(filePath, 0, 128);
						sprintf(filePath, "%s/exported_image_%03d.png", outPath, i);

						char* texData = (char*)malloc(activeGhoulTex->header.chunkSize);
						memcpy(texData, ghoulDemandFile.dataPtr + ghoulDemandFile.gpuSectOffset + activeGhoulTex->gpuHeader.offset + (activeGhoulTex->header.chunkSize * i), activeGhoulTex->header.chunkSize);

						unsigned char* imgData = GetRawImageData_Base(texData, activeGhoulTex->header.width, activeGhoulTex->header.height, activeGhoulTex->header.format);

						if (activeGhoulTex->header.format == XboxTexFormat::BGRA8888) {
							stbi__bgra_to_rgba(imgData, activeGhoulTex->header.width, activeGhoulTex->header.height, 4);
						}

						int success = stbi_write_png(filePath, activeGhoulTex->header.width, activeGhoulTex->header.height, 4, imgData, activeGhoulTex->header.width * 4);

						if (success != 0) {
							printf("Image has been successfully exported.\n");
						}
						else {
							printf("An error occured while trying to export the image.\n");
						}

						free(texData);
						free(imgData);
					}
				}
			}
		}

		ImGui::SeparatorText("Texture Info");
		ImGui::Text("Format: %d", activeGhoulTex->header.format);
		ImGui::Text("Format: %d | W/H : %d / %d | Chunk Size: %d", activeGhoulTex->header.format, activeGhoulTex->header.width, activeGhoulTex->header.height, activeGhoulTex->header.chunkSize);
		ImGui::Text("Frames Info: %d - %d FPS", activeGhoulTex->header.frameCount, activeGhoulTex->header.framerate);
		ImGui::SeparatorText("Texture View");

		if (activeGhoulTex->header.frameCount != 1) {
			if (ImGui::ArrowButton("texFrameDown", ImGuiDir_Left)) {
				if (activeGhoulTex->framePos > 0) {
					activeGhoulTex->framePos = activeGhoulTex->framePos - 1;
				}
				else {
					activeGhoulTex->framePos = activeGhoulTex->header.frameCount - 1;
				}

				ReadGhoulDemandTexture();
			}
		}

		if (activeGhoulTex->header.frameCount != 1) {
			ImGui::SameLine();
			ImGui::Text("Current Frame: %03d", activeGhoulTex->framePos + 1);
			ImGui::SameLine();
		}

		if (activeGhoulTex->header.frameCount != 1) {
			if (ImGui::ArrowButton("texFrameUp", ImGuiDir_Right)) {
				if (activeGhoulTex->framePos < activeGhoulTex->header.frameCount - 1) {
					activeGhoulTex->framePos = activeGhoulTex->framePos + 1;
				}
				else {
					activeGhoulTex->framePos = 0;
				}

				ReadGhoulDemandTexture();
			}
		}

		if (tempTexInt != -1) {
			int width = activeGhoulTex->header.width;
			int height = activeGhoulTex->header.height;

			if (activeGhoulTex->header.width > 128 || activeGhoulTex->header.height > 128) {
				width = width / 4;
				height = height / 4;
			}
			ImGui::Image(tempTexInt, ImVec2(width, height));
		}
		break;
	case ghoulDB_Loctext:
		if (ImGui::Button("Load Localization Text")) {
			char* dataSect = (char*)malloc(ghoulDemandFile.dataSectSize);
			memcpy(dataSect, ghoulDemandFile.dataPtr + ghoulDemandFile.dataSectOffset, ghoulDemandFile.dataSectSize);

			SetupLoadingPromptWidget("Currently loading the loctext file. Please wait.");
			std::thread(&readLoctextFile, dataSect, currentFileName).detach();
			getLoctextWindowParams()->activeLoctext->startEndianness = 0;
		}
		break;
	}
}

void displayActiveGhoulBundleProperty() {
	if (fileId == -1) return;

	char lbl[1024];
	char type[32];

	ImGui::SeparatorText("General File Properties");

	memset(lbl, 0, 1024);
	memset(type, 0, 32);

	char* ptr = strstr(ghoulBundleFile.fileEntries[fileId].fileName, "aid_");
	if (ptr != NULL) {
		strcpy(lbl, ptr);
		assetGetTypeFromString(lbl + 4, type);
	}
	else {
		strcpy(lbl, ghoulBundleFile.fileEntries[fileId].fileName);
	}

	ImGui::Text("Filename: %s", lbl);
	ImGui::Text("Asset Type: %d (%s)", ghoulBundleFile.fileEntries[fileId].type, type);

	std::time_t stamp = ghoulBundleFile.fileEntries[fileId].timestamp;
	//std::time_t time = std::time(&stamp);
	ImGui::Text("Timestamp: %d -> %s", ghoulBundleFile.fileEntries[fileId].timestamp, ctime(&stamp));

	//if (suffix != NULL) {
	//	ImGui::Text("Suffix: %s", suffix);
	//}


	ImGui::Text("Sections: ");
	ImGui::SameLine();

	ImGui::Text(".data");

	if (ghoulBundleFile.fileEntries[fileId].gpuSectSize != 0) {
		ImGui::SameLine();
		ImGui::Text(".gpu");
	}

	ImGui::SeparatorText("File Options");

	ImGui::PushID("export");
	ImGui::Text("Export from Available Sections:");
	ImGui::SameLine();

	ImGui::PushID("data");
	if (ImGui::Button(".data")) {
		char* activeSect = ghoulBundleFile.GetFileData(fileId);

		writeDataToFile(lbl, ".data", activeSect, ghoulBundleFile.fileEntries[fileId].dataSectSize);
	}
	ImGui::PopID();

	// Check if we actually have any GPU data for the selected item. (Usually just textures)
	if (ghoulBundleFile.fileEntries[fileId].gpuSectSize != 0) {
		ImGui::SameLine();

		ImGui::PushID("gpu");
		if (ImGui::Button(".gpu")) {
			char* gpu = ghoulBundleFile.GetFileGPU(fileId);

			printf("Exporting GPU for file %s. Offset %d with size %d.\n", ghoulBundleFile.fileEntries[fileId].fileName, ghoulBundleFile.fileEntries[fileId].gpuSectOffset, ghoulBundleFile.fileEntries[fileId].gpuSectSize);
			writeDataToFile(lbl, ".gpu", gpu, ghoulBundleFile.fileEntries[fileId].gpuSectSize);

			free(gpu);
		}
		ImGui::PopID();
	}

	ImGui::PopID();

	switch (ghoulBundleFile.fileEntries[fileId].type)
	{
	case 1:
		//Failsafe
		if (activeGhoulTex == nullptr) {
			activeGhoulTex = new TextureFile();
		}

		// If we've loaded up a new file, refresh the display.
		if (activeGhoulTex->refresh) {
			ReadGhoulBundleTexture();
			activeGhoulTex->refresh = false;
		}

		if (ImGui::Button("Export Current Frame")) {
			char* outPath;
			char filePath[2048];

			if (NFD_PickFolderU8(&outPath, "fileName") == NFD_OKAY) {
				memset(filePath, 0, 2048);

				sprintf(filePath, "%s\\%s.png", outPath, lbl);

				char* dataSect = ghoulBundleFile.GetFileData(fileId);

				activeGhoulTex->ParseTextureHeader(dataSect);

				free(dataSect);

				char* texData = (char*)malloc(activeGhoulTex->header.chunkSize);

				char* gpuSect = ghoulBundleFile.GetFileGPUForTexture(fileId);

				memcpy(texData, gpuSect + (activeGhoulTex->header.chunkSize * activeGhoulTex->framePos), activeGhoulTex->header.chunkSize);

				unsigned char* imgData = GetRawImageData_Base(texData, activeGhoulTex->header.width, activeGhoulTex->header.height, activeGhoulTex->header.format);

				if (activeGhoulTex->header.format == XboxTexFormat::BGRA8888) {
					stbi__bgra_to_rgba(imgData, activeGhoulTex->header.width, activeGhoulTex->header.height, 4);
				}

				int success = stbi_write_png(filePath, activeGhoulTex->header.width, activeGhoulTex->header.height, 4, imgData, activeGhoulTex->header.width * 4);

				if (success != 0) {
					printf("Image has been successfully exported.\n");
				}
				else {
					printf("An error occured while trying to export the image.\n");
				}

				free(gpuSect);
				free(texData);
				free(imgData);
			}

		}

		if (activeGhoulTex->header.frameCount != 1) {
			ImGui::SameLine();
			ImGui::Text("OR");
			ImGui::SameLine();
			if (ImGui::Button("Export All Frames")) {
				char* outPath;
				char filePath[2048];

				if (NFD_PickFolderU8(&outPath, "fileName") == NFD_OKAY) {
					for (int i = 0; i < activeGhoulTex->header.frameCount; i++) {
						memset(filePath, 0, 128);
						sprintf(filePath, "%s\\%s_%03d.png", outPath, lbl, i);

						char* dataSect = ghoulBundleFile.GetFileData(fileId);

						activeGhoulTex->ParseTextureHeader(dataSect);

						free(dataSect);

						char* texData = (char*)malloc(activeGhoulTex->header.chunkSize);

						char* gpuSect = ghoulBundleFile.GetFileGPUForTexture(fileId);

						memcpy(texData, gpuSect + (activeGhoulTex->header.chunkSize * i), activeGhoulTex->header.chunkSize);

						unsigned char* imgData = GetRawImageData_Base(texData, activeGhoulTex->header.width, activeGhoulTex->header.height, activeGhoulTex->header.format);

						if (activeGhoulTex->header.format == XboxTexFormat::BGRA8888) {
							stbi__bgra_to_rgba(imgData, activeGhoulTex->header.width, activeGhoulTex->header.height, 4);
						}

						int success = stbi_write_png(filePath, activeGhoulTex->header.width, activeGhoulTex->header.height, 4, imgData, activeGhoulTex->header.width * 4);

						if (success != 0) {
							printf("Image has been successfully exported.\n");
						}
						else {
							printf("An error occured while trying to export the image.\n");
						}

						free(gpuSect);
						free(texData);
						free(imgData);
					}
				}
			}
		}

		ImGui::SeparatorText("Texture Info");
		ImGui::Text("Format: %d | W/H : %d / %d | Chunk Size: %d", activeGhoulTex->header.format, activeGhoulTex->header.width, activeGhoulTex->header.height, activeGhoulTex->header.chunkSize);
		ImGui::Text("Frames Info: %d - %d FPS", activeGhoulTex->header.frameCount, activeGhoulTex->header.framerate);
		ImGui::SeparatorText("Texture Preview");

		if (activeGhoulTex->header.frameCount != 1) {
			if (ImGui::ArrowButton("texFrameDown", ImGuiDir_Left)) {
				if (activeGhoulTex->framePos > 0) {
					activeGhoulTex->framePos = activeGhoulTex->framePos - 1;
				}
				else {
					activeGhoulTex->framePos = activeGhoulTex->header.frameCount - 1;
				}

				ReadGhoulBundleTexture();
			}
		}

		if (activeGhoulTex->header.frameCount != 1) {
			ImGui::SameLine();
			ImGui::Text("Current Frame: %03d", activeGhoulTex->framePos + 1);
			ImGui::SameLine();
		}

		if (activeGhoulTex->header.frameCount != 1) {
			if (ImGui::ArrowButton("texFrameUp", ImGuiDir_Right)) {
				if (activeGhoulTex->framePos < activeGhoulTex->header.frameCount - 1) {
					activeGhoulTex->framePos = activeGhoulTex->framePos + 1;
				}
				else {
					activeGhoulTex->framePos = 0;
				}

				ReadGhoulBundleTexture();
			}
		}

		if (tempTexInt != -1) {
			int width = activeGhoulTex->header.width;
			int height = activeGhoulTex->header.height;

			if (activeGhoulTex->header.width > 128 || activeGhoulTex->header.height > 128) {
				width = width / 4;
				height = height / 4;
			}
			ImGui::Image(tempTexInt, ImVec2(width, height));
		}
		break;
	}
}

void displayActiveRPKFileProperty() {
	if (fileId == -1) return;

	char lbl[1024];
	char type[32];

	ImGui::SeparatorText("General File Properties");

	memset(lbl, 0, 1024);
	memset(type, 0, 32);

	char* ptr = strrchr(rpkFile->fileEntries[fileId].fileName, '/');
	if (ptr != NULL) {
		strcpy(lbl, ptr + 1);
	}
	else {
		strcpy(lbl, rpkFile->fileEntries[fileId].fileName);
	}

	ImGui::Text("Filename:\t%s", lbl);
	ImGui::Text("File Offset in RPK:\t%d", rpkFile->fileEntries[fileId].dataOffs);
	ImGui::Text("File Size:\t%d", rpkFile->fileEntries[fileId].dataSize);
	ImGui::Text("UNK VAL 1:\t%d", rpkFile->fileEntries[fileId].unk2);
	ImGui::Text("UNK VAL 2:\t%d", rpkFile->fileEntries[fileId].unk4);
	//ImGui::Text("Asset Type: %d (%s)", ghoulBundleFile.fileEntries[fileId].type, type);

	if (rpkFile->fileEntries[fileId].dataSize != 0) {
		ImGui::SeparatorText("File Options");

		ImGui::PushID("export");
		if (ImGui::Button("Export File")) {
			char* activeSect = rpkFile->getFileData(fileId);

			writeDataToFile(lbl, "", activeSect, rpkFile->fileEntries[fileId].dataSize);
		}
		ImGui::PopID();
	}
}

void displayActiveStreamBundleFileProperty() {
	if (imGuiWindowInfo.streamBundleSelectedItem == -1) return;

	StreamFileEntry* fileEntry = &streamBundleFile->fileEntries[imGuiWindowInfo.streamBundleSelectedItem];
	StreamEntry* entry = &streamBundleFile->bundleFiles[imGuiWindowInfo.streamBundleSelectedItem];

	ImGui::SeparatorText("Streamed File Properties");
	ImGui::Text("Total Bundle Size: %d", fileEntry->dataSize);
	ImGui::Text("Aid Hash: %08x", fileEntry->aid);

	if (entry->entryType == ENTRY_BUNDLE) {
		if (entry->bundleFile->sectionTable.adbStringLen != 0) {
			char lbl[1024];

			char* ptr = strstr(entry->bundleFile->sectionTable.adbString, "aid_");

			ImGui::Text("ADB: %s", ptr);
		}
	}

	ImGui::SeparatorText("Streamed File Options");

	if (ImGui::Button("Export Data")) {
		char file[1024];
		memset(file, 0, 1024);

		switch (entry->entryType)
		{
		case ENTRY_DNBW:
			strcat(file, entry->waveBankFile->bankName);
			strcat(file, ".xwb");
			break;
		case ENTRY_BUNDLE:
			sprintf(file, "%08x", fileEntry->aid);
			break;
		default:
			strcat(file, "lbl");
			break;
		}

		char* activeSect = 0;
		activeSect = streamBundleFile->getFileData(imGuiWindowInfo.streamBundleSelectedItem);

		writeDataToFile(file, "", activeSect, streamBundleFile->fileEntries[imGuiWindowInfo.streamBundleSelectedItem].dataSize);
	}

	if (ImGui::Button("Import Data")) {
		char* importPath;

		if (NFD_OpenDialog(&importPath, NULL, 0, "") == NFD_OKAY) {
			BufferedSave savedFile;

			FILE* importedFile = fopen(importPath, "rb");

			fseek(importedFile, 0L, SEEK_END);
			int len = ftell(importedFile);
			fseek(importedFile, 0L, SEEK_SET);

			savedFile.savedData = (char*)malloc(len);

			fread(savedFile.savedData, 1, len, importedFile);
			fclose(importedFile);

			// Overwrite two ints on the DNBW header, as Nuts & Bolts expects them to be 0x2B & 0x2A.
			if (entry->entryType == ENTRY_DNBW) {
				unsigned int maj = flipEndian(0x2B);
				unsigned int min = flipEndian(0x2A);
				memcpy(savedFile.savedData + 4, &maj, 4);
				memcpy(savedFile.savedData + 8, &min, 4);
			}

			savedFile.dataSize = len;
			savedFile.fileId = imGuiWindowInfo.streamBundleSelectedItem;
			savedFile.sect = 0;

			bundleSetup.AddToSaveBuffer(savedFile);
		}
	}

	char lbl[1024];
	int totalFileSize = 0;
	char* tStamp = { 0 };
	char* suffix = { 0 };
	memset(lbl, 0, 1024);

	if (fileIdx != -1) {
		ImGui::SeparatorText("General File Properties");

		if (streamBundleFile->bundleFiles[imGuiWindowInfo.streamBundleSelectedItem].entryType == ENTRY_BUNDLE) {
			BundleV36* bundle = streamBundleFile->bundleFiles[imGuiWindowInfo.streamBundleSelectedItem].bundleFile;
			char* ptr = strstr(bundle->sectionTable.fileLabelTable.fileLabels[fileIdx - 1].label, "aid_");
			if (ptr != NULL) {
				strcpy(lbl, ptr);
			}
			else {
				strcpy(lbl, bundle->sectionTable.fileLabelTable.fileLabels[fileIdx - 1].label);
			}

			if (strchr(lbl, ',') != NULL) {
				strtok(lbl, ",");

				tStamp = strtok(NULL, ",");
				suffix = strtok(NULL, ",");
			}

			ImGui::InputText("Filename", lbl, 1024, ImGuiInputTextFlags_ReadOnly);

			if (activeManifest != nullptr) {
				unsigned int fileHash = activeManifest->GetAidHash(fileId);

				if (fileHash != 0) {
					ImGui::Text("Aid Hash: ");
					ImGui::SameLine();
					ImGui::InputScalar("##xx", ImGuiDataType_U32, &fileHash, 0, 0, "%08X", ImGuiInputTextFlags_ReadOnly);
				}
			}

			if (tStamp != NULL) {
				std::time_t stamp = atol(tStamp);
				//std::time_t time = std::time(&stamp);
				ImGui::Text("Timestamp: %s -> %s", tStamp, ctime(&stamp));
			}

			if (suffix != NULL) {
				ImGui::Text("Suffix: %s", suffix);
			}

			ImGui::Text("Sections: ");
			ImGui::SameLine();

			for (int s = 0; s < bundle->header.numSections; s++) {
				if (bundle->sectionTable.fileInfos[s].ID == fileIdx) {
					totalFileSize = totalFileSize + bundle->sectionTable.fileInfos[s].dataSize;
					char* label = bundle->sectionTable.sectionLabels[bundle->sectionTable.fileInfos[s].section - 1].label;
					ImGui::Text("%s ", label);
					ImGui::SameLine();
				}
			}

			ImGui::NewLine();
			ImGui::Text("Total File Size: %d", totalFileSize);
		}
	}
}

void displayActivePinataDbBundleFileProperty() {
	if (fileId == -1) return;

	int hashIdx = PinataDbBundleFile.precachedEntries[fileId].hashIdx;
	int indexIdx = PinataDbBundleFile.precachedEntries[fileId].indexIdx;

	char lbl[1024];
	char type[32];

	ImGui::SeparatorText("General File Properties");

	memset(lbl, 0, 1024);
	memset(type, 0, 32);

	strcpy(lbl, PinataDbBundleFile.indexFile[indexIdx].filename);


	ImGui::Text("Filename:\t%s", lbl);

	std::time_t stamp = PinataDbBundleFile.indexFile[indexIdx].timestamp;
	//std::time_t time = std::time(&stamp);
	ImGui::Text("Timestamp: %d -> %s", PinataDbBundleFile.indexFile[indexIdx].timestamp, ctime(&stamp));

	ImGui::Text("Version: %.03f", PinataDbBundleFile.indexFile[indexIdx].version);

	ImGui::Text("File Offset:\t%d", PinataDbBundleFile.hashFile.offsetArray[hashIdx]);
	ImGui::Text("File Hash:\t%08X", PinataDbBundleFile.hashFile.hash32_Array[fileId]);

	ImGui::SeparatorText("Streamed File Options");

	if (ImGui::Button("Export Bundle")) {
		char file[1024];
		memset(file, 0, 1024);

		char* activeSect = 0;
		int fileSize = 0;
		activeSect = PinataDbBundleFile.getFileData(hashIdx, &fileSize);

		writeDataToFile(lbl, "", activeSect, fileSize);
	}
}
#pragma endregion

#pragma region File List
void displayAvailableFilesList(float barHeight) {
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	ImGui::SetNextWindowPos(ImVec2(width / 2.5, barHeight));
	ImGui::SetNextWindowSize(ImVec2(width / 1.665, height - barHeight));

	if (ImGui::Begin("Bundle File List", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
		switch (fileType)
		{
		case NONE:
			break;
		case BUNDLEV36:
			fillBundleFileList();
			break;
		case NB_STREAMBUNDLE:
			fillStreamBundleFileList();
			break;
		case BUNDLEV31:
			if (bundleFile.V31Bundle->type == V31_KameoDB) {
				fillKameoDBFileList();
			}

			if (bundleFile.V31Bundle->type != V31_KameoDB) {
				fillBundleV31FileList();
			}
			break;
		case BUNDLEV26:
			fillBundleV26FileList();
			break;
		case GHOUL_BUNDLE:
			fillGhouliesBundleFileList();
			break;
		case PINATA_DBBUNDLE:
			fillPinataDbBundleFileList();
			break;
		case RR_RPK:
			fillRPKFileList();
			break;
		}

		ImGui::End();
	}
}

void fillBundleFileList() {
	// Setup the local parameters needed.
	char lbl[1024];
	char type[32];
	char domain[32];
	char subtype[32];

	ImGui::Text("File List");
	ImGui::Separator();
	for (int i = 0; i < bundleFile.V36Bundle->header.numAssets; i++) {
		ImGui::PushID(i);
		memset(lbl, 0, 1024);
		memset(type, 0, 32);
		memset(domain, 0, 32);
		memset(subtype, 0, 32);

		// Check if the entry label we're reading contains "aid_".
		char* ptr = strstr(bundleFile.V36Bundle->sectionTable.fileLabelTable.fileLabels[i].label, "aid_");
		if (ptr != NULL) {
			// If so, copy the contents from the location of that and fetch the asset type.
			strcpy(lbl, ptr);

			assetGetTypeFromString(lbl + 4, type);

			assetGetTypeFromString(lbl + 4 + (strlen(type) + 1), domain);

			if (strcmp(type, "misc") == 0 || strcmp(type, "objparams") == 0 || strcmp(type, "statetable") == 0) {
				assetGetTypeFromString(lbl + 4 + (strlen(type) + 1) + (strlen(domain) + 1), subtype);
			}
			else {
			}
		}
		else {
			// Else, just copy the contents of the label to the dest.
			strcpy(lbl, bundleFile.V36Bundle->sectionTable.fileLabelTable.fileLabels[i].label);
		}

		if (strlen(imGuiWindowInfo.search) != 0) {
			if (strstr(lbl, imGuiWindowInfo.search) == NULL) {
				ImGui::PopID();
				continue;
			}
		}

		if (imGuiWindowInfo.aidSearch != 0) {
			if (activeManifest) {
				if (activeManifest->GetAidHash(i) != imGuiWindowInfo.aidSearch) {
					ImGui::PopID();
					continue;
				}
			}

			if (!activeManifest) {
				if (bundleFile.V36Bundle->hashTable[i] != imGuiWindowInfo.aidSearch) {
					ImGui::PopID();
					continue;
				}
			}
		}

		// Most entries will contain a timestamp and presumably a version number, separated by "," characters.
		if (strchr(lbl, ',') != NULL) {
			strtok(lbl, ",");
		}

		//All (properly named) texture files end with "\default.rtx". Filter that out as well.
		if (strcmp(type, "texture") == 0) {
			strtok(lbl, "\\");
		}

		GLuint img = 0;

		// Display Challenge Icon
		if (strcmp(type, "challenge") == 0) {
			img = RC_PNG_CHALICON;
		}

		// Display Animation Icon
		if (strcmp(type, "anim") == 0) {
			img = RC_PNG_ANIMICON;
		}

		// Display Vehicle Icon
		if (strcmp(type, "vehicle") == 0) {
			img = RC_PNG_VEHICON;
		}

		// Display Havok Icon
		if (strcmp(type, "havok") == 0) {
			img = RC_PNG_HAVOKICON;
		}

		// Check if the item we're passing in is a misc type.
		if (strcmp(type, "misc") == 0) {
			// Display Audio Icon
			if (strcmp(subtype, "audio") == 0 || strcmp(subtype, "challengesfx") == 0 || strcmp(subtype, "frontendsfx") == 0) {
				img = RC_PNG_AUDIOICON;
			}

			// Display List Icon
			if (strcmp(subtype, "gameassetref") == 0) {
				img = RC_PNG_LISTICON;
			}
		}

		// Check if the item we're passing in is an objparam type.
		if (strcmp(type, "objparams") == 0) {
			// Display Vehicle Block Icon
			if (strcmp(subtype, "vehicleblock") == 0) {
				img = RC_PNG_VEHBLOCKICON;
			}
		}

		if (img != 0) {
			float main_scale = ImGui_ImplGlfw_GetContentScaleForWindow(window);
			ImGui::Image(img, ImVec2(26 * main_scale, 26 * main_scale));
			ImGui::SameLine();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ((13 * main_scale) - ImGui::CalcTextSize(lbl, NULL, false, -1.f).y / 2));
		}

		if (ImGui::Selectable(lbl, fileIdx == i + 1)) {
			if (fileIdx == i + 1) {
				fileId = -1;
				fileIdx = -1;
			}
			else {
				assetType = GetAssetIDFromType(type);
				if (assetType == 1 && activeTex != nullptr) {
					//activeTex->framePos = 0;
					delete(activeTex);
					activeTex = new Texture();
				}
				if (strcmp(lbl, "manifest") == 0) {
					assetType = 86;
				}

				fileId = i;
				fileIdx = i + 1;
			}
		}

		ImGui::PopID();
	}
}

void fillKameoDBFileList() {
	// Setup the local parameters needed.
	char lbl[1024];
	char type[32];
	char domain[32];
	char subtype[32];

	ImGui::Text("File List");
	ImGui::Separator();
	for (int i = 0; i < activeKameoDBFile.fileTable.header.entryCount; i++) {
		ImGui::PushID(i);
		memset(lbl, 0, 1024);
		memset(type, 0, 32);
		memset(domain, 0, 32);
		memset(subtype, 0, 32);

		// Check if the entry label we're reading contains "aid_".
		strcpy(lbl, activeKameoDBFile.fileTable.fileNames[i]);

		if (strlen(imGuiWindowInfo.search) != 0) {
			if (strstr(lbl, imGuiWindowInfo.search) == NULL) {
				ImGui::PopID();
				continue;
			}
		}

		if (imGuiWindowInfo.aidSearch != 0) {
		}

		if (ImGui::Selectable(lbl, fileId == i)) {
			if (fileId == i) {
				fileId = -1;
			}
			else {
				fileId = i;
			}
		}

		ImGui::PopID();
	}
}

void fillBundleV31FileList() {
	// Setup the local parameters needed.
	char lbl[1024];
	char type[32];
	char domain[32];
	char subtype[32];

	ImGui::Text("File List");
	ImGui::Separator();
	for (int i = 0; i < bundleFile.V31Bundle->header.numOfFiles; i++) {
		ImGui::PushID(i);
		memset(lbl, 0, 1024);
		memset(type, 0, 32);
		memset(domain, 0, 32);
		memset(subtype, 0, 32);

		// Check if the entry label we're reading contains "aid_".
		if (bundleFile.V31Bundle->fileInfoTable.debugTable.sizeOfStringTable != 0) {
			strcpy(lbl, bundleFile.V31Bundle->fileInfoTable.debugTable.fileNames[i]);
		}
		else {
			sprintf(lbl, "File %d", i);
		}

		if (strlen(imGuiWindowInfo.search) != 0) {
			if (strstr(lbl, imGuiWindowInfo.search) == NULL) {
				ImGui::PopID();
				continue;
			}
		}

		if (imGuiWindowInfo.aidSearch != 0) {
		}

		if (ImGui::Selectable(lbl, fileId == i)) {
			if (fileId == i) {
				fileId = -1;
				fileIdx = -1;
			}
			else {
				fileId = i;
				fileIdx = i + 1;
			}
		}

		ImGui::PopID();
	}
}

void fillBundleV26FileList() {
	// Setup the local parameters needed.
	char lbl[1024];
	char type[32];
	char domain[32];
	char subtype[32];

	ImGui::Text("File List");
	ImGui::Separator();
	for (int i = 0; i < bundleFile.V26Bundle->header.numOfFiles; i++) {
		ImGui::PushID(i);
		memset(lbl, 0, 1024);
		memset(type, 0, 32);
		memset(domain, 0, 32);
		memset(subtype, 0, 32);

		// Check if the entry label we're reading contains "aid_".
		if (bundleFile.V26Bundle->fileInfoTable.debugTable.sizeOfStringTable != 0) {
			strcpy(lbl, bundleFile.V26Bundle->fileInfoTable.debugTable.fileNames[i]);
		}
		else {
			sprintf(lbl, "File %d", i);
		}

		if (strlen(imGuiWindowInfo.search) != 0) {
			if (strstr(lbl, imGuiWindowInfo.search) == NULL) {
				ImGui::PopID();
				continue;
			}
		}

		if (imGuiWindowInfo.aidSearch != 0) {
		}

		if (ImGui::Selectable(lbl, fileId == i)) {
			if (fileId == i) {
				fileId = -1;
				fileIdx = -1;
			}
			else {
				fileId = i;
				fileIdx = i + 1;
			}
		}

		ImGui::PopID();
	}
}

void fillStreamBundleFileList() {

	if (imGuiWindowInfo.streamBundleSelectedBundle != -1) {
		fillStreamBundleFileListOfBundle();
		return;
	}

	for (int i = 0; i < streamBundleFile->header.totalFileTotal; i++) {
		char* ptr = 0;

		if (imGuiWindowInfo.aidSearch != 0) {
			if (streamBundleFile->fileEntries[i].aid != imGuiWindowInfo.aidSearch) {
				ImGui::PopID();
				continue;
			}
		}

		switch (streamBundleFile->bundleFiles[i].entryType)
		{
		case ENTRY_BUNDLE:
			ImGui::PushID(i);
			char lbl[1024];
			char type[32];
			memset(lbl, 0, 1024);
			memset(type, 0, 32);

			ptr = strstr(streamBundleFile->bundleFiles[i].bundleFile->sectionTable.fileLabelTable.fileLabels[0].label, "aid_");
			if (ptr != NULL) {
				// If so, copy the contents from the location of that and fetch the asset type.
				strcpy(lbl, ptr);

				assetGetTypeFromString(lbl + 4, type);
			}
			else {
				// Else, just copy the contents of the label to the dest.
				strcpy(lbl, streamBundleFile->bundleFiles[i].bundleFile->sectionTable.fileLabelTable.fileLabels[0].label);
			}

			if (strlen(imGuiWindowInfo.search) != 0) {
				if (strstr(lbl, imGuiWindowInfo.search) == NULL) {
					ImGui::PopID();
					continue;
				}
			}

			// Most entries will contain a timestamp and presumably a version number, separated by "," characters.
			if (strchr(lbl, ',') != NULL) {
				strtok(lbl, ",");
			}

			//All (properly named) texture files end with "\default.rtx". Filter that out as well.
			if (strcmp(type, "texture") == 0) {
				strtok(lbl, "\\");
			}

			if (ImGui::Button(lbl)) {
				imGuiWindowInfo.streamBundleSelectedItem = i;
				//imGuiWindowInfo.streamBundleSelectedBundle = i;
			}
			ImGui::PopID();
			break;
		case ENTRY_DNBW:
			ImGui::PushID(i);
			char bnkLbl[0x50];
			memset(bnkLbl, 0, 0x50);

			strcpy(bnkLbl, streamBundleFile->bundleFiles[i].waveBankFile->bankName);

			if (strlen(imGuiWindowInfo.search) != 0) {
				if (strstr(bnkLbl, imGuiWindowInfo.search) == NULL) {
					ImGui::PopID();
					continue;
				}
			}

			if (ImGui::Button(bnkLbl)) {
				imGuiWindowInfo.streamBundleSelectedItem = i;
			}
			ImGui::PopID();
			break;
		}
	}
}

void fillStreamBundleFileListOfBundle() {

	if (ImGui::ArrowButton("backToStreamBundle", ImGuiDir_Left)) {
		imGuiWindowInfo.streamBundleSelectedItem = -1;
		imGuiWindowInfo.streamBundleSelectedBundle = -1;
		fileIdx = -1;
		return;
	}
	ImGui::SameLine();
	ImGui::Text("Back to Bundle List");

	// Setup the local parameters needed.
	char lbl[1024];
	char type[32];
	char domain[32];
	char subtype[32];

	int currentBundle = imGuiWindowInfo.streamBundleSelectedBundle;

	for (int i = 0; i < streamBundleFile->bundleFiles[currentBundle].bundleFile->header.numAssets; i++) {
		ImGui::PushID(i);
		memset(lbl, 0, 1024);
		memset(type, 0, 32);
		memset(domain, 0, 32);
		memset(subtype, 0, 32);

		// Check if the entry label we're reading contains "aid_".
		char* ptr = strstr(streamBundleFile->bundleFiles[currentBundle].bundleFile->sectionTable.fileLabelTable.fileLabels[i].label, "aid_");
		if (ptr != NULL) {
			// If so, copy the contents from the location of that and fetch the asset type.
			strcpy(lbl, ptr);

			assetGetTypeFromString(lbl + 4, type);

			assetGetTypeFromString(lbl + 4 + (strlen(type) + 1), domain);

			if (strcmp(type, "misc") == 0 || strcmp(type, "objparams") == 0 || strcmp(type, "statetable") == 0) {
				assetGetTypeFromString(lbl + 4 + (strlen(type) + 1) + (strlen(domain) + 1), subtype);
			}
			else {
			}
		}
		else {
			// Else, just copy the contents of the label to the dest.
			strcpy(lbl, streamBundleFile->bundleFiles[currentBundle].bundleFile->sectionTable.fileLabelTable.fileLabels[i].label);
		}

		// Most entries will contain a timestamp and presumably a version number, separated by "," characters.
		if (strchr(lbl, ',') != NULL) {
			strtok(lbl, ",");
		}

		//All (properly named) texture files end with "\default.rtx". Filter that out as well.
		if (strcmp(type, "texture") == 0) {
			strtok(lbl, "\\");
		}

		GLuint img = 0;

		// Display Challenge Icon
		if (strcmp(type, "challenge") == 0) {
			img = RC_PNG_CHALICON;
		}

		// Display Animation Icon
		if (strcmp(type, "anim") == 0) {
			img = RC_PNG_ANIMICON;
		}

		// Display Vehicle Icon
		if (strcmp(type, "vehicle") == 0) {
			img = RC_PNG_VEHICON;
		}

		// Check if the item we're passing in is a misc type.
		if (strcmp(type, "misc") == 0) {
			// Display Audio Icon
			if (strcmp(subtype, "audio") == 0 || strcmp(subtype, "challengesfx") == 0 || strcmp(subtype, "frontendsfx") == 0) {
				img = RC_PNG_AUDIOICON;
			}

			// Display List Icon
			if (strcmp(subtype, "gameassetref") == 0) {
				img = RC_PNG_LISTICON;
			}
		}

		// Check if the item we're passing in is an objparam type.
		if (strcmp(type, "objparams") == 0) {
			// Display Vehicle Block Icon
			if (strcmp(subtype, "vehicleblock") == 0) {
				img = RC_PNG_VEHBLOCKICON;
			}
		}

		if (img != 0) {
			ImGui::Image(img, ImVec2(32.5, 32.5));
			ImGui::SameLine();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6.5);
		}

		if (ImGui::Button(lbl)) {
			assetType = -1;
			if (strcmp(lbl, "manifest") == 0) {
				assetType = 86;
			}

			ImGui::OpenPopup("Menu");
		}

		if (ImGui::BeginPopup("Menu", ImGuiWindowFlags_MenuBar)) {
			fileIdx = i + 1;

			ImGui::Text("Export Available Sections:");
			char sects[96];
			memset(sects, 0, 96);
			int availableSects = 0;
			for (int s = 0; s < streamBundleFile->bundleFiles[currentBundle].bundleFile->header.numSections; s++) {
				if (streamBundleFile->bundleFiles[currentBundle].bundleFile->sectionTable.fileInfos[s].ID == i + 1) {
					fileId = i;
					availableSects = availableSects + 1;
					char* label = streamBundleFile->bundleFiles[currentBundle].bundleFile->sectionTable.sectionLabels[streamBundleFile->bundleFiles[currentBundle].bundleFile->sectionTable.fileInfos[s].section - 1].label;
					ImGui::PushID(label);
					if (ImGui::Button(label)) {
						/*char* activeSect = 0;
						activeSect = bundleFile->getFileData(currentFileName, s);

						char file[1024];
						memset(file, 0, 1024);

						strcat(file, lbl);
						strcat(file, label);

						writeDataToFile(lbl, label, activeSect, bundleFile->sectionTable.fileInfos[s].dataSize);*/
					}

					ImGui::PopID();
					ImGui::SameLine();
				}
			}
			ImGui::NewLine();

			if (strcmp(type, "vehicle") == 0) {
				if (ImGui::Button("Load Vehicle")) {
					/*char* activeSect = 0;
					activeSect = bundleFile->getFileData(currentFileName, s);

					if (activeVehicle != nullptr) {
						free(activeVehicle);
						activeVehicle = nullptr;
					}

					// Need to quickly grab the number of parts first in order to allocate memort
					unsigned short numOfParts = 0;

					memcpy(&numOfParts, activeSect, sizeof(unsigned short));

					numOfParts = flipEndian(numOfParts);

					printf("Loading vehicle from file \"%s\"\nTotal Vehicle Filesize = %d.\n", lbl, numOfParts);
					activeVehicle = (Vehicle*)malloc(0x7C + (0x24 * numOfParts));
					activeVehicle->ReadVehicle(activeSect, false);

					imGuiWindowInfo.showVehicleEditor = true;*/
				}
			}

			ImGui::EndPopup();
		};
		ImGui::PopID();
	}
}

void fillGhouliesBundleFileList() {
	// Setup the local parameters needed.
	char lbl[1024];
	char type[32];
	char domain[32];
	char subtype[32];

	ImGui::Text("File List");
	ImGui::Separator();
	for (int i = 0; i < ghoulBundleFile.entryCount; i++) {
		ImGui::PushID(i);
		memset(lbl, 0, 1024);
		memset(type, 0, 32);
		memset(domain, 0, 32);
		memset(subtype, 0, 32);

		// Check if the entry label we're reading contains "aid_".
		char* ptr = strstr(ghoulBundleFile.fileEntries[i].fileName, "aid_");
		if (ptr != NULL) {
			// If so, copy the contents from the location of that and fetch the asset type.
			strcpy(lbl, ptr);

			assetGetTypeFromString(lbl + 4, type);

			assetGetTypeFromString(lbl + 4 + (strlen(type) + 1), domain);

			if (strcmp(type, "misc") == 0 || strcmp(type, "objparams") == 0 || strcmp(type, "statetable") == 0) {
				assetGetTypeFromString(lbl + 4 + (strlen(type) + 1) + (strlen(domain) + 1), subtype);
			}
		}
		else {
			// Else, just copy the contents of the label to the dest.
			strcpy(lbl, ghoulBundleFile.fileEntries[i].fileName);
		}

		if (strlen(imGuiWindowInfo.search) != 0) {
			if (strstr(lbl, imGuiWindowInfo.search) == NULL) {
				ImGui::PopID();
				continue;
			}
		}

		//All (properly named) texture files end with "\default.rtx". Filter that out as well.
		if (strcmp(type, "texture") == 0) {
			strtok(lbl, "\\");
		}

		if (ImGui::Selectable(lbl, fileIdx == i + 1)) {
			if (fileIdx == i + 1) {
				fileId = -1;
				fileIdx = -1;
			}
			else {
				if (ghoulBundleFile.fileEntries[i].type == 1) {
					if (activeGhoulTex != nullptr) {
						activeGhoulTex->framePos = 0;
						activeGhoulTex->refresh = true;
					}
				}
				fileId = i;
				fileIdx = i + 1;
			}
		}

		ImGui::PopID();
	}
}

void fillRPKFileList() {
	ImGui::Text("File List");
	ImGui::Separator();
	for (int i = 0; i < rpkFile->fileCount; i++) {
		ImGui::PushID(i);

		if (strlen(imGuiWindowInfo.search) != 0) {
			if (strstr(rpkFile->fileEntries[i].fileName, imGuiWindowInfo.search) == NULL) {
				ImGui::PopID();
				continue;
			}
		}

		if (ImGui::Selectable(rpkFile->fileEntries[i].fileName, fileId == i)) {
			if (fileId == i) {
				fileId = -1;
				fileIdx = -1;
			}
			else {
				fileId = i;
				fileIdx = i + 1;
			}
		}

		ImGui::PopID();
	}
}

void fillPinataDbBundleFileList() {
	// Setup the local parameters needed.
	char lbl[1024];
	char type[32];
	char domain[32];
	char subtype[32];

	ImGui::Text("File List");
	ImGui::Separator();
	for (int i = 0; i < PinataDbBundleFile.hashFile.fileCount; i++) {
		ImGui::PushID(i);
		memset(lbl, 0, 1024);
		memset(type, 0, 32);
		memset(domain, 0, 32);
		memset(subtype, 0, 32);

		// Check if the entry label we're reading contains "aid_".
		int idx = PinataDbBundleFile.precachedEntries[i].indexIdx;
		if (idx != -1) {
			strcpy(lbl, PinataDbBundleFile.indexFile[idx].filename);

			assetGetTypeFromString(lbl + 4, type);

			assetGetTypeFromString(lbl + 4 + (strlen(type) + 1), domain);

			if (strcmp(type, "misc") == 0 || strcmp(type, "objparams") == 0 || strcmp(type, "statetable") == 0) {
				assetGetTypeFromString(lbl + 4 + (strlen(type) + 1) + (strlen(domain) + 1), subtype);
			}
			else {
			}
		}
		else {
			sprintf(lbl, "CAFF Entry %d", i);
		}


		if (strlen(imGuiWindowInfo.search) != 0) {
			if (strstr(lbl, imGuiWindowInfo.search) == NULL) {
				ImGui::PopID();
				continue;
			}
		}

		// Most entries will contain a timestamp and presumably a version number, separated by "," characters.
		if (strchr(lbl, ' ') != NULL) {
			strtok(lbl, " ");
		}

		if (ImGui::Selectable(lbl, fileIdx == i + 1)) {
			if (fileIdx == i + 1) {
				fileId = -1;
				fileIdx = -1;
			}
			else {
				assetType = GetAssetIDFromType(type);
				if (assetType == 1 && activeTex != nullptr) {
					//activeTex->framePos = 0;
					delete(activeTex);
					activeTex = new Texture();
				}

				fileId = i;
				fileIdx = i + 1;
			}
		}

		ImGui::PopID();
	}
}
#pragma endregion

void readMarkerFile(char* data) {
	GetMarkerEditorWindowParameters()->isFileActive = false;

	GetMarkerEditorWindowParameters()->activeMarker.ReadMarkerFile(data);

	GetMarkerEditorWindowParameters()->isFileActive = true;

	SetupMarkerEditorWindow(false);

	CloseLoadingPromptWidget();
}

void readLoctextFile(char* data, char* fileName) {
	LoctextWindowParams* locParams = getLoctextWindowParams();
	locParams->ready = false;

	if (locParams->activeLoctext != nullptr) {
		delete locParams->activeLoctext;
		locParams->activeLoctext = nullptr;
	}

	printf("Loading loctext from file \"%s\".\n", fileName);
	locParams->activeLoctext = new Loctext();
	locParams->activeLoctext->ReadLoctext(data);

	locParams->ready = true;

	locParams->showLoctextEditor = true;

	CloseLoadingPromptWidget();
}

void writeCaffFile(const char* fileName) {

	if (!bundleSetup.isDirty) {
		PRINT("No changes have been made to the CAFF file.\n");
	}

	if (bundleFile.V36Bundle->header.compression) {
		CloseLoadingBarPromptWidget();
		FireMessage("We currently don't support saving compressed files.", ErrorType_Info);
		//printf("We currently don't support saving compressed files.\n");
		return;
	}

	FILE* newFile = fopen(fileName, "wb");

	if (newFile == nullptr) {
		switch (errno) {
		default:
			ASSERT("An unmanaged error has occured while attempting to open the CAFF file for writing. Error - %d\n", errno);
			return;
		case ENOENT:
			ASSERT("No such file or directory could be found... while attempting to open the active CAFF file for writing. This should never be able to be reached.\n");
			return;
		case EACCES:
			ASSERT("A permission denied error occured while attempting to open the CAFF file for writing. File might be set to read-only.\n");
			return;
		case EIO:
			ASSERT("An I/O error occured while attempting to open the CAFF file for writing.\n");
			return;
		}
	}

	try {
		fwrite(bundleFile.V36Bundle->bundleData, 1, bundleFile.V36Bundle->header.headerSize + bundleFile.V36Bundle->header.sectionTableUncompedSize + bundleFile.V36Bundle->header.fileTableUncompedSize, newFile);

		int baseOffset = bundleFile.V36Bundle->header.headerSize + bundleFile.V36Bundle->header.sectionTableUncompedSize + bundleFile.V36Bundle->header.fileTableUncompedSize;

		// Allocate a padding buffer to use for writing necessary padding.
		char* buffer = (char*)malloc(0x1000);
		memset(buffer, 0, 0x1000);

		for (int i = 0; i < bundleFile.V36Bundle->header.numSectionTypes; i++) {

			int shiftingOffset = 0;
			int sectSize = 0;

			for (int f = 0; f < bundleFile.V36Bundle->header.numAssets; f++) {
				int id = bundleFile.V36Bundle->GetMatchingFileInfoIdx(f + 1, i + 1);
				if (id == -1) {
					continue;
				}

				int offsetRemains = 0;
				int originalPos = 0;
				int newSize = 0;
				int boundarySize = 0x10;

				char* lbl = bundleFile.V36Bundle->sectionTable.fileLabelTable.fileLabels[f].label;
				if (strstr(bundleFile.V36Bundle->sectionTable.fileLabelTable.fileLabels[f].label, "aid_") != NULL) {
					lbl = strstr(bundleFile.V36Bundle->sectionTable.fileLabelTable.fileLabels[f].label, "aid_");
				}

				PRINT("File Info [File %d - Section %d] - %s>\n", f + 1, i + 1, lbl);

				// .texturegpu has a buffer allocation of 4096. Every other section appears to have a buffer allocation of 16.
				bool isGPUSect = bundleFile.V36Bundle->isGPUSection(bundleFile.V36Bundle->sectionTable.fileInfos[id].section - 1);
				bool isStreamSect = bundleFile.V36Bundle->isStreamSection(bundleFile.V36Bundle->sectionTable.fileInfos[id].section - 1);
				bool isTexture = isGPUSect && IsValidTextureFile(lbl + 4);
				bool isModel = isGPUSect && IsValidModelFile(lbl + 4);
				bool isModelStream = isStreamSect && IsValidModelFile(lbl + 4);

				if (isTexture) { // Textures have the largest boundary space, needing to be on a boundary of 4096 bytes.
					boundarySize = 0x1000;
				}
				else if (isModel) { // Models are on a boundary of 32 bytes.
					boundarySize = 0x20;
				}
				else if (isModelStream) { // Model entries on the stream sect are on a boundary of 8 bytes.
					boundarySize = 4;
				}
				else { // Most files (such as the ones in the data section) are on a boundary of 16 bytes.
					boundarySize = 0x10;
				}

				if (bundleSetup.doesBufferedSaveExist(f + 1, i + 1)) {
					int bufferedSaveId = bundleSetup.getIdOfBufferedSave(f + 1, i + 1);

					if (f + 1 < bundleFile.V36Bundle->header.numAssets && (bundleSetup.bufferedSaves[bufferedSaveId].dataSize % boundarySize) != 0) {
						offsetRemains = bundleSetup.bufferedSaves[bufferedSaveId].dataSize % boundarySize;
					}
					else {
						offsetRemains = boundarySize;
					}

					newSize = bundleSetup.bufferedSaves[bufferedSaveId].dataSize + (boundarySize - offsetRemains);

					fwrite(bundleSetup.bufferedSaves[bufferedSaveId].savedData, bundleSetup.bufferedSaves[bufferedSaveId].dataSize, 1, newFile);
					fwrite(buffer, 1, (boundarySize - offsetRemains), newFile);

					// Grab the current pos
					int tempPos = ftell(newFile);
					fseek(newFile, bundleFile.V36Bundle->sectionTable.fileInfosOffset + (id * 0xE) + 4, SEEK_SET);

					// Write new file info data to accomodate for the new offset.
					int beShiftingOffset = flipEndian(shiftingOffset);
					int beDataSize = flipEndian(bundleSetup.bufferedSaves[bufferedSaveId].dataSize);
					fwrite(&beShiftingOffset, 4, 1, newFile);
					fwrite(&beDataSize, 4, 1, newFile);

					// return to our original pos.
					fseek(newFile, tempPos, SEEK_SET);

					shiftingOffset += bundleSetup.bufferedSaves[bufferedSaveId].dataSize + (boundarySize - offsetRemains);

					PRINT("File Info [File %d - Section %d] - <New Data Size - %d, Default Offset - %d, Next Offset - %d (%d)>\n", f + 1, i + 1, bundleSetup.bufferedSaves[bufferedSaveId].dataSize, bundleFile.V36Bundle->sectionTable.fileInfos[id].dataOffset, shiftingOffset, boundarySize - offsetRemains);
				}
				else {
					if (f + 1 < bundleFile.V36Bundle->header.numAssets && (bundleFile.V36Bundle->sectionTable.fileInfos[id].dataSize % boundarySize) != 0) {
						offsetRemains = bundleFile.V36Bundle->sectionTable.fileInfos[id].dataSize % boundarySize;
					}
					else {
						offsetRemains = boundarySize;
					}

					newSize = bundleFile.V36Bundle->sectionTable.fileInfos[id].dataSize + (boundarySize - offsetRemains);

					fwrite(bundleFile.V36Bundle->bundleData + baseOffset + bundleFile.V36Bundle->sectionTable.fileInfos[id].dataOffset, 1, bundleFile.V36Bundle->sectionTable.fileInfos[id].dataSize, newFile);

					fwrite(buffer, 1, (boundarySize - offsetRemains), newFile);

					// Grab the current pos
					int tempPos = ftell(newFile);
					fseek(newFile, bundleFile.V36Bundle->sectionTable.fileInfosOffset + (id * 0xE) + 4, SEEK_SET);

					// Write new file info data to accomodate for the new offset.
					int beShiftingOffset = flipEndian(shiftingOffset);
					int beDataSize = flipEndian(bundleFile.V36Bundle->sectionTable.fileInfos[id].dataSize);
					fwrite(&beShiftingOffset, 4, 1, newFile);
					fwrite(&beDataSize, 4, 1, newFile);

					// return to our original pos.
					fseek(newFile, tempPos, SEEK_SET);

					shiftingOffset += bundleFile.V36Bundle->sectionTable.fileInfos[id].dataSize + (boundarySize - offsetRemains);
					PRINT("File Info [File %d - Section %d] - <Data Size - %d, Default Offset - %d, Next Offset - %d (%d)>\n", f + 1, i + 1, bundleFile.V36Bundle->sectionTable.fileInfos[id].dataSize, bundleFile.V36Bundle->sectionTable.fileInfos[id].dataOffset, shiftingOffset, boundarySize - offsetRemains);
				}

				IncreaseCurrentSavedOnLoadingWidget();
				sectSize += newSize;
			}

			baseOffset += bundleFile.V36Bundle->sectionTable.entries[i].uncompressedSize;

			// Grab the current pos
			int tempPos = ftell(newFile);
			fseek(newFile, bundleFile.V36Bundle->header.headerSize + (i * 0x21) + 0x9, SEEK_SET);

			// Write new file info data to accomodate for the new offset.
			int beSectSize = flipEndian(sectSize);
			fwrite(&beSectSize, 4, 1, newFile);

			fseek(newFile, bundleFile.V36Bundle->header.headerSize + (i * 0x21) + 0x1D, SEEK_SET);
			fwrite(&beSectSize, 4, 1, newFile);

			// return to our original pos.
			fseek(newFile, tempPos, SEEK_SET);
		}
	}
	catch (int e) {
		switch (e) {
		default:
			PRINT("An unmanaged error has occured while attempting to write to the CAFF file. Error - %d\n", e);
			break;
		case ENOENT:
			PRINT("No such file or directory could be found... while attempting to write to the active CAFF file. This should never be able to be reached.\n");
			break;
		case EACCES:
			PRINT("A permission denied error occured while attempting to write to the CAFF file. File might be set to read-only.\n");
			break;
		case EIO:
			PRINT("An I/O error occured while attempting to write to the CAFF file.\n");
			break;
		case EINVAL:
			PRINT("An invalid argument was passed to a function while attempting to write to the CAFF file.\n");
			break;
		}
	}

	fflush(newFile);
	fclose(newFile);

	CloseLoadingBarPromptWidget();
}

void readOtherSupportedFile(int type) {
	SetupLoadingPromptWidget("Currently reading the file. Please wait.");

	if (type == CaffType::GHOUL_BUNDLE) {
		if (!ghoulBundleFile.readStandaloneBundleFile(currentFileName)) {
			CloseLoadingPromptWidget();
			FireMessage("Either the file provided is bad or this is not a valid bundle file.\n", ErrorType_Warn);
			imGuiWindowInfo.saveData.targetType = NONE;
		}
	}

	if (type == CaffType::GHOUL_DEMAND) {
		if (!ghoulDemandFile.readStandaloneDemandFile(currentFileName)) {
			CloseLoadingPromptWidget();
			FireMessage("Either the file provided is bad or this is not a valid bundle file.\n", ErrorType_Warn);
			imGuiWindowInfo.saveData.targetType = NONE;
		}
	}

	if (type == CaffType::PINATA_DBBUNDLE) {
		if (!PinataDbBundleFile.readStandaloneDbBundleFiles(currentFileName)) {
			CloseLoadingPromptWidget();
			FireMessage("A problem was encountered while reading the data.\nIf this is a Trouble in Paradise file, we do not currently support it until the hashing code is reversed.\nOtherwise, the files might be bad or the files needed aren't in the directory given.", ErrorType_Warn);
			imGuiWindowInfo.saveData.targetType = NONE;
		}
	}

	if (type == CaffType::RR_RPK) {

	}

	CloseLoadingPromptWidget();
}

void readCaffFile() {
	FILE* fdart = fopen(currentFileName, "rb");

	char* outData = nullptr;

	unsigned short val = 0;
	fread(&val, 2, 1, fdart);

	fseek(fdart, 0, SEEK_SET);

	PRINT("%04X\n", val);

	if (val == 0xF50F) { // Failsafe until we can somehow reverse engineer the Xbox 360 specific compression.
		CloseLoadingPromptWidget();
		FireMessage("The file provided was compressed with xbcompress.\nPlease run the file through xbdecompress first.\n", ErrorType_Info);
		return;
	}

	// We only need to get about the first 0x14 bytes of data.
	if (val == 0xDA78) {
		SetupLoadingPromptWidget("Currently decompressing the file. Please wait.");
		outData = inf(fdart);
		PRINT("comp\n");
	}

	fclose(fdart);

	if (val != 0xDA78) {
		outData = ReadContentsFromFile(currentFileName);
		PRINT("no comp\n");
	}

	PRINT("%08X\n", outData);

	unsigned int caffMagic;

	memcpy(&caffMagic, outData, 4);

	char caffVersion[0x10];

	memcpy(&caffVersion, outData + 4, 0x10);

	PRINT("MAGIC: %08x, VERSION: %s\n", caffMagic, caffVersion);

	if (caffMagic != 0x46464143 && caffMagic != 0x7CB48C43) { 
		//PRINT("The file supplied is not a valid bundle or streambundle file.\n");
		CloseLoadingPromptWidget();
		FireMessage("The file supplied is not a valid bundle or streambundle file.\n", ErrorType_Warn);
		return; 
	}

	SetupLoadingPromptWidget("Currently reading the file. Please wait.");

	// Check if the read magic matches the one for a bundle or streambundle.
	if (caffMagic == 0x46464143) {
		PRINT("About to start reading a standalone bundle file.\n");

		if (strcmp(caffVersion, "24.09.03.0026") == 0) {
			imGuiWindowInfo.saveData.targetType = BUNDLEV26;
		}

		if (strcmp(caffVersion, "28.01.05.0031") == 0) {
			imGuiWindowInfo.saveData.targetType = BUNDLEV31;
		}

		if (strcmp(caffVersion, "07.08.06.0036") == 0) {
			imGuiWindowInfo.saveData.targetType = BUNDLEV36;
		}

		if (!bundleFile.ReadBundleFile(outData)) {
			CloseLoadingPromptWidget();
			imGuiWindowInfo.saveData.targetType = NONE;
		}

		//imGuiWindowInfo.saveData.loadThread = std::thread(&BundleFile::ReadBundleFile, &bundleFile, outData);
		//imGuiWindowInfo.saveData.loadThread.detach();
	}
	else if(caffMagic == 0x7CB48C43) {
		if (streamBundleFile == nullptr) {
			streamBundleFile = new StreamBundle();
		}

		streamBundleFile->readStandaloneStreamBundleFile(currentFileName);
		imGuiWindowInfo.saveData.targetType = NB_STREAMBUNDLE;
		//imGuiWindowInfo.saveData.loadThread = std::thread(&StreamBundle::readStandaloneStreamBundleFile, streamBundleFile, currentFileName);
		//imGuiWindowInfo.saveData.loadThread.detach();
	}
	return;
}

void writeStreamBundleFile(const char* fileName) {

	if (!bundleSetup.isDirty) {
		printf("No changes have been made to the CAFF file.\n");
	}

	FILE* currentFile = fopen(currentFileName, "rb");

	if (ferror(currentFile) != 0) {
		printf("Error occured while trying to open the file.\n");
		return;
	}

	fseek(currentFile, 0L, SEEK_END);
	int length = ftell(currentFile);
	fseek(currentFile, 0L, SEEK_SET);

	char* data = (char*)malloc(length);

	fread(data, sizeof(char), length, currentFile);

	fclose(currentFile);

	FILE* newFile = fopen(fileName, "wb");

	if (newFile == nullptr) {
		switch (errno) {
		default:
			printf("An unmanaged error has occured while attempting to open the CAFF file for writing. Error - %d\n", errno);
			return;
		case ENOENT:
			printf("No such file or directory could be found... while attempting to open the active CAFF file for writing. This should never be able to be reached.\n");
			return;
		case EACCES:
			printf("A permission denied error occured while attempting to open the CAFF file for writing. File might be set to read-only.\n");
			return;
		case EIO:
			printf("An I/O error occured while attempting to open the CAFF file for writing.\n");
			return;
		}
	}

	try {
		fwrite(data, 1, 0x10, newFile);

		int entryTotalSize = streamBundleFile->header.totalFileTotal * 0xC;
		int refTotalSize = streamBundleFile->header.referenceTableCount * 0x4;

		int refCount = flipEndian(streamBundleFile->header.referenceTableCount);
		fwrite(&refCount, 4, 1, newFile);

		for (int i = 0; i < streamBundleFile->header.referenceTableCount; i++) {
			unsigned int ref = flipEndian(streamBundleFile->header.referenceTable[i]);
			fwrite(&ref, 4, 1, newFile);
		}

		int offset = 0x14 + refTotalSize + entryTotalSize;

		for (int i = 0; i < streamBundleFile->header.totalFileTotal; i++) {
			int writeAid = flipEndian(streamBundleFile->fileEntries[i].aid);
			int writeOffset = flipEndian(offset);
			int writeSize = flipEndian(streamBundleFile->fileEntries[i].dataSize);

			int sizeOfData = streamBundleFile->fileEntries[i].dataSize;

			if (bundleSetup.doesBufferedSaveExist(i, 0)) {
				writeSize = flipEndian(bundleSetup.bufferedSaves[bundleSetup.getIdOfBufferedSave(i, 0)].dataSize);
				sizeOfData = bundleSetup.bufferedSaves[bundleSetup.getIdOfBufferedSave(i, 0)].dataSize;
			}

			fwrite(&writeAid, 4, 1, newFile);
			fwrite(&writeOffset, 4, 1, newFile);
			fwrite(&writeSize, 4, 1, newFile);

			offset += sizeOfData;
		}

		for (int i = 0; i < streamBundleFile->header.totalFileTotal; i++) {
			if (bundleSetup.doesBufferedSaveExist(i, 0)) {
				fwrite(bundleSetup.bufferedSaves[bundleSetup.getIdOfBufferedSave(i, 0)].savedData, 1, bundleSetup.bufferedSaves[bundleSetup.getIdOfBufferedSave(i, 0)].dataSize, newFile);
			}
			else {
				fwrite(data + streamBundleFile->fileEntries[i].offset, 1, streamBundleFile->fileEntries[i].dataSize, newFile);
			}

			imGuiWindowInfo.saveData.currentSaved++;
		}
	}
	catch (int e) {
		switch (e) {
		default:
			printf("An unmanaged error has occured while attempting to write to the CAFF file. Error - %d\n", e);
			break;
		case ENOENT:
			printf("No such file or directory could be found... while attempting to write to the active CAFF file. This should never be able to be reached.\n");
			break;
		case EACCES:
			printf("A permission denied error occured while attempting to write to the CAFF file. File might be set to read-only.\n");
			break;
		case EIO:
			printf("An I/O error occured while attempting to write to the CAFF file.\n");
			break;
		case EINVAL:
			printf("An invalid argument was passed to a function while attempting to write to the CAFF file.\n");
			break;
		}
	}

	fflush(newFile);
	fclose(newFile);
}

void writeDataToFile(const char* fileName, const char* filter, char *data, size_t dataSize) {
	char* outPath;

	if (NFD_PickFolderU8(&outPath, fileName) == NFD_OKAY) {

		char* buf = (char*)malloc(1024);
		sprintf(buf, "%s\\%s%s", outPath, fileName, filter);

		printf("%s\n",outPath);

		FILE* writeFile = fopen(buf, "wb");

		if (writeFile == NULL) {
			printf("An error occured while creating the output file (%s).\n", buf);
			return;
		}

		size_t written = fwrite(data, sizeof(char), dataSize, writeFile);

		if (written != dataSize) {
			printf("An error occured while writing data to output file.\n");
		}

		int flush = fflush(writeFile);

		if (flush != 0) {
			printf("An error occured while flushing data to output file.\n");
		}

		fclose(writeFile);

		free((void*)buf);
	}
}

/// <summary>
/// Disposes of any active file.
/// </summary>
void disposeAndCloseActiveFile() {
	bundleFile.ClearActiveBundleFile();

	if (streamBundleFile != nullptr) {
		delete(streamBundleFile);
		streamBundleFile = nullptr;
	}

	ghoulBundleFile.ClearBundleFileData();

	PinataDbBundleFile.ClearActiveBundleData();

	if (rpkFile != nullptr) {
		delete(rpkFile);
		rpkFile = nullptr;
	}

	fileId = -1;
	fileIdx = -1;
	assetType = -1;

	if (activeManifest != nullptr) {
		delete(activeManifest);
		activeManifest = nullptr;
	}
	
	if (activeTex != nullptr) {
		free(activeTex);
		activeTex = nullptr;
	}

	activeConkerTex.refresh = true;

	if (activeGhoulTex != nullptr) {
		activeGhoulTex->refresh = true;
		free(activeGhoulTex);
		activeGhoulTex = nullptr;
	}

	if (tempTexInt != -1) {
		glDeleteTextures(1, &tempTexInt);
	}

	tempTexInt = -1;
	fileType = CaffType::NONE;
	imGuiWindowInfo.saveData.targetType = NONE;
}

void TestBundleRecompilation() {
	for (int i = 0; i < bundleFile.V36Bundle->header.numSectionTypes; i++) {

		int shiftingOffset = 0;
		int sectSize = 0;

		for (int f = 0; f < bundleFile.V36Bundle->header.numAssets; f++) {
			int id = bundleFile.V36Bundle->GetMatchingFileInfoIdx(f + 1, i + 1);
			if (id == -1) {
				continue;
			}

			int offsetRemains = 0;
			int originalPos = 0;
			int newSize = 0;
			int boundarySize = 0x10;

			char* lbl = bundleFile.V36Bundle->sectionTable.fileLabelTable.fileLabels[f].label;
			if (strstr(bundleFile.V36Bundle->sectionTable.fileLabelTable.fileLabels[f].label, "aid_") != NULL) {
				lbl = strstr(bundleFile.V36Bundle->sectionTable.fileLabelTable.fileLabels[f].label, "aid_");
			}

			printf("File Info [File %d - Section %d] - %s>", f + 1, i + 1, lbl);

			// .texturegpu has a buffer allocation of 4096. Every other section appears to have a buffer allocation of 16.
			bool isGPUSect = bundleFile.V36Bundle->isGPUSection(bundleFile.V36Bundle->sectionTable.fileInfos[id].section - 1);
			bool isStreamSect = bundleFile.V36Bundle->isStreamSection(bundleFile.V36Bundle->sectionTable.fileInfos[id].section - 1);
			bool isTexture = isGPUSect && IsValidTextureFile(lbl + 4);
			bool isModel = isGPUSect && IsValidModelFile(lbl + 4);
			bool isModelStream = isStreamSect && IsValidModelFile(lbl + 4);

			if (isTexture) { // Textures have the largest boundary space, needing to be on a boundary of 4096 bytes.
				boundarySize = 0x1000;
			}
			else if (isModel) { // Models are on a boundary of 32 bytes.
				boundarySize = 0x20;
			}
			else if (isModelStream) { // Model entries on the stream sect are on a boundary of 8 bytes.
				boundarySize = 4;
			}
			else { // Most files (such as the ones in the data section) are on a boundary of 16 bytes.
				boundarySize = 0x10;
			}

			// Check what the next item in front of us is.
			int nextId = bundleFile.V36Bundle->GetMatchingFileInfoIdx(f + 2, i + 1);
			if (nextId != -1) {
				char* nextLbl = bundleFile.V36Bundle->sectionTable.fileLabelTable.fileLabels[f + 1].label;
				if (IsValidModelFile(nextLbl + 4)) {
					boundarySize = 0x20;
				}
			}

			if (f + 1 < bundleFile.V36Bundle->header.numAssets && (bundleFile.V36Bundle->sectionTable.fileInfos[id].dataSize % boundarySize) != 0) {
				offsetRemains = bundleFile.V36Bundle->sectionTable.fileInfos[id].dataSize % boundarySize;
			}
			else {
				offsetRemains = boundarySize;
			}

			newSize = bundleFile.V36Bundle->sectionTable.fileInfos[id].dataSize + (boundarySize - offsetRemains);

			shiftingOffset += bundleFile.V36Bundle->sectionTable.fileInfos[id].dataSize + (boundarySize - offsetRemains);
			printf("\n");

			printf("File Info [File %d - Section %d] - {", f + 1, i + 1);
			printf(" Data Size - %d, Default Offset - %d, Next Offset - %d (%d)>", bundleFile.V36Bundle->sectionTable.fileInfos[id].dataSize, bundleFile.V36Bundle->sectionTable.fileInfos[id].dataOffset, shiftingOffset, boundarySize - offsetRemains);

			if (nextId != -1) {
				int nextOffset = bundleFile.V36Bundle->sectionTable.fileInfos[nextId].dataOffset;

				if (nextOffset - shiftingOffset != 0) {
					printf(" | Error with padding, Next Offset is off by %d.", nextOffset - shiftingOffset);
				}
			}
			printf(" }\n");

			sectSize += newSize;
		}
	}
}

void ReadGhoulBundleTexture() {
	char* dataSect = ghoulBundleFile.GetFileData(fileId);

	activeGhoulTex->ParseTextureHeader(dataSect);

	free(dataSect);

	char* texData = (char*)malloc(activeGhoulTex->header.chunkSize);

	char* gpuSect = ghoulBundleFile.GetFileGPUForTexture(fileId);

	memcpy(texData, gpuSect + (activeGhoulTex->header.chunkSize * activeGhoulTex->framePos), activeGhoulTex->header.chunkSize);

	if (tempTexInt != -1) {
		glDeleteTextures(1, &tempTexInt);
	}

	tempTexInt = LoadImageFromData_Base(texData, activeGhoulTex->header.width, activeGhoulTex->header.height, activeGhoulTex->header.format);
	printf("tempTexInt %d\n", tempTexInt);

	free(texData);
	free(gpuSect);
}

void ReadConkerLiveReloadedTexture(char* gpuData) {

	if (tempTexInt != -1) {
		glDeleteTextures(1, &tempTexInt);
	}

	tempTexInt = LoadImageFromData_Base(gpuData, activeConkerTex.header.width, activeConkerTex.header.height, activeConkerTex.header.format);
	printf("tempTexInt %d\n", tempTexInt);
}

void ReadGhoulDemandTexture() {
	char* texData = (char*)malloc(activeGhoulTex->gpuHeader.chunkSize);
	memcpy(texData, ghoulDemandFile.dataPtr + ghoulDemandFile.gpuSectOffset + activeGhoulTex->gpuHeader.offset + (activeGhoulTex->header.chunkSize * activeGhoulTex->framePos), activeGhoulTex->header.chunkSize);

	if (tempTexInt != -1) {
		glDeleteTextures(1, &tempTexInt);
	}

	tempTexInt = LoadImageFromData_Base(texData, activeGhoulTex->header.width, activeGhoulTex->header.height, activeGhoulTex->header.format);
	printf("tempTexInt %d\n", tempTexInt);
	free(texData);
}

/// <summary>
/// Handles the functionality of the Loctext Editor window.
/// </summary>
static void DisplaySaveEditorBaseWindow() {
	if (ImGui::Begin("Save Editor", &imGuiWindowInfo.showSaveEditor, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
		if (ImGui::BeginMenuBar())
		{
			bool clearActive = activeSave != nullptr;

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load From File")) {
					openLoadSaveFile();
				}

				if (ImGui::MenuItem("Save", NULL, false, clearActive)) {
				}
				if (ImGui::MenuItem("Save as New", NULL, false, clearActive)) {
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Clear", NULL, false, clearActive)) {
					if (activeSave != nullptr) {
						delete activeSave;
						activeSave = nullptr;
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

		ImGui::Text("This is for Nuts & Bolts only");

		if (activeSave != nullptr) {
			ImGui::Text("Simplified provides a easy way to modify key values. This is the recommended option.");
			ImGui::Text("Advanced is not recommended unless you know what you're doing, as this provides every available value.");

			if (ImGui::TreeNode("Simplified")) {
				if (ImGui::TreeNode("Global")) {
					int jiggyCount = activeSave->GetGameCounter(57);
					ImGui::InputScalar("Jiggies Banked", ImGuiDataType_U32, &jiggyCount);
					activeSave->SetGameCounter(jiggyCount, 57);

					int jiggyCount_NA = activeSave->GetGameCounter(61);
					ImGui::InputScalar("Jiggies Won - NA", ImGuiDataType_U32, &jiggyCount_NA);
					activeSave->SetGameCounter(jiggyCount_NA, 61);

					int jiggyCount_CPU = activeSave->GetGameCounter(59);
					ImGui::InputScalar("Jiggies Won - CPU", ImGuiDataType_U32, &jiggyCount_CPU);
					activeSave->SetGameCounter(jiggyCount_CPU, 59);

					int jiggyCount_Banjoland = activeSave->GetGameCounter(58);
					ImGui::InputScalar("Jiggies Won - Banjoland", ImGuiDataType_U32, &jiggyCount_Banjoland);
					activeSave->SetGameCounter(jiggyCount_Banjoland, 58);

					int jiggyCount_WeirdWest = activeSave->GetGameCounter(65);
					ImGui::InputScalar("Jiggies Won - Weird West", ImGuiDataType_U32, &jiggyCount_WeirdWest);
					activeSave->SetGameCounter(jiggyCount_WeirdWest, 65);

					int jiggyCount_Jiggosseum = activeSave->GetGameCounter(66);
					ImGui::InputScalar("Jiggies Won - Jiggosseum", ImGuiDataType_U32, &jiggyCount_Jiggosseum);
					activeSave->SetGameCounter(jiggyCount_Jiggosseum, 66);

					int jiggyCount_ToT = activeSave->GetGameCounter(63);
					ImGui::InputScalar("Jiggies Won - Terrarium of Terror", ImGuiDataType_U32, &jiggyCount_ToT);
					activeSave->SetGameCounter(jiggyCount_ToT, 63);

					int jiggyCount_SM = activeSave->GetGameCounter(62);
					ImGui::InputScalar("Jiggies Won - Spiral Mountain", ImGuiDataType_U32, &jiggyCount_SM);
					activeSave->SetGameCounter(jiggyCount_SM, 62);

					int jiggyCount_JR = activeSave->GetGameCounter(60);
					ImGui::InputScalar("Jiggies Won - Jolly Dodger", ImGuiDataType_U32, &jiggyCount_JR);
					activeSave->SetGameCounter(jiggyCount_JR, 60);

					int jiggyCount_TT = activeSave->GetGameCounter(64);
					ImGui::InputScalar("Jiggies Won - Trophy Thomas", ImGuiDataType_U32, &jiggyCount_TT);
					activeSave->SetGameCounter(jiggyCount_TT, 64);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Showdown Town")) {
					bool tmpVal = activeSave->GetNormalGameFlag(37);
					//printf("%d %d %d | ", 37, 37 % 8, 37 / 8);
					ImGui::Checkbox("Always Set", &tmpVal);
					activeSave->SetNormalGameFlag(tmpVal, 37);
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Advanced")) {
				if (ImGui::TreeNode("Normal Game Flags")) {
					int idx = 0;
					for (int i = 0; i < activeSave->gameFlagNormalByteCount; i++) {
						ImGui::PushID(i);

						DisplayFlagByteValues(&activeSave->gameFlagNormalArr[i], idx, NORMAL_GAME_FLAG_COUNT, NormalGameFlagNames);
						idx += 8;

						ImGui::PopID();
					}
					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Global Game Flags")) {
					int idx = 0;
					for (int i = 0; i < activeSave->gameFlagGlobalByteCount; i++) {
						ImGui::PushID(i);

						DisplayFlagByteValues(&activeSave->gameFlagGlobalArr[i], idx, GLOBAL_GAME_FLAG_COUNT, GlobalGameFlagNames);
						idx += 8;

						ImGui::PopID();
					}
					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Volatile Game Flags")) {
					int idx = 0;
					for (int i = 0; i < activeSave->gameFlagVolatileByteCount; i++) {
						ImGui::PushID(i);

						DisplayFlagByteValues(&activeSave->GameFlagVolatileArr[i], idx, VOLATILE_GAME_FLAG_COUNT, VolatileGameFlagNames);
						idx += 8;

						ImGui::PopID();
					}
					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Game Counters")) {
					for (int i = 0; i < activeSave->gameCounterCount; i++) {
						ImGui::PushID(i);

						if (ImGui::TreeNode(GameCounterNames[i])) {
							ImGui::Text("ID %d", i);
							ImGui::InputScalar("Value", ImGuiDataType_U32, &activeSave->gameCountersArr[i]);
							if (strstr(GameCounterNames[i], "Timer")) {
								ImGui::SameLine();
								ImGui::Text("(%02d:%02d:%02d)", (activeSave->gameCountersArr[i] / 3600000) % 60, ((activeSave->gameCountersArr[i] / 1000) / 60) % 60, (activeSave->gameCountersArr[i] / 1000) % 60);
							}
							ImGui::TreePop();
						}

						ImGui::PopID();
					}
					ImGui::TreePop();
				}
				
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}
}

static void DisplayFlagByteValues(char* flag, int idx, int count, const char** nameArr) {
	bool flag1 = (*flag & 0x1) != 0;
	bool flag2 = (*flag & 0x2) != 0;
	bool flag3 = (*flag & 0x4) != 0;
	bool flag4 = (*flag & 0x8) != 0;
	bool flag5 = (*flag & 0x10) != 0;
	bool flag6 = (*flag & 0x20) != 0;
	bool flag7 = (*flag & 0x40) != 0;
	bool flag8 = (*flag & 0x80) != 0;

	char newFlag = 0;

	for (int k = 0; k < 8; k++) {
		if (idx + k >= count) break;

		if (ImGui::TreeNode(nameArr[idx + k])) {
			switch (k) {
			case 0:
				ImGui::Checkbox("Value", &flag1);
				break;
			case 1:
				ImGui::Checkbox("Value", &flag2);
				break;
			case 2:
				ImGui::Checkbox("Value", &flag3);
				break;
			case 3:
				ImGui::Checkbox("Value", &flag4);
				break;
			case 4:
				ImGui::Checkbox("Value", &flag5);
				break;
			case 5:
				ImGui::Checkbox("Value", &flag6);
				break;
			case 6:
				ImGui::Checkbox("Value", &flag7);
				break;
			case 7:
				ImGui::Checkbox("Value", &flag8);
				break;
			}

			ImGui::TreePop();
		}
	}

	newFlag = newFlag | flag1;
	newFlag = newFlag | flag2 << 1;
	newFlag = newFlag | flag3 << 2;
	newFlag = newFlag | flag4 << 3;
	newFlag = newFlag | flag5 << 4;
	newFlag = newFlag | flag6 << 5;
	newFlag = newFlag | flag7 << 6;
	newFlag = newFlag | flag8 << 7;

	if (*flag != newFlag) {
		printf("%02hhx\n", newFlag);
		*flag = newFlag;
	}
}

static void openLoadSaveFile() {
	imGuiWindowInfo.saveFilePath = new char[MAX_PATH];

	if (NFD_OpenDialog(&imGuiWindowInfo.saveFilePath, NULL, 0, NULL) == NFD_OKAY) {
		std::ifstream is(imGuiWindowInfo.saveFilePath, std::ifstream::binary);

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

		printf("Loading save from file \"%s\"\n", imGuiWindowInfo.saveFilePath);
		activeSave = new SaveData();
		activeSave->LoadSaveFile(fileData);

		free(fileData);
	}
	else {
	}
}

/// <summary>
/// Loads a font from the resource file and loads it into the ImGui font atlas.
/// </summary>
/// <param name="resourceName">The ID of the resource as is defined in the resource file.</param>
/// <param name="resourceType">The type of the resource.</param>
/// <param name="extraScale">Optional. Extra scaling to apply to the font if needed.</param>
/// <returns>A pointer to the created ImFont object.</returns>
static ImFont* LoadResourceFont(int resourceName, const wchar_t* resourceType, float extraScale = 1) {
	HRESULT hr = S_OK;

	// Resource management.
	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;
	unsigned char* pImageFile = NULL;
	DWORD imageFileSize = 0;

	// Locate the resource in the application's executable.
	imageResHandle = FindResource(
		NULL,             // This component.
		MAKEINTRESOURCE(resourceName),   // Resource name.
		resourceType);        // Resource type.

	hr = (imageResHandle ? S_OK : E_FAIL);

	// Load the resource to the HGLOBAL.
	if (SUCCEEDED(hr)) {
		imageResDataHandle = LoadResource(NULL, imageResHandle);
		hr = (imageResDataHandle ? S_OK : E_FAIL);
	}
	else {
		PRINT("Failed to find resource.\n");
	}

	// Lock the resource to retrieve memory pointer.
	if (SUCCEEDED(hr)) {
		pImageFile = (unsigned char*)LockResource(imageResDataHandle);
		hr = (pImageFile ? S_OK : E_FAIL);
	}
	else {
		PRINT("Failed to load resource.\n");
	}

	// Calculate the size.
	if (SUCCEEDED(hr)) {
		imageFileSize = SizeofResource(NULL, imageResHandle);
		hr = (imageFileSize ? S_OK : E_FAIL);
	}
	else {
		PRINT("Failed to lock resource.\n");
	}

	ImFontConfig cfg;
	cfg.ExtraSizeScale = extraScale; // Extra scaling is needed for the pick of font for Japanese and Korean characters (Noto Sans).
	cfg.MergeMode = true;

	PRINT("%d\n", (int)imageFileSize);

	void* fontFile = ImGui::MemAlloc((int)imageFileSize);
	memcpy(fontFile, pImageFile, imageFileSize);

	// Calculate the size.
	if (SUCCEEDED(hr)) {
		UnlockSegment(imageResDataHandle);
		FreeResource(imageResDataHandle);
	}
	else {
		PRINT("Failed to lock resource.\n");
	}

	return ImGui::GetIO().Fonts->AddFontFromMemoryTTF(fontFile, (int)imageFileSize, 0.f, &cfg);
}

static unsigned char* GetRawImageData_Base(char* data, int width, int height, int type) {
	int internalType = GL_UNSIGNED_BYTE;
	int format = GL_RGBA;
	int imageSize = 0;
	bool isFormatSupported = false;
	unsigned char* imageData = nullptr;

	if (type == XboxTexFormat::DXT1) {
		printf("Image Format: DXT1\n");
		printf("Image Size: %d\n", (width * height) * 4);

		imageData = new unsigned char[(width * height) * 4];

		squish::DecompressImage(imageData, width, height, data, squish::kDxt1);

		isFormatSupported = true;
	}
	if (type == XboxTexFormat::DXT3) {
		printf("Image Format: DXT3\n");
		printf("Image Size: %d\n", (width * height) * 4);

		imageData = new unsigned char[(width * height) * 4];

		squish::DecompressImage(imageData, width, height, data, squish::kDxt3);
		isFormatSupported = true;
	}
	if (type == XboxTexFormat::DXT5) {
		printf("Image Format: DXT5\n");
		printf("Image Size: %d\n", (width * height) * 4);

		imageData = new unsigned char[(width * height) * 4];

		squish::DecompressImage(imageData, width, height, data, squish::kDxt5);
		isFormatSupported = true;
	}
	if (type == XboxTexFormat::BGRA8888) {
		printf("Image Format: BGRA8888\n");
		printf("Image Size: %d\n", (width * height) * 4);

		imageData = new unsigned char[(width * height) * 4];
		memcpy(imageData, data, (width * height) * 4);

		format = GL_BGRA;
		isFormatSupported = true;
	}

	if (!isFormatSupported) {
		printf("Provided texture format (%d) for Ghoulies is not supported or has not been implemented.\n", type);
		return 0;
	}

	return imageData;
}

static unsigned char* GetRawImageData_Banjo(char* data, int width, int height, int type, int isSwizzled) {
	int internalType = GL_UNSIGNED_BYTE;
	int format = GL_RGBA;
	int imageSize = 0;
	bool isFormatSupported = false;
	unsigned char* imageData = nullptr;

	if (type == TEXTURE_FORMAT::TEX_DXT1) {
		printf("Image Format: DXT1\n");
		printf("Image Size: %d\n", (width * height) * 4);
		imageData = new unsigned char[(width * height) * 4];
		stbi__endian_swap(data, width, height, 2);

		if (isSwizzled) {
			unsigned char* linTex = ModifyLinearTexture((unsigned char*)data, width, height, type, true);
			squish::DecompressImage(imageData, width, height, linTex, squish::kDxt1);
		}
		else {
			squish::DecompressImage(imageData, width, height, data, squish::kDxt1);
		}

		isFormatSupported = true;
	}
	if (type == TEXTURE_FORMAT::TEX_DXT3) {
		printf("Image Format: DXT3\n");
		printf("Image Size: %d\n", (width * height) * 4);

		imageData = new unsigned char[(width * height) * 4];
		stbi__endian_swap(data, width, height, 2);

		if (isSwizzled) {
			unsigned char* linTex = ModifyLinearTexture((unsigned char*)data, width, height, type, true);
			squish::DecompressImage(imageData, width, height, linTex, squish::kDxt3);
		}
		else {
			squish::DecompressImage(imageData, width, height, data, squish::kDxt3);
		}

		isFormatSupported = true;
	}
	if (type == TEXTURE_FORMAT::TEX_DXT5) {
		printf("Image Format: DXT5\n");
		printf("Image Size: %d\n", (width * height) * 4);

		imageData = new unsigned char[(width * height) * 4];
		stbi__endian_swap(data, width, height, 2);

		if (isSwizzled) {
			unsigned char* linTex = ModifyLinearTexture((unsigned char*)data, width, height, type, true);
			squish::DecompressImage(imageData, width, height, linTex, squish::kDxt5);
		}
		else {
			squish::DecompressImage(imageData, width, height, data, squish::kDxt5);
		}

		isFormatSupported = true;
	}
	if (type == TEXTURE_FORMAT::TEX_ARGB8888) {
		
		printf("Image Size: %d\n", (width * height) * 4);

		if (isSwizzled) {
			imageData = ModifyLinearTexture((unsigned char*)data, width, height, type, true);
			stbi__abgr_to_rgba(imageData, width, height, 4);
		}
		else {
			imageData = new unsigned char[(width * height) * 4];
			memcpy(imageData, data, (width * height) * 4);
			stbi__argb_to_rgba(imageData, width, height, 4);
		}

		isFormatSupported = true;
	}

	if (!isFormatSupported) {
		printf("Provided texture format (%d) for Ghoulies is not supported or has not been implemented.\n", type);
		return 0;
	}

	return imageData;
}

static GLuint LoadImageFromData_Base(char* data, int width, int height, int type) {
	GLuint tex = -1;

	int internalType = GL_UNSIGNED_BYTE;
	int format = GL_RGBA;
	int imageSize = 0;
	bool isFormatSupported = false;
	unsigned char* imageData = nullptr;

	if (type == XboxTexFormat::DXT1) {
		printf("Image Format: DXT1\n");
		printf("Image Size: %d\n", (width * height) * 4);

		imageData = new unsigned char[(width * height) * 4];

		squish::DecompressImage(imageData, width, height, data, squish::kDxt1);

		isFormatSupported = true;
	}
	if (type == XboxTexFormat::DXT3) {
		printf("Image Format: DXT3\n");
		printf("Image Size: %d\n", (width * height) * 4);

		imageData = new unsigned char[(width * height) * 4];

		squish::DecompressImage(imageData, width, height, data, squish::kDxt3);
		isFormatSupported = true;
	}
	if (type == XboxTexFormat::DXT5) {
		printf("Image Format: DXT5\n");
		printf("Image Size: %d\n", (width * height) * 4);

		imageData = new unsigned char[(width * height) * 4];

		squish::DecompressImage(imageData, width, height, data, squish::kDxt5);
		isFormatSupported = true;
	}
	if (type == XboxTexFormat::BGRA8888) {
		printf("Image Format: BGRA8888\n");
		printf("Image Size: %d\n", (width * height) * 4);

		imageData = new unsigned char[(width * height) * 4];
		memcpy(imageData, data, (width * height) * 4);

		format = GL_BGRA;
		isFormatSupported = true;
	}

	if (!isFormatSupported) {
		printf("Provided texture format for Ghoulies is not supported or has not been implemented.\n");
		return 0;
	}

	tex = LoadImageFromData(imageData, width, height, format, internalType);

	return tex;
}

static GLuint LoadImageFromData_Pinata(char* data, int width, int height, int type) {
	GLuint tex = -1;

	int internalType = GL_UNSIGNED_BYTE;
	int internalFormat = GL_RGB;
	int imageSize = 0;
	unsigned char* imageData = nullptr;

	switch (type) {
	default:
		printf("An invalid image type has been passed.");
		return 0;
	case 0x1:
		printf("Image Size: %d", (width * height) * 4);

		imageData = new unsigned char[(width * height) * 4];

		squish::DecompressImage(imageData, width, height, data, squish::kDxt1);
		break;
	case 0x2:
		printf("Image Size: %d", (width * height) * 4);

		imageData = new unsigned char[(width * height) * 4];

		squish::DecompressImage(imageData, width, height, data, squish::kDxt3);
		break;
	case 13:
		printf("Image Size: %d", (width * height) * 4);

		imageData = new unsigned char[(width * height) * 4];

		squish::DecompressImage(imageData, width, height, data, squish::kDxt3);

		internalFormat = GL_RGBA8;
		break;
	case 0x3:
	case 12:
		printf("Image Size: %d", (width * height) * 4);

		imageData = new unsigned char[(width * height) * 4];

		squish::DecompressImage(imageData, width, height, data, squish::kDxt5);
		break;
	case 14:
		imageData = (unsigned char*)data;
		internalFormat = GL_RG8;
		break;
	}

	tex = LoadImageFromData(imageData, width, height, internalFormat, internalType);

	return tex;
}

static GLuint LoadImageFromData_Banjo(char* data, int width, int height, int type, int isSwizzled) {
	GLuint tex = -1;

	int internalType = GL_UNSIGNED_BYTE;
	int format = GL_RGBA;
	int imageSize = 0;
	bool isFormatSupported = false;
	unsigned char* imageData = nullptr;

	try {
		// The order needs to be Endian Swap -> Deswizzle -> Decompress or Adjust the Texture -> Load Texture.
		if (type == TEXTURE_FORMAT::TEX_DXT1) {
			PRINT("Image Format: DXT1\n");
			PRINT("Image Size: %d\n", (width * height) * 4);

			imageData = new unsigned char[(width * height) * 4];
			stbi__endian_swap(data, width, height, 2);

			if (isSwizzled) {
				unsigned char* linTex = ModifyLinearTexture((unsigned char*)data, width, height, type, true);
				squish::DecompressImage(imageData, width, height, linTex, squish::kDxt1);
			}
			else {
				squish::DecompressImage(imageData, width, height, data, squish::kDxt1);
			}

			format = GL_RGBA;
			isFormatSupported = true;
		}
		if (type == TEXTURE_FORMAT::TEX_DXT3) {
			PRINT("Image Format: DXT3\n");
			PRINT("Image Size: %d\n", (width * height) * 4);

			imageData = new unsigned char[(width * height) * 4];
			stbi__endian_swap(data, width, height, 2);

			if (isSwizzled) {
				unsigned char* linTex = ModifyLinearTexture((unsigned char*)data, width, height, type, true);
				squish::DecompressImage(imageData, width, height, linTex, squish::kDxt3);
			}
			else {
				squish::DecompressImage(imageData, width, height, data, squish::kDxt3);
			}

			isFormatSupported = true;
		}
		if (type == TEXTURE_FORMAT::TEX_DXT5) {
			PRINT("Image Format: DXT5\n");
			PRINT("Image Size: %d\n", (width * height) * 4);

			imageData = new unsigned char[(width * height) * 4];
			stbi__endian_swap(data, width, height, 2);

			if (isSwizzled) {
				unsigned char* linTex = ModifyLinearTexture((unsigned char*)data, width, height, type, true);
				squish::DecompressImage(imageData, width, height, linTex, squish::kDxt5);
			}
			else {
				squish::DecompressImage(imageData, width, height, data, squish::kDxt5);
			}

			isFormatSupported = true;
		}
		if (type == TEXTURE_FORMAT::TEX_ARGB8888) {
			PRINT("Image Format: ARGB8888\n");
			PRINT("Image Size: %d\n", (width * height) * 4);

			if (isSwizzled) {
				imageData = ModifyLinearTexture((unsigned char*)data, width, height, type, true);
			}
			else {
				imageData = new unsigned char[(width * height) * 4];
				memcpy(imageData, data, (width * height) * 4);
			}

			stbi__argb_to_rgba(imageData, width, height, 4);

			format = GL_BGRA;
			isFormatSupported = true;
		}

		if (!isFormatSupported) {
			PRINT("Provided texture format for Banjo-Kazooie: Nuts & Bolts is not supported or has not been implemented.\n");
			return 0;
		}

		tex = LoadImageFromData(imageData, width, height, format, internalType);
	}
	catch (int err) {
		PRINT("An error occured while processing the texture. Error code %d.\n", err);
	}

	return tex;
}

static GLuint LoadImageFromData(unsigned char* data, int width, int height,int format, int type) {
	GLuint tex = -1;

	try {
		glGenTextures(1, &tex);

		glBindTexture(GL_TEXTURE_2D, tex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, data);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	catch (int err){
		PRINT("An error occured while creating/binding the texture. Error code %d.\n", err);
	}

	return tex;
}

/// <summary>
/// Loads an image from the resource file and loads it for use.
/// </summary>
/// <param name="resourceName"></param>
/// <param name="resourceType"></param>
/// <returns>If successful, the target of the texture.</returns>
static GLuint LoadResourceImage(int resourceName, const wchar_t* resourceType) {
	HRESULT hr = S_OK;

	// Resource management.
	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;
	unsigned char* pImageFile = NULL;
	DWORD imageFileSize = 0;

	// Locate the resource in the application's executable.
	imageResHandle = FindResource(
		NULL,             // This component.
		MAKEINTRESOURCE(resourceName),   // Resource name.
		resourceType);        // Resource type.

	hr = (imageResHandle ? S_OK : E_FAIL);

	// Load the resource to the HGLOBAL.
	if (SUCCEEDED(hr)) {
		imageResDataHandle = LoadResource(NULL, imageResHandle);
		hr = (imageResDataHandle ? S_OK : E_FAIL);
	}

	// Lock the resource to retrieve memory pointer.
	if (SUCCEEDED(hr)) {
		pImageFile = (unsigned char*)LockResource(imageResDataHandle);
		hr = (pImageFile ? S_OK : E_FAIL);
	}

	// Calculate the size.
	if (SUCCEEDED(hr)) {
		imageFileSize = SizeofResource(NULL, imageResHandle);
		hr = (imageFileSize ? S_OK : E_FAIL);
	}

	GLuint tex;
	int w;
	int h;
	int comp;
	unsigned char* image = stbi_load_from_memory(pImageFile, imageFileSize, &w, &h, &comp, STBI_rgb_alpha);

	if (image == nullptr)
		throw(std::string("Failed to load texture"));

	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(image);

	return tex;
}

static GLFWimage LoadResourceImageToGLFWImage(int resourceName, const wchar_t* resourceType) {
	HRESULT hr = S_OK;

	// Resource management.
	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;
	unsigned char* pImageFile = NULL;
	DWORD imageFileSize = 0;

	// Locate the resource in the application's executable.
	imageResHandle = FindResource(
		NULL,             // This component.
		MAKEINTRESOURCE(resourceName),   // Resource name.
		resourceType);        // Resource type.

	hr = (imageResHandle ? S_OK : E_FAIL);

	// Load the resource to the HGLOBAL.
	if (SUCCEEDED(hr)) {
		imageResDataHandle = LoadResource(NULL, imageResHandle);
		hr = (imageResDataHandle ? S_OK : E_FAIL);
	}

	// Lock the resource to retrieve memory pointer.
	if (SUCCEEDED(hr)) {
		pImageFile = (unsigned char*)LockResource(imageResDataHandle);
		hr = (pImageFile ? S_OK : E_FAIL);
	}

	// Calculate the size.
	if (SUCCEEDED(hr)) {
		imageFileSize = SizeofResource(NULL, imageResHandle);
		hr = (imageFileSize ? S_OK : E_FAIL);
	}

	GLuint tex;
	int w;
	int h;
	int comp;
	unsigned char* image = stbi_load_from_memory(pImageFile, imageFileSize, &w, &h, &comp, STBI_rgb_alpha);

	if (image == nullptr)
		throw(std::string("Failed to load texture"));

	GLFWimage img;
	img.height = h;
	img.width = w;
	img.pixels = image;

	return img;
}

// GLFW
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	// No clue why this was here.
	/*if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);*/
}