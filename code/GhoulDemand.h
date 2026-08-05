#pragma once

struct GhoulDemand {
	char* dataPtr;

	bool isReady;
	bool hasErrored;

	int type;
	unsigned int timestamp;
	float fUnk1;
	int iUnk1;

	// Actual Section Info
	int dataSectOffset;
	int dataSectSize;
	int gpuSectOffset;
	int gpuSectSize;

	void ReadDemandFile(char* data);
	bool readStandaloneDemandFile(char* fileName);

	char* GetDataSection() {
		if (dataPtr == nullptr) return nullptr;

		char* data = (char*)malloc(dataSectSize);
		memcpy(data, dataPtr + dataSectOffset, dataSectSize);

		return data;
	}

	char* GetGpuSection() {
		if (dataPtr == nullptr) return nullptr;
		if (gpuSectSize == 0) return nullptr;

		char* data = (char*)malloc(gpuSectSize);
		memcpy(data, dataPtr + gpuSectOffset, gpuSectSize);

		return data;
	}
};