#pragma once
#ifndef _DIFFBUNDLE
#define _DIFFBUNDLE
#include <cstdint>

struct DiffV37FileHeader {
	char identifier[4];
	char patchVersion[16];
	char sectionVersion[16];
	int32_t fileSize;
	int32_t compressedSize;
	int32_t numDiffs;
	int32_t stringTableLen;
	int32_t diffDataLen;
	char numSrcSectionTypes;
	char bigEndian;
	char compressed;
};

#endif