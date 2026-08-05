#include "CommonReader.h"
#include "GhoulDemand.h"

void GhoulDemand::ReadDemandFile(char* data) {
	dataPtr = data;
	isReady = false;

	try {
		memcpy(&type, data, sizeof(int));
		memcpy(&timestamp, data + 4, sizeof(int));
		memcpy(&fUnk1, data + 8, sizeof(int));
		memcpy(&iUnk1, data + 0xC, sizeof(int));

		memcpy(&dataSectOffset, data + 0x10, sizeof(int));
		memcpy(&dataSectSize, data + 0x14, sizeof(int));
		memcpy(&gpuSectOffset, data + 0x18, sizeof(int));
		memcpy(&gpuSectSize, data + 0x1C, sizeof(int));

		time_t time = timestamp;

		printf("Demand File - {\n");
		printf("\tTYPE %d\n", type);
		printf("\tTIMESTAMP %u - %s", timestamp, ctime(&time));
		printf("\tFLOAT %f\n", fUnk1);
		printf("\tINT %d\n", iUnk1);
		printf("}\n", type, ctime(&time), fUnk1, iUnk1);

		if (type < 0 || type >= 30) {
			hasErrored = true;
			return;
		}

		isReady = true;
	}
	catch (int e) {

	}
}

bool GhoulDemand::readStandaloneDemandFile(char* fileName) {

	FILE* currentFile = fopen(fileName, "rb");

	if (ferror(currentFile) != 0) {
		printf("Error occured while trying to open the file.\n");
		return false;
	}

	fseek(currentFile, 0L, SEEK_END);
	int length = ftell(currentFile);
	fseek(currentFile, 0L, SEEK_SET);

	char* data = (char*)malloc(length);

	fread(data, sizeof(char), length, currentFile);

	fclose(currentFile);

	ReadDemandFile(data);
	return true;
}