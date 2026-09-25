#pragma once

//Ghoulies Stuff
static void openGhouliesDemand();
static void openGhouliesBundle();
void displayGhoulDemandInfo();
void displayGhoulBundleInfo();
void displayActiveGhoulDemandProperty();
void fillGhouliesBundleFileList();
void displayActiveGhoulBundleProperty();
void ReadGhoulBundleTexture();
void ReadGhoulDemandTexture();

//h
void countTimeToLoad();
void ReadConkerLiveReloadedTexture(char* gpuData);

//CAFF Stuff
void readCaffFile();
void writeCaffFile(const char* fileName);
void writeStreamBundleFile(const char* fileName);
void disposeAndCloseActiveFile();
void writeDataToFile(const char* fileName, const char* filter, char* data, size_t dataSize);

void exportFilesFromBundleRaw(int fileCount);
void exportFilesFromBundleSpecial(int fileCount);

static void openPinataDbBundle();
static void openPinataPKGBundle();
void fillPinataDbBundleFileList();
void fillPinataPKGFileList();
void displayActivePinataDbBundleFileProperty();
void displayActivePinataPKGFileProperty();
void displayPinataDbFileInfo();
void displayPinataPkgInfo();
static unsigned char* GetRawImageData_Pinata(char* data, int32_t width, int32_t height, int32_t type);

// RR RPK Stuff
static void openRareRPKFile();
void displayRPKInfo();
void fillRPKFileList();
void displayActiveRPKFileProperty();

void readFolderGroup();
void fillFolderGroupList();

//Image Functions
void ExportXenonTextures(char* exportPath, char* baseLabel);
static GLuint LoadResourceImage(int32_t  resourceName, const wchar_t* resourceType);
static GLuint LoadImageFromData_Base(char* data, int32_t  width, int32_t  height, int32_t  type);
static GLuint LoadImageFromData_Pinata(char* data, int32_t  width, int32_t  height, int32_t  type);
static GLuint LoadImageFromData_Banjo(char* data, int32_t  width, int32_t  height, int32_t  type, int32_t  isSwizzled);
static GLuint LoadImageFromData(unsigned char* data, int32_t  width, int32_t  height, int32_t  format, int32_t  type);
static GLFWimage LoadResourceImageToGLFWImage(int32_t  resourceName, const wchar_t* resourceType);
static unsigned char* GetRawImageData_Base(char* data, int32_t  width, int32_t  height, int32_t  type);
static unsigned char* GetRawImageData_Xenon(char* data, int32_t  width, int32_t  height, int32_t  type);

static ImFont* LoadResourceFont(int32_t  resourceName, const wchar_t* resourceType, float extraSize);

int32_t  mainWindowCode();
void drawWindow();
void readOtherSupportedFile(int32_t  type);
void handleSavingAsync(const char* fileName);

//Base Window
void buildBaseImGuiWindow();
void buildMainWindow();
void buildTitleBar();
void displayFileInfo(float barHeight);
void displayProperties(float barHeight);
void displayBundleInfo();
void displayBundleV40Info();
void displayBundleV31Info();
void displayStreamBundleInfo();
void displayActiveFileProperty();
void displayActiveV40BundleProperty();
void displayActiveBundleV26Property();
void displayActiveBundleV31Property();
void displayActiveStreamBundleFileProperty();
void displayAvailableFilesList(float barHeight);
void fillBundleFileList();
void fillKameoDBFileList();
void fillStreamBundleFileList();
void fillBundleV26FileList();
void fillBundleV31FileList();
void fillBundleV40FileList();
static void ShowMenuFile();
static void openMenu();
static void ShowSearchMenu();
void TestBundleRecompilation();
void readLoctextFile(char* data, int32_t  startEndian);
void readMarkerFile(char* data);

//Save Editor
static void DisplaySaveEditorBaseWindow();

static void DisplayFlagByteValues(char* flag, int32_t  idx, int32_t  count, const char** nameArr);

// Open Functions
static void openLoadSaveFile();


struct BufferedSave {
	char* savedData;
	int32_t  fileId;
	int32_t  dataSize;
	int32_t  sect;
};

struct BundleSave {

};

struct StreamedBundleSetup {
	BundleSave* bufferedSaves;
	int32_t  totalBufferedSavesCount;
	int32_t * modifiedFileBufferIDs;
	int32_t  modifiedFilesCount;
	bool isDirty;
};

struct BundleSetup {
public:
	std::vector<BufferedSave> bufferedSaves;
	int32_t  totalBufferedSavesCount;
	std::vector<int32_t> modifiedFileBufferIDs;
	int32_t  modifiedFilesCount;
	bool isDirty;

	bool haveNewFilesBeenAdded;
	int32_t  newFilesCount;
	std::vector<int32_t> newFileBufferIDs;

	bool bundleCompression;

	bool doesBufferedSaveExist(int32_t  fileIdx, int32_t  sect) {
		for (int32_t  i = 0; i < totalBufferedSavesCount; i++) {
			if (bufferedSaves[i].fileId == fileIdx && bufferedSaves[i].sect == sect) {
				return true;
			}
		}
		return false;
	}

	int32_t  getIdOfBufferedSave(int32_t  fileIdx, int32_t  sect) {
		for (int32_t  i = 0; i < totalBufferedSavesCount; i++) {
			if (bufferedSaves[i].fileId == fileIdx && bufferedSaves[i].sect == sect) {
				return i;
			}
		}
		return -1;
	}

	void AddToSaveBuffer(BufferedSave itemToSave) {
		isDirty = true;

		totalBufferedSavesCount++;
		bufferedSaves.push_back(itemToSave);
		printf("Item of ID %d in Section %d added to the save buffer.\n", itemToSave.fileId, itemToSave.sect);
		printf("Save Buffer now contains %d values.\n", totalBufferedSavesCount);
	}

	void AddNewFileToSaveBuffer(BufferedSave itemToSave) {
		isDirty = true;

		totalBufferedSavesCount++;
		newFilesCount++;

		bufferedSaves.push_back(itemToSave);
		newFileBufferIDs.push_back(totalBufferedSavesCount - 1);
		
		printf("New file with ID %d in Section %d added to the save buffer.\n", itemToSave.fileId, itemToSave.sect);
		printf("Save Buffer now contains %d values.\n", totalBufferedSavesCount);
	}
};

struct LoadingWindow {
public:
	std::thread saveThread;
	std::thread loadThread;

	bool showLoadingPrompt = false;
	char loadingMessage[1024] = { 0 };
	int32_t  totalAmount = 0;
	int32_t  currentSaved = 0;

	int32_t  loadWheel = 0;

	int32_t  targetType = 0;
};

struct AddFileWindow {
public:
	char filePath[MAX_PATH];
	char* fileName = nullptr;

	int32_t  fileType = 0;
};

struct ImGuiGarageWindow {

	// Bools
	bool showFileInfo = true;
	bool showSearch = false;

	// Editor Window Bool
	bool showScriptEditor = false;
	bool showMarkerEditor = false;
	bool showSaveEditor = false;

	LoadingWindow saveData;
	AddFileWindow addFileData;

	bool addRefToStreamed = false;
	bool addFileToBundle = false;

	bool db_ShowImGuiDemo = false;
	bool db_showMetrics = false;

	// Values
	float titleBarHeight = 20;

	int32_t  bundleSelectedItem = -1;
	int32_t  streamBundleSelectedBundle = -1;
	int32_t  streamBundleSelectedItem = -1;
	float storedScrollY = 0;

	ImFont* defFont;
	ImFont* jpnFont;
	ImFont* chnFont;
	ImFont* korFont;
	ImFont* rusFont;

	nfdchar_t* saveFilePath;
	nfdchar_t* loctextExportPath;
	char* search;
	uint32_t  aidSearch = 0;
	char* fileNameSearch;
};

//GLFW
void framebuffer_size_callback(GLFWwindow* window, int32_t  width, int32_t  height);
void window_refresh_callback(GLFWwindow* window);
void processInput(GLFWwindow* window);