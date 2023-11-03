#include <stdint.h>

#include "../util/filter.h"

#include "../submodules/cubiomes/generator.h"

int shipwreck_type(int64_t lower48, SeedInfo* seed_info, int allowed_types);
int shipwreck_pos(int64_t lower48, SeedInfo* seed_info, int range, int quadrants, int mc_version);
int shipwreck_biome(int64_t seed, SeedInfo* seed_info, Generator* world_gen);
int shipwreck_loot(int64_t lower48, SeedInfo* seed_info, ShipLootRequirements* ship_loot, int mc_version);