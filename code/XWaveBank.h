#pragma once

// more or less a stub struct just so that I have somewhere to put the name.
struct DNBW {
public:
	char bankName[0x50];

	bool ReadBankFile(char* data);
};