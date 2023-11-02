#include <stdint.h>

#include "../util/filter.h"
#include "../util/mathutils.h"

#include "../submodules/cubiomes/finders.h"
#include "../submodules/cubiomes/generator.h"

// Finds a bastion within a certain distance of origin
int bastion_pos(int64_t lower48, SeedInfo* seed_info, int range, int allowed_quadrants, int mc_version) {
	int q = -1;
	
	// Iterate through regions in X and Z
	for (int rX = -1; rX <= 0; rX++) for (int rZ = -1; rZ <= 0; rZ++) {
		q++;

		if (!get_bit(allowed_quadrants, q)) {
			continue;
		}

		Pos bast_pos;
		
		if (!getStructurePos(Bastion, mc_version, lower48, rX, rZ, &bast_pos)) {
			continue;
		}

		if (bast_pos.x > range*16 || bast_pos.x < -range*16) {
			continue;
		}

		if (bast_pos.z > range*16 || bast_pos.z < -range*16) {
			continue;
		}

		seed_info->bastions[seed_info->bast_count] = bast_pos;
		seed_info->bast_count++;
	}

	if (seed_info->bast_count) {
		return 1;
	}

	return 0;
}

// Forces the bastion to be of a certain type
int bastion_type(int64_t lower48, SeedInfo* seed_info, int allowed_types) {
	int viable_bastions = 0;

	int64_t finder_seed = lower48 ^ 0x5DEECE66DLL;

	int64_t carveX = nextLong(&finder_seed);
	int64_t carveZ = nextLong(&finder_seed);

	for (int b = 0; b < seed_info->bast_count; b++) {
		Pos bast_pos = seed_info->bastions[b];

		int chunkX = bast_pos.x >> 4;
		int chunkZ = bast_pos.z >> 4;

		finder_seed = ((chunkX * carveX) ^ (chunkZ * carveZ) ^ lower48 ^ 0x5DEECE66DLL) & 0xFFFFFFFFFFFFL;

		int type = nextInt(&finder_seed, 4);

		if (allowed_types & (1 << type)) {
			seed_info->bastions[viable_bastions] = bast_pos;
			viable_bastions++;
		}
	}

	if (viable_bastions) {
		seed_info->bast_count = viable_bastions;
		return 1;
	}

	return 0;
}

// Checks for basalt deltas at bastion location
int bastion_biome(int64_t seed, SeedInfo* seed_info, Generator* world_gen) {
	Generator nether_gen = *world_gen;
	applySeed(&nether_gen, DIM_NETHER, seed);

	int viable_bastions = 0;

	for (int i = 0; i < seed_info->bast_count; i++) {
		Pos bast_pos = seed_info->bastions[i];

		if (isViableStructurePos(Bastion, &nether_gen, bast_pos.x, bast_pos.z, 0)) {
			seed_info->bastions[viable_bastions] = bast_pos;
			viable_bastions++;
		}
	}

	if (viable_bastions) {
		seed_info->bast_count = viable_bastions;
		return 1;
	}
	
	return 0;
}