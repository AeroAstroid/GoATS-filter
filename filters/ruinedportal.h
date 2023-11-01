#include <stdint.h>
#include "../libs/cubiomes/generator.h"
#include "../module/filter.h"

int ruined_portal_pos(int64_t seed, SeedInfo* seed_info, int max_dist, int mc_version);
int lava_portal(int64_t seed, SeedInfo* seed_info);
int portal_loot(int64_t lower48, SeedInfo* seed_info, RPLootRequirements* rp_loot);

int portal_biome(int64_t seed, SeedInfo* seed_info, int mc_version, Generator* world_gen);