#include "../include/rng.h"
#include "../module/filter.h"

int bt_at_chunk(int64_t lower48, int chunkX, int chunkZ);
int buried_treasure(int64_t lower48, SeedInfo* seed_info, int bt_min, int exc_range, int allowed_quadrants);
int bt_biome(int64_t seed, SeedInfo* seed_info, int inc_range, int bt_min, int bt_max, int mc_version, Generator* world_gen);
int buried_treasure_loot(int64_t lower48, SeedInfo* seed_info, int bt_minimum, BTLootRequirements* bt_loot);