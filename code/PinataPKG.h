#pragma once
#ifndef _PINATAPKG
#define _PINATAPKG
#include <cstdint>
#include "Bundle.h"

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

		char* data = nullptr;

		BundleV36* caffEntries;

		bool isBigEndian = false;
		bool isDirty = false;

		bool readStandalonePkgFile(char* filePath);
		bool readDbBundleFiles(char* fileData);

		char* GetFileData(int32_t fileIdx);

		void writePinataPKG(const char* outPath);

		void ClearActiveData();
	};
}
#endif