#include "../filters/buriedtreasure.h"
#include "../filters/jungletemple.h"
#include "../filters/deserttemple.h"
#include "../filters/magmaravine.h"
#include "../filters/bastion.h"
#include "../filters/fortress.h"
#include "../filters/ruinedportal.h"
#include "../filters/shipwreck.h"
#include "../filters/village.h"
#include "../filters/stronghold.h"
#include "../filters/lavapool.h"
#include "../filters/spawn.h"
#include "../filters/zerocycletower.h"
#include "../filters/endisland.h"

#include "../util/filter.h"

#include "../submodules/cubiomes/biomes.h"

int structureFilterLogic(int64_t seed, SeedInfo* seed_info, Filter* filter) {
	if (filter->BASTION) {
		if (!bastion_pos(seed, seed_info, filter->bast_range, filter->bast_quadrants, filter->mc_version)) {
			return 0;
		}

		if (filter->bast_types != 0b1111) {
			if (!bastion_type(seed, seed_info, filter->bast_types)) {
				return 0;
			}
		}
	}

	if (filter->FORTRESS) {
		if (!fortress_pos(seed, seed_info, filter->fort_range, filter->fort_quadrants, filter->mc_version)) {
			return 0;
		}

		if (filter->double_spawner) {
			if (!double_spawner(seed, seed_info, filter->mc_version)) {
				return 0;
			}
		}
	}

	if (filter->STRONGHOLD) {
		// Stronghold angle check (FSG only)
		if (filter->fsg_stronghold) {
			if (!fsg_stronghold_angle(seed, seed_info, filter->sh_range_from_ref, filter->mc_version)) {
				return 0;
			}
		}
	}

	if (filter->VILLAGE) {
		if (!village_pos(seed, seed_info, filter->village_range, filter->village_quadrants, filter->mc_version)) {
			return 0;
		}
	}

	if (filter->RUINED_PORTAL) {

		if (!ruined_portal_pos(seed, seed_info, filter->rp_range, filter->mc_version)) {
			return 0;
		}

		if (filter->lava_portal) {
			if (!lava_portal(seed, seed_info)) {
				return 0;
			}
		}

		if (filter->check_rp_loot) {
			if (!portal_loot(seed, seed_info, filter->rp_loot)) {
				return 0;
			}
		}
	}

	if (filter->BURIED_TREASURE) {
		if (!buried_treasure(seed, seed_info, filter->bt_minimum, filter->bt_exclude_range, filter->bt_quadrants)) {
			return 0;
		}

		if (filter->check_bt_loot) {
			if (!buried_treasure_loot(seed, seed_info, filter->bt_minimum, filter->bt_loot)) {
				return 0;
			}
		}
	}

	if (filter->SHIPWRECK) {
		if (!shipwreck_pos(seed, seed_info, filter->ship_range, filter->ship_quadrants, filter->mc_version)) {
			return 0;
		}
	}

	if (filter->JUNGLE_TEMPLE) {
		if (!jungle_temple_pos(seed, seed_info, filter->jt_range, filter->jt_quadrants, filter->mc_version)) {
			return 0;
		}
	}

	if (filter->DESERT_TEMPLE) {
		if (!desert_temple_pos(seed, seed_info, filter->dt_range, filter->dt_quadrants, filter->mc_version)) {
			return 0;
		}

		if (filter->check_dt_loot) {
			if (!desert_temple_loot(seed, seed_info, filter->dt_loot, filter->mc_version)) {
				return 0;
			}
		}
	}

	if (filter->MAGMA_RAVINE) {
		if (!magma_ravine(seed, filter->ravine_range, filter->ravine_quadrants, seed_info)) {
			return 0;
		}
	}

	if (filter->END_CAGE) {
		if (!end_cage(seed, filter->mc_version, filter->cage_limit)) {
			return 0;
		}
	}

	return 1;
}

int biomeFilterLogic(int64_t seed, SeedInfo* seed_info, Filter* filter, Generator *world_gen) {

	if (filter->END_TOWERS) {
		if (!zero_cycle_towers(seed, filter->mc_version, filter->zero_direction, filter->zero_towers)) {
			return 0;
		}
	}

	applySeed(world_gen, DIM_OVERWORLD, seed);

	if (filter->BASTION) {
		if (!bastion_biome(seed, seed_info, world_gen)) {
			return -1;
		}
	}

	if (filter->STRONGHOLD) {
		if (filter->fsg_stronghold) {
			if (!fsg_stronghold_pos(seed, seed_info, filter->sh_range_from_ref, filter->check_sh_ocean, filter->mc_version, world_gen)) {
				return 0;
			}

		} else {
			if (!close_stronghold_pos(seed, filter->sh_range_from_ref, filter->check_sh_ocean, filter->mc_version, world_gen)) {
				return 0;
			}
		}
	}

	if (filter->VILLAGE) {
		if (!village_biome(seed, seed_info, filter->village_range, filter->village_biome_code, filter->mc_version, world_gen)) {
			return 0;
		}
	}

	if (filter->RUINED_PORTAL) {
		if (!portal_biome(seed, seed_info, filter->mc_version, world_gen)) {
			return 0;
		}
	}

	if (filter->BURIED_TREASURE) {
		if (!bt_biome(seed, seed_info, filter->bt_range, filter->bt_minimum, filter->bt_maximum, filter->mc_version, world_gen)) {
			return 0;
		}
	}

	if (filter->SHIPWRECK) {
		if (filter->ship_types != 0b111) {
			if (!shipwreck_type(seed, seed_info, filter->ship_types)) {
				return 0;
			}
		}

		if (filter->check_ship_loot) {
			if(!shipwreck_loot(seed, seed_info, filter->ship_loot)) {
				return 0;
			}
		}
		
		if (!shipwreck_biome(seed, seed_info, world_gen)) {
			return 0;
		}
	}

	if (filter->JUNGLE_TEMPLE) {
		if (!jungle_temple_biome(seed, seed_info, world_gen)) {
			return 0;
		}
	}

	if (filter->DESERT_TEMPLE) {
		if (!desert_temple_biome(seed, seed_info, world_gen)) {
			return 0;
		}
	}

	if (filter->MAGMA_RAVINE) {
		if (!ravine_biome(seed, seed_info, world_gen)) {
			return 0;
		}
	}

	if (filter->LAVA_POOL) {
		if (!lava_pool(seed, seed_info, filter->lava_range, filter->pool_minimum, world_gen)) {
			return 0;
		}
	}

	if (filter->SPAWN_POINT) {
		if (!spawn_pos(seed, seed_info, filter->sp_range, filter->mc_version, filter->sp_accuracy, world_gen)) {
			return 0;
		}
	}

	return 1;
}