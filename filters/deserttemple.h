#include <stdint.h>

#include "../util/filter.h"

#include "../submodules/cubiomes/finders.h"

int desert_temple_pos(int64_t lower48, SeedInfo* seed_info, int range, int quadrants, int mc_version);
int desert_temple_biome(int64_t seed, SeedInfo* seed_info, Generator* world_gen);
int desert_temple_loot(int64_t lower48, SeedInfo* seed_info, DTLootRequirements* dt_loot, int mc_version);