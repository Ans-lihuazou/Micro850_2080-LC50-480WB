#include "GlobalTool.h"

namespace Tool {

	void initRand() {
		srand(time(NULL));
	}

	uint32_t getRand() {

		return rand();
	}

	uint16_t getConnectionSerialNumber() {

		uint16_t unNumber;
		while (true) {
			unNumber = getRand()&(0xFFFF);
			if (0 == setSerialNumber.count(unNumber)) {
				setSerialNumber.insert(unNumber);
				break;
			}
		}

		return unNumber;
	}
}