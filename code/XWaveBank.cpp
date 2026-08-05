#include "CommonReader.h"
#include "XWaveBank.h"

bool DNBW::ReadBankFile(char* data) {
	if (data == nullptr) {
		printf("Passed data array is null.\n");
		return false;
	}

	memset(bankName, 0, 0x50);

	strcpy(bankName, data + 0x3C);
	return true;
}