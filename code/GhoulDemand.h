#pragma once

struct GhoulDemand {
	char* dataPtr;

	bool isReady;
	bool hasErrored;

	int32_t type;
	uint32_t timestamp;
	float fUnk1;
	int32_t iUnk1;

	// Actual Section Info
	int32_t dataSectOffset;
	int32_t dataSectSize;
	int32_t gpuSectOffset;
	int32_t gpuSectSize;

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