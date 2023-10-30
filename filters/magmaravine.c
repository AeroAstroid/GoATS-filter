#include <stdint.h>
#include <stdio.h>
#include "../include/finders.h"
#include "../module/filter.h"
#include "../module/mathutils.h"

int ravine_at_chunk(int64_t lower48, int chunkX, int chunkZ, int* rx, int* rz) {
	int64_t finder_seed = (lower48) ^ 0x5DEECE66DL;

	int64_t carveX = nextLong(&finder_seed);
	int64_t carveZ = nextLong(&finder_seed);

	finder_seed = (chunkX * carveX) ^ (chunkZ * carveZ) ^ (lower48) ^ 0x5DEECE66DL;
	finder_seed = finder_seed & 0xFFFFFFFFFFFF;

	// Carver call for whether or not to generate a ravine in the chunk
	if (nextFloat(&finder_seed) >= 0.02) {
		return 0;
	}

	int magmaX = chunkX * 16 + nextInt(&finder_seed, 16); // random X offset in magma

	int temporary_y = nextInt(&finder_seed, 40) + 8;
	int magmaY = 20 + nextInt(&finder_seed, temporary_y);

	int magmaZ = chunkZ * 16 + nextInt(&finder_seed, 16); // random Z offset in magma

	nextFloat(&finder_seed); // angle call (could be useful, not implementing this)

	nextFloat(&finder_seed); // skip call for pitch (don't care)

	float temporary_w = nextFloat(&finder_seed);
	float width = 2 * (2 * temporary_w + nextFloat(&finder_seed));

	nextInt(&finder_seed, 28); // skip call for maximum ravine length (don't care)

	// parameters are arbitrary but have proven to work well
	if (magmaY < 25 && width > 5) {
		*rx = magmaX; 
		*rz = magmaZ;

		return 1;
	}

	return 0;
}

int magma_ravine(int64_t lower48, int range, int allowed_quadrants, SeedInfo* seed_info) {
	int quadrant = -1;

	// Iterate through regions in X and Z
	for (int regX = -1; regX <= 0; regX++) for (int regZ = -1; regZ <= 0; regZ++) {
		quadrant++;

		if (!get_bit(allowed_quadrants, quadrant)) {
			continue;
		}

		// Iterate through chunks in X and Z
		for (int cX = 0; cX < range; cX++) for (int cZ = 0; cZ < range; cZ++) {
			int rx, rz;
			int chunkX = cX + regX * range;
			int chunkZ = cZ + regZ * range;

			if (ravine_at_chunk(lower48, chunkX, chunkZ, &rx, &rz) && seed_info->ravine_count < 5) {
				Pos ravine_pos;

				ravine_pos.x = rx;
				ravine_pos.z = rz;

				seed_info->magma_ravines[seed_info->ravine_count] = ravine_pos;

				seed_info->ravine_count++;
			}
		}
	}

	if (seed_info->ravine_count) {
		return 1;
	}

	return 0;
}

int ravine_biome(int64_t seed, SeedInfo* seed_info, Generator* world_gen) {
	int viable_ravines = 0;

	for (int rv = 0; rv < seed_info->ravine_count; rv++) {
		Pos ravine_pos = seed_info->magma_ravines[rv];

		int biome = getBiomeAt(world_gen, 1, ravine_pos.x, 63, ravine_pos.z);

		// exclude bad oceans
		if (isDeepOcean(biome) && biome != deep_lukewarm_ocean && biome != deep_warm_ocean) {
			seed_info->magma_ravines[viable_ravines] = ravine_pos;
			viable_ravines++;
		}
	}

	seed_info->ravine_count = viable_ravines;
	if (viable_ravines > 0) {
		return 1;
	}

    return 0;
}