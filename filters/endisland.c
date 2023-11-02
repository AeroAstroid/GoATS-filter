#include "../submodules/cubiomes/biomenoise.h"
#include <stdint.h>
#include <stdio.h>

int end_cage(int64_t lower48, int mc_version, int end_cage_limit) {
	if (getSurfaceHeightEnd(mc_version, lower48, 100, 0) > 51 + end_cage_limit) {
		return 0;
	}

	return 1;
}