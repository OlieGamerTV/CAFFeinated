#pragma once

struct assetId_s {
public:
	unsigned int aid; // Used from Nuts & Bolts onwards.
	char text[0x80]; // Used for games prior to Nuts & Bolts.
};