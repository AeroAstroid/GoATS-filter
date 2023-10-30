#include <stdint.h>
#include "../include/finders.h"
#include "../module/filter.h"
#include "../module/mathutils.h"

// Finds a village within max_dist chunks in any quadrant
int village_pos(int64_t lower48, SeedInfo* seed_info, int max_dist, int quadrants, int mc_version) {
	int q = -1;
	
	// Iterate through regions in X and Z
	for (int rX = -1; rX <= 0; rX++) for (int rZ = -1; rZ <= 0; rZ++) {
		q++;

		if (!get_bit(quadrants, q)) {
			continue;
		}

		Pos vill_pos;
		
		if (!getStructurePos(Village, mc_version, lower48, rX, rZ, &vill_pos)) {
			continue;
		}

		if (vill_pos.x > max_dist*16 || vill_pos.x < -max_dist*16) {
			continue;
		}

		if (vill_pos.z > max_dist*16 || vill_pos.z < -max_dist*16) {
			continue;
		}

		seed_info->villages[seed_info->vill_count] = vill_pos;
		seed_info->vill_count++;
	}

	if (seed_info->vill_count) {
		return 1;
	}

	return 0;
}

// Biome (and structure viability check) for a potential village
int village_biome(int64_t seed, SeedInfo* seed_info, int max_dist, int v_biomes, int mc_version, Generator* world_gen) {
	int viable_vill = 0;

	for (int v = 0; v < seed_info->vill_count; v++) {
		Pos vill_pos = seed_info->villages[v];

		int biome = isViableStructurePos(Village, world_gen, vill_pos.x, vill_pos.z, 0);

		if (
			((biome == plains) && (v_biomes & 0b10000)) ||
			((biome == desert) && (v_biomes & 0b01000)) ||
			((biome == savanna) && (v_biomes & 0b00100)) ||
			((biome == taiga) && (v_biomes & 0b00010)) ||
			((biome == snowy_tundra) && (v_biomes & 0b00001))) {
			seed_info->villages[viable_vill] = vill_pos;
			viable_vill++;
		}

	}

	if (viable_vill) {
		seed_info->vill_count = viable_vill;
		return 1;
	}

	return 0;
}