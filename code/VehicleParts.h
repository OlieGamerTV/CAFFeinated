#pragma once

// Part Color Presets
const uint32_t COL_BLUE = 0xFFC77B4E;
const uint32_t COL_PURPLE = 0xFFCA68AF;
const uint32_t COL_MAGENTA = 0xFFBF6BC9;
const uint32_t COL_PEACH = 0xFFB1CBE4;
const uint32_t COL_ORANGE = 0xFF079AE0;
const uint32_t COL_GREENBLUE = 0xFF498000;
const uint32_t COL_PINK = 0xFFA065CD;
const uint32_t COL_BLACK = 0xFF303030;
const uint32_t COL_SILVER = 0xFFEBEBEB;
const uint32_t COL_LIME = 0xFF17E378;
const uint32_t COL_AQUA = 0xFFB7AF59;
const uint32_t COL_YELLOW = 0xFF1AFFFA;
const uint32_t COL_RED = 0xFF0309D1;
const uint32_t COL_BROWN = 0xFF004080;

const uint32_t COL_ARR[] = {
	COL_BLUE,
	COL_PURPLE,
	COL_PEACH,
	COL_MAGENTA,
	COL_ORANGE,
	COL_GREENBLUE,
	COL_PINK,
	COL_BLACK,
	COL_SILVER,
	COL_LIME,
	COL_AQUA,
	COL_YELLOW,
	COL_RED,
	COL_BROWN
};

const char* COLNAME_ARR[] = {
	"Blue",
	"Purple",
	"Peach",
	"Magenta",
	"Orange",
	"Greeny-Blue",
	"Pink",
	"Black",
	"Silver",
	"Green",
	"Aqua",
	"Yellow",
	"Red",
	"Brown"
};

uint32_t GetColorMatch(uint32_t col) {
	uint32_t doesColMatch = 0;
	size_t arrSize = sizeof(COL_ARR) / sizeof(COL_ARR[0]);

	for (int32_t i = 0; i < arrSize; i++) {
		if (COL_ARR[i] == col) {
			col = COL_ARR[i];
			doesColMatch = 1;
		}
	}

	return doesColMatch;
}

uint32_t GetDefaultColor(uint32_t col) {
	uint32_t doesColMatch = COL_BLUE;
	size_t arrSize = sizeof(COL_ARR) / sizeof(COL_ARR[0]);

	for (int32_t i = 0; i < arrSize; i++) {
		if (COL_ARR[i] == col) col = COL_ARR[i];
	}

	return doesColMatch;
}

int32_t GetColIdx(uint32_t col) {
	size_t arrSize = sizeof(COL_ARR) / sizeof(COL_ARR[0]);

	for (int32_t i = 0; i < arrSize; i++) {
		if (COL_ARR[i] == col) return i;
	}

	return 0;
}

static size_t getCountOfColorTable() {
	return sizeof(COL_ARR) / sizeof(COL_ARR[0]);
}

struct VehiclePartEntry {
	uint32_t aid;
	const char* vehicleName;

	bool isComment = false;
};

const VehiclePartEntry vehicleAssignments[] = {
	{0x0, "None"},

	// Propellers
	{0xFFFFFFFF, "Propellers", true},
	{0x1FC071F0, "Large Folding Propeller"},

	//Wings
    { 0xFFFFFFFF, "Wings", true },
    { 0x1F2CAD9B, "Folding Wings" },

    // Hover / Float
    { 0xFFFFFFFF, "Hover / Float", true },
    { 0x1F079766, "Balloon" },
    { 0x1F978E81, "Floater" },

	// Gadgets
	{0xFFFFFFFF, "Gadgets", true},
	{0x1F74119D, "Gyroscope"},
	{0x1F72D497, "Spotlight"},
	{0x1F0002C7, "Spec O Spy"},
	{0x1F583430, "Self Destruct"},
	{0x1F41AAC0, "Liquid Squirter"},
	{0x1FB56B94, "Spoiler"},
	{0x1F6EFC34, "Suck N Blow"},
	{ 0x1FBE5755, "Sticky Ball" },
	{ 0x1FB27042, "Vacuum" },
	{ 0x1F8517A9, "Spring" },
	{ 0x1FFEC0B5, "Tow Bar" },
	{ 0x1F10C777, "Detacher" },
	{ 0x1F3BA23B, "Ejector Seat" },
	{ 0x1F029162, "Horn" },
	{ 0x1F63BDE6, "Chameleon" },
	{ 0x1F5591E5, "Smoke Sphere" },

	// Weapons
	{ 0xFFFFFFFF, "Ammo Free Weapons", true },
	{ 0x1F37BB7E, "Boot in a Box" },

	{ 0xFFFFFFFF, "Uses Ammo Weapons", true },
	{ 0x1FEF20AD, "Egg Gun" },
	{ 0x1F9BF8B5, "Grenade Gun" },
	{ 0x1F8B1BB8, "Egg Turret" },
	{ 0x1F37131C, "Grenade Turret" },
	{ 0x1F31EFAB, "Torpedo" },
	{ 0x1F3E8E01, "Mumbo Bombo" },
	{ 0x1F603CC3, "Clockwork Kaz" },
	{ 0x1FC597FB, "Rust Bin" },
	{ 0x1F5D961C, "Citrus Slick" },
	{ 0x1F31CA4C, "EMP" },
	{ 0x1F54E704, "Freezeezy"},
	{ 0x1F506D0F, "Weldar's Breath"},
	{ 0x1F7CFEF4, "Laser" },

	{ 0xFFFFFFFF, "Misc.", true },
	{ 0x1F97C327, "Stereo" },

	{ 0xFFFFFFFF, "Additional Components", true },

	// Scrapped Components
	{ 0xFFFFFFFF, "Scrapped Gadgets", true },
	{ 0x1F6889CD, "Autopilot"},
	{ 0x1FF35BB8, "Remote Control" },
	{ 0x1FFD800C, "Blower" },
	{ 0x1F6D416D, "Sucker" },
};

const VehiclePartEntry vehicleParts[] = {
	// Driver Seats
	{0xFFFFFFFF, "Driver Seats", true},
	{0x1FEA444A, "Standard Seat"},
	{0x1FD1984E, "Strong Seat"},
	{0x1F123ED4, "Scuba Seat"},
	{0x1FF03006, "Super Seat"},

	// Passenger Seats
	{0xFFFFFFFF, "Passenger Seats", true},
	{0x1FD199A6, "Small Taxi Seat"},
	{0x1FFF3B45, "Large Taxi Seat"},

	// Wheels
	{0xFFFFFFFF, "Wheels", true},
	{0x1F09D39A, "Standard Wheel"},
	{0x1FF1B874, "High Grip Wheel"},
	{0x1F95D087, "Monster Wheel"},
	{0x1F5F9098, "Super Wheel"},

	// Engine
	{0xFFFFFFFF, "Engines", true},
	{ 0x1F658ECD, "Small Engine" },
	{ 0x1F636B72, "Medium Engine" },
	{ 0x1FC89446, "Large Engine" },
	{ 0x1F287929, "Super Engine" },

		// Jets
	{ 0xFFFFFFFF, "Jets", true },
	{ 0x1F207106, "Small Jet" },
	{ 0x1F0EB2AB, "Large Jet" },

		// Fuel
	{ 0xFFFFFFFF, "Fuel", true },
	{ 0x1F84FD7F, "Small Fuel" },
	{ 0x1FFC8DF8, "Medium Fuel" },
	{ 0x1FAE2C77, "Large Fuel" },
	{ 0x1FFFCF5F, "Super Fuel" },

	// Ammunition
	{ 0xFFFFFFFF, "Ammo", true },
	{ 0x1F85D4CE, "Small Ammo" },
	{ 0x1FE80FCB, "Medium Ammo" },
	{ 0x1FAF05C6, "Large Ammo" },
	{ 0x1FFEE6EE, "Super Ammo" },

		// Propellers
	{ 0xFFFFFFFF, "Propellers", true },
	{ 0x1F2AE2B8, "Small Propeller" },
	{ 0x1F042115, "Large Propeller" },
	{ 0x1FC071F0, "Large Folding Propeller" },

		//Wings
	{ 0xFFFFFFFF, "Wings", true },
	{ 0x1FFC7505, "Standard Wings" },
	{ 0x1F2CAD9B, "Folding Wings" },

	// Hover / Float
	{ 0xFFFFFFFF, "Hover / Float", true },
	{ 0x1FF7BB6C, "Air Cushion" },
	{ 0x1F079766, "Balloon" },
	{ 0x1F978E81, "Floater" },
	{ 0x1F8D1F16, "Sinker" },

		//Trays
	{ 0xFFFFFFFF, "Containers", true },
	{ 0x1F127A1B, "Tray" },
	{ 0x1F29AE62, "Large Tray" },
	{ 0x1FA0396E, "Box" },
	{ 0x1FB798FD, "Large Box" },

		// Light Blocks
	{ 0xFFFFFFFF, "Light Blocks", true },
	{ 0x1F4BB5E1, "Light Cube" },
	{ 0x1FCC7461, "Light Wedge" },
	{ 0x1FF3E64A, "Light Corner" },
	{ 0x1FD546B8, "Light Pole" },
	{ 0x1FD61511, "Light L Pole" },
	{ 0x1F0A9F36, "Light T Pole" },
	{ 0x1FF360D6, "Light Panel" },
	{ 0x1F4178C9, "Light L Panel" },
	{ 0x1F331128, "Light T Panel" },

		// Heavy Blocks
	{ 0xFFFFFFFF, "Heavy Blocks", true },
	{ 0x1F841B45, "Heavy Cube" },
	{ 0x1FD5DC3E, "Heavy Wedge" },
	{ 0x1FF6B387, "Heavy Corner" },
	{ 0x1F1AE81C, "Heavy Pole" },
	{ 0x1F1C34C3, "Heavy L Pole" },
	{ 0x1F133769, "Heavy T Pole" },
	{ 0x1FEAC889, "Heavy Panel" },
	{ 0x1F4F0110, "Heavy L Panel" },
	{ 0x1F3644E5, "Heavy T Panel" },

		// Super Blocks
	{ 0xFFFFFFFF, "Super Blocks", true },
	{ 0x1FEA640E, "Super Cube" },
	{ 0x1F242A59, "Super Wedge" },
	{ 0x1FC1B68A, "Super Corner" },
	{ 0x1F749757, "Super Pole" },
	{ 0x1F51E31C, "Super L Pole" },
	{ 0x1FE2C10E, "Super T Pole" },
	{ 0x1F1B3EEE, "Super Panel" },
	{ 0x1FB58382, "Super L Panel" },
	{ 0x1F0141E8, "Super T Panel" },

		// Gadgets
	{ 0xFFFFFFFF, "Gadgets", true },
	{ 0x1FD1698C, "Aerial" },
	{ 0x1F74119D, "Gyroscope" },
	{ 0x1F72D497, "Spotlight" },
	{ 0x1F0002C7, "Spec O Spy" },
	{ 0x1F583430, "Self Destruct" },
	{ 0x1F41AAC0, "Liquid Squirter" },
	{ 0x1FB56B94, "Spoiler" },
	{ 0x1F6EFC34, "Suck N Blow" },
	{ 0x1FBE5755, "Sticky Ball" },
	{ 0x1FB27042, "Vacuum" },
	{ 0x1F8517A9, "Spring" },
	{ 0x1FFEC0B5, "Tow Bar" },
	{ 0x1F10C777, "Detacher" },
	{ 0x1F3BA23B, "Ejector Seat" },
	{ 0x1F4E68C7, "Robo-Fix" },
	{ 0x1F982C42, "Replenisher" },
	{ 0x1F029162, "Horn" },
	{ 0x1F63BDE6, "Chameleon" },
	{ 0x1F5591E5, "Smoke Sphere" },

	{ 0xFFFFFFFF, "Protection", true },
	{ 0x1F8DF274, "Bumper" },
	{ 0x1F2BF215, "Armor" },
	{ 0x1F953740, "Energy Shield" },

		// Weapons
	{ 0xFFFFFFFF, "Ammo Free Weapons", true },
	{ 0x1F4BC61E, "Fulgore's Fist" },
	{ 0x1F37BB7E, "Boot in a Box" },
	{ 0x1F5B8D3C, "Spike" },

	{ 0xFFFFFFFF, "Uses Ammo Weapons", true },
	{ 0x1FEF20AD, "Egg Gun" },
	{ 0x1F9BF8B5, "Grenade Gun" },
	{ 0x1F8B1BB8, "Egg Turret" },
	{ 0x1F37131C, "Grenade Turret" },
	{ 0x1F31EFAB, "Torpedo" },
	{ 0x1F3E8E01, "Mumbo Bombo" },
	{ 0x1F603CC3, "Clockwork Kaz" },
	{ 0x1FC597FB, "Rust Bin" },
	{ 0x1F5D961C, "Citrus Slick" },
	{ 0x1F31CA4C, "EMP" },
	{ 0x1F54E704, "Freezeezy"},
	{ 0x1F506D0F, "Weldar's Breath"},
	{ 0x1F7CFEF4, "Laser" },

	{ 0xFFFFFFFF, "Misc.", true },
	{ 0x1F585A51, "Crusin' Light" },
	{ 0x1F2E95E7, "Plant Pot" },
	{ 0x1F646AE2, "Spirit Of Pants" },
	{ 0x1F130448, "Windscreen" },
	{ 0x1F1064AF, "Mirror" },
	{ 0x1F0FD73C, "Tag Plate" },
	{ 0x1F97C327, "Stereo" },
	{ 0x1F98FC81, "Papery Pal" },

	// Stop 'n' Swop Items
	{ 0xFFFFFFFF, "SnS Items", true},
	{0x1FBBAF2A, "Flag"},
	{0x1F0C59FD, "Mole on a Pole"},
	{0x1FBEA986, "Fluffy Dice"},
	{0x1FE338DD, "Goldfish"},
	{0x1FC6C417, "Beacon"},
	{0x1F817BC9, "Disco Ball"},
	{0x1FC180AF, "Googly Eyes"},

	{ 0xFFFFFFFF, "Additional Components", true },

	{ 0xFFFFFFFF, "NPC Exclusive", true },
	{ 0x1F62B405, "Grunty Secondary Seat" },
	{ 0x1F780CED, "Grunty Secondary Seat (Watertight)" },
	{ 0x1F7056FB, "Floater (Grunty)" },
	{ 0x1FAA7271, "Pikelet Secondary Seat" },
	{ 0x1F4633B3, "Pikelet Passenger Seat" },

	{ 0xFFFFFFFF, "AI Exclusive", true },
	{ 0x1F941C9B, "Small Engine (AI)" },
	{ 0x1FBC6E21, "Medium Engine (AI)" },
	{ 0x1F390610, "Large Engine (AI)" },
	{ 0x1FD9EB7F, "Super Engine (AI)" },
	{ 0x1F9E77F8, "Spring (AI)" },
	{ 0x1F33AF00, "Small Jet (AI)" },
	{ 0x1F1D6CAD, "Large Jet (AI)" },

	{ 0xFFFFFFFF, "Challenge Exclusive", true },
	{ 0x1FEB371C, "LogOlympic Torch" },
	{ 0x1FBB5062, "Gameplay Creator Part A" },
	{ 0x1F4D875E, "Gameplay Creator Part B" },
	{ 0x1FBC60F4, "Gameplay Creator Part C" },
	{ 0x1F6885C3, "Gameplay Creator Locator" },
	{ 0x1FB94FC8, "Tray (Egg 'n Spoon Race)" },
	{ 0x1FB5EC7B, "Fixed Egg Turret" }, // Only used for "The Saucer of Peril Returns".

	// Scrapped Components
	{ 0xFFFFFFFF, "Scrapped Gadgets", true },
	{ 0x1F6889CD, "Autopilot"},
	{ 0x1FF35BB8, "Remote Control" },
	{ 0x1FFD800C, "Blower" },
	{ 0x1F6D416D, "Sucker" },

	{ 0xFFFFFFFF, "Misc.", true },
	{ 0x1F7A892D, "Standard Seat (Cutscene)" },
	{ 0x1FA336A3, "Energy Shield (No Big Hits)" },
	{ 0x1F4FA2EC, "Spotlight (Always On)" },
	{ 0x1F73F0D7, "Spring (Trolley)" },
	{ 0x1F80AC26, "Dummy Propeller" },
	{ 0x1F948B1B, "Heavy High Grip Wheel" },

	// Unknown
	{ 0xFFFFFFFF, "Unknown Elements", true },
	{ 0x1FC39E27, "Leak Point" },
	{ 0x1F8A6690, "Attach Point" },
	{ 0x1F48B278, "Small Passenger Seat" },
	{ 0x1F6671D5, "Large Passenger Seat" },
};

static size_t getCountOfAssignmentTable() {
	return sizeof(vehicleAssignments) / sizeof(VehiclePartEntry);
}

static size_t getCountOfPartsTable() {
	return sizeof(vehicleParts) / sizeof(VehiclePartEntry);
}

static const char* getPartNameFromAid(uint32_t aid) {
	size_t arrSize = sizeof(vehicleParts) / sizeof(VehiclePartEntry);

	for (int32_t i = 0; i < arrSize; i++) {
		if (vehicleParts[i].aid == aid) return vehicleParts[i].vehicleName;
	}

	return "Unknown / Not Implemented";
}