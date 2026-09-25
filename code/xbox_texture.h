#pragma once

enum XboxTexFormat : int {
	DXT1 = 0xC,
	DXT3 = 0xE,
	DXT5 = 0xF,
	B8G8R8A8 = 0x12,
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
	case XboxTexFormat::B8G8R8A8:
		strcpy(formatName, "B8G8R8A8");
		break;
	}

	return formatName;
}