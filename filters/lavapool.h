#include <stdint.h>

#include "../util/filter.h"

#include "../submodules/cubiomes/generator.h"

int potential_desert_lava(int64_t lower48, int x, int z, int* lx, int* lz);
int default_lava(int64_t lower48, int x, int z, int* lx, int* lz);
int lava_pool(int64_t seed, SeedInfo* seed_info, int range, int pool_minimum, Generator* world_gen);