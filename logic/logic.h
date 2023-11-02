#include <stdint.h>

#include "../util/filter.h"

#include "../submodules/cubiomes/generator.h"

int structureFilterLogic(int64_t seed, SeedInfo* seed_info, Filter* filter);
int biomeFilterLogic(int64_t seed, SeedInfo* seed_info, Filter* filter, Generator *world_gen);