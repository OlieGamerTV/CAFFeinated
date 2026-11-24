#ifndef _INCUBATIONFORMATS
#define _INCUBATIONFORMATS
#include <cstdint>
#include <vector>

#ifdef _WIN32
#pragma once
#endif

namespace Incubation {
	static const char* AssetInventoryVERSION = "version,2\n";
	static const char* AssetInventoryDEVROOT = "devroot,GAME:\\cdx\\\n";
	static const char* AssetInventoryDEVROOT_SPORTS = "devroot,DEVKIT:\\cdx\\Olympus\\AM";
	static const char* AssetInventoryTABLEHEADER = "AMID,VERSION,TIMESTAMP,FILEPATH,BINARY FORMAT VERSION\n";
	static const char* AssetInventoryTABLEENTRYFORMAT = "%s,%d,%llu,%s,%.2f\n";

	struct AssetInventoryEntry {
	public:
		int32_t amidOffs = 0;
		int32_t filePathOffs = 0;
		int32_t amidVersion = 0;
		float binaryFormatVersion = 0;
		int64_t timeStamp = 0;
	};

	struct AssetInventoryStringEntry {
	public:
		char* amid = nullptr;
		char* filePath = nullptr;
	};

	struct AssetInventory {
	public:
		char* fileData = nullptr;

		int32_t entryCount;
		std::vector<AssetInventoryEntry> entries;
		std::vector<AssetInventoryStringEntry> nameEntries;

		void ReadStandaloneFile(char* fileName);
		void ReadFromData(char* data);

		void ExportToFile(char* fileName);
	};
}

#endif