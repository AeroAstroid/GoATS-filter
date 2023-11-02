#include <stdint.h>

#include "../util/filter.h"

#include "../submodules/cubiomes/generator.h"

int village_pos(int64_t lower48, SeedInfo* seed_info, int max_dist, int quadrants, int mc_version);
int village_biome(int64_t seed, SeedInfo* seed_info, int max_dist, int v_biomes, int mc_version, Generator* world_gen);