#pragma once
#include <cstdint>

namespace Pinata {
	static const int32_t PKG_VER1 = 1;
	static const int32_t PKG_VER2 = 2;

	struct PKGFileEntry {
		int32_t id;
		int32_t offset;
		int32_t size;
	};

	struct PKGFileHeader {
		int32_t version;
		int32_t entryCount;
	};

	struct PKGFile {
		PKGFileHeader header;
		PKGFileEntry* entries;

		//BundleV36* caffEntries;

		bool isReady = false;

		bool readStandaloneDbBundleFiles(char* filePath);
		bool readDbBundleFiles(char* fileData);
	};
}