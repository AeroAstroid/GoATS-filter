#include <stdint.h>
#include "../util/filter.h"
#include "../submodules/cubiomes/generator.h"

int bastion_pos(int64_t lower48, SeedInfo* seed_info, int range, int quadrants, int mc_version);
int bastion_type(int64_t lower48, SeedInfo* seed_info, int allowed_types);
int bastion_biome(int64_t seed, SeedInfo* seed_info, Generator* world_gen);