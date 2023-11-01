#include <stdio.h>
#include <stdint.h>
#include "../libs/cubiomes/finders.h"
#include "../module/filter.h"

// Structure seed call for potential desert lava pools
// Deserts do not generate water lakes, so we skip those calls instead of simulating them
// Returns 1 and populates the pointers if there's a potential lava pool in the chunk, returns 0 otherwise
int potential_desert_lava(int64_t lower48, int x, int z, int* lx, int* lz) {
	int64_t finder_seed = (lower48) ^ 0x5DEECE66DUL; // Initialize java Random

	long a = nextLong(&finder_seed) | 1;
	long b = nextLong(&finder_seed) | 1;

	finder_seed = ((long)x * a + (long)z * b) ^ lower48;
	finder_seed = finder_seed & 0xFFFFFFFFFFFFLL; // population seed set?

	int64_t lava_seed = (finder_seed + 10000) ^ 0x5DEECE66DUL; // salt the lava lakes

	// Run the decorator probability roll for whether the pool tries to generate
	if (nextInt(&lava_seed, 8) != 0) {
		return 0;
	}

	x += nextInt(&lava_seed, 16); // add the random X offset of lava pool
	z += nextInt(&lava_seed, 16); // add the random Z offset of lava pool
	int y = nextInt(&lava_seed, nextInt(&lava_seed, 256 - 8) + 8); // Y height of lava pool

	// filter primarily for surface (or close enough) pools
	if (y >= 64 && nextInt(&lava_seed, 10) == 0) {
		*lx = x + 8;
		*lz = z + 8;

		return 1;
	}

	return 0;
}

// Structure seed call for potential non-desert lava pools
// We must simulate the water pool code first in order to properly seed the RNG for lava pools
int default_lava(int64_t lower48, int x, int z, int* lx, int* lz) {
	int64_t finder_seed = (lower48) ^ 0x5DEECE66DUL;

	long a = nextLong(&finder_seed) | 1;
	long b = nextLong(&finder_seed) | 1;

	finder_seed = ((long)x * a + (long)z * b) ^ lower48;
	finder_seed = finder_seed & 0xFFFFFFFFFFFFLL; // population seed set?

	int64_t lake_seed = (finder_seed + 10000) ^ 0x5DEECE66DUL; // salt the water lakes

	int water_y = -1;

	// Simulate water lake generation calls
	nextInt(&lake_seed, 4); // decorator chance call (not important)
	nextInt(&lake_seed, 16); // X offset call (not important)
	nextInt(&lake_seed, 16); // Z offset call (not important)
	water_y = nextInt(&lake_seed, 256); // Y height of water lake (this one is important)

	int64_t lava_seed = (finder_seed + 10001) ^ 0x5DEECE66DUL; // salt the lava lakes

	// Run the decorator probability roll for whether the pool tries to generate
	if (nextInt(&lava_seed, 8) != 0) {
		return 0;
	}

	x += nextInt(&lava_seed, 16); // add the random X offset of lava pool
	z += nextInt(&lava_seed, 16); // add the random Z offset of lava pool

	int y = nextInt(&lava_seed, nextInt(&lava_seed, 256 - 8) + 8); // Y height of lava pool

	// filter primarily for surface (or close enough) pools, with water pools deep
	// enough not to potentially override the lava
	if (y >= 64 && nextInt(&lava_seed, 10) == 0 && water_y < 61) {
		*lx = x + 8;
		*lz = z + 8;

		return 1;
	}

	return 0;
}

// Sweeps through the range chunk by chunk to find potential lava pools
int lava_pool(int64_t seed, SeedInfo* seed_info, int range, int pool_minimum, Generator* world_gen) {
	int64_t lower48 = seed & 0xFFFFFFFFFFFFLL;

	int chunkX, chunkZ;
	int lx, lz;

	int default_pool_count = 0;
	Pos default_pools[5]; // buffer for 5 potential non-desert pools
	int desert_pool_count = 0;
	Pos desert_pools[5]; // buffer for 5 potential desert pools

	for (chunkX = -range; chunkX < range; chunkX++) {
		for (chunkZ = -range; chunkZ < range; chunkZ++) {
			if (default_lava(lower48, chunkX << 4, chunkZ << 4, &lx, &lz) && default_pool_count < 5) {
				default_pools[default_pool_count].x = lx;
				default_pools[default_pool_count].z = lz;
				default_pool_count++;
			}

			if (potential_desert_lava(lower48, chunkX << 4, chunkZ << 4, &lx, &lz) && desert_pool_count < 5) {
				desert_pools[desert_pool_count].x = lx;
				desert_pools[desert_pool_count].z = lz;
				desert_pool_count++;

			}
		}
	}

	seed_info->pool_count = 0;

	for (int d = 0; d < default_pool_count; d++) {
		int lava_biome = getBiomeAt(world_gen, 1, default_pools[d].x, 63, default_pools[d].z);

		if (lava_biome == plains || lava_biome == savanna || lava_biome == snowy_tundra || lava_biome == jungle) {
			if (seed_info->pool_count > 4) {
				break;
			}
			
			seed_info->lava_pools[seed_info->pool_count] = default_pools[d];
			seed_info->pool_count++;
		}
	}

	for (int d = 0; d < desert_pool_count; d++) {
		int lava_biome = getBiomeAt(world_gen, 1, desert_pools[d].x, 63, desert_pools[d].z);

		if (lava_biome == desert) {
			if (seed_info->pool_count > 9) {
				break;
			}
			
			seed_info->lava_pools[seed_info->pool_count] = desert_pools[d];
			seed_info->pool_count++;
		}
	}

	if (seed_info->pool_count >= pool_minimum) {
		return 1;
	}

	return 0;
}