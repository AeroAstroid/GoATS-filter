#include <stdint.h>
#include "../submodules/cubiomes/finders.h"
#include "../util/filter.h"

int fsg_stronghold_angle(int64_t seed, SeedInfo* seed_info, int max_dist, int mc_version);

int fsg_stronghold_pos(int64_t seed, SeedInfo* seed_info, int max_dist, int ocean_check, int mc_version, Generator* world_gen);
int close_stronghold_pos(int64_t seed, int max_dist, int ocean_check, int mc_version, Generator* world_gen);