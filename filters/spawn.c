#include <stdint.h>
#include "../libs/cubiomes/finders.h"
#include "../module/filter.h"

int spawn_pos(int64_t seed, SeedInfo* seed_info, int range, int mc_version, int accurate, Generator* world_gen) {
	Pos spawn;
	
	if (accurate) {
		spawn = getSpawn(world_gen);
	} else {
		spawn = estimateSpawn(world_gen, NULL);
	}

	if (spawn.x > range || spawn.x < -range) {
		return 0;
	}

	if (spawn.z > range || spawn.z < -range) {
		return 0;
	}

	seed_info->spawn_point = spawn;
	return 1;
}