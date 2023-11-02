#include <stdint.h>
#include "../util/filter.h"

int ravine_at_chunk(int64_t lower48, int chunkX, int chunkZ, int* rx, int* rz);
int magma_ravine(int64_t lower48, int range, int quadrants, SeedInfo* seed_info);
int ravine_biome(int64_t seed, SeedInfo* seed_info, Generator* world_gen);