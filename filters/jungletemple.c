#include <stdint.h>

#include "../util/filter.h"
#include "../util/mathutils.h"

#include "../submodules/cubiomes/finders.h"
#include "../submodules/cubiomes/generator.h"

// Finds a jungle temple within max_dist chunks in any quadrant
int jungle_temple_pos(int64_t lower48, SeedInfo* seed_info, int range, int allowed_quadrants, int mc_version) {
	int q = -1;
	
	// Iterate through regions in X and Z
	for (int rX = -1; rX <= 0; rX++) for (int rZ = -1; rZ <= 0; rZ++) {
		q++;

		if (!get_bit(allowed_quadrants, q)) {
			continue;
		}

		Pos jt_pos;
		
		if (!getStructurePos(Jungle_Pyramid, mc_version, lower48, rX, rZ, &jt_pos)) {
			continue;
		}

		if (jt_pos.x > range*16 || jt_pos.x < -range*16) {
			continue;
		}

		if (jt_pos.z > range*16 || jt_pos.z < -range*16) {
			continue;
		}

		seed_info->jungle_temples[seed_info->jt_count] = jt_pos;
		seed_info->jt_count++;
	}

	if (seed_info->jt_count) {
		return 1;
	}

	return 0;
}

// Biome check for a potential jungle temple
int jungle_temple_biome(int64_t seed, SeedInfo* seed_info, Generator* world_gen) {
	int viable_jts = 0;

	for (int jt = 0; jt < seed_info->jt_count; jt++) {
		Pos jt_pos = seed_info->jungle_temples[jt];

		if(isViableStructurePos(Jungle_Pyramid, world_gen, jt_pos.x, jt_pos.z, 0)) {
			seed_info->jungle_temples[viable_jts] = jt_pos;
			viable_jts++;
		}
	}

	if (viable_jts) {
		seed_info->jt_count = viable_jts;
		return 1;
	}

	return 0;
}