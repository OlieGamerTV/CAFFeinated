#pragma once

enum XboxTexFormat : int {
	DXT1 = 0xC,
	DXT3 = 0xE,
	DXT5 = 0xF,
	BGRA8888 = 0x12,
};

static char* GetXboxTextureFormatName(int format) {
	char* formatName = new char[12];

	switch (format) {
	case XboxTexFormat::DXT1:
		strcpy(formatName, "DXT1");
		break;
	case XboxTexFormat::DXT3:
		strcpy(formatName, "DXT3");
		break;
	case XboxTexFormat::DXT5:
		strcpy(formatName, "DXT5");
		break;
		break;
	case XboxTexFormat::BGRA8888:
		strcpy(formatName, "BGRA8888");
		break;
	}

	return formatName;
}