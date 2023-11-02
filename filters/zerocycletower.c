#include "../submodules/cubiomes/rng.h"
#include "../util/mathutils.h"
#include <stdio.h>

int zero_cycle_towers(int64_t seed, int mc_version, int zero_direction, int zero_towers) {
	int ind[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	int64_t pillar_seed = seed ^ 0x5DEECE66DUL;
	pillar_seed = nextLong(&pillar_seed) & 0xFFFF;
	pillar_seed = pillar_seed ^ 0x5DEECE66DUL;

	shuffle(&pillar_seed, ind, 10);

	int front = ind[0] > ind[5];

	// if it's front but we're excluding front
	if (front && !(zero_direction / 2)) {
		return 0;
	}

	// if it's back but we're excluding back
	if (!front && !(zero_direction % 2)) {
		return 0;
	}

	int tower_index = front ? ind[9] : ind[4];

	// Bit corresponing to the tower we got is not set to 1
	if (!(zero_towers >> (9 - tower_index) & 1)) {
		return 0;
	}
	
	return 1;
}