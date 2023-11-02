#include <stdint.h>
#include "../util/filter.h"

int jungle_temple_pos(int64_t lower48, SeedInfo* seed_info, int range, int allowed_quadrants, int mc_version);
int jungle_temple_biome(int64_t seed, SeedInfo* seed_info, Generator* world_gen);