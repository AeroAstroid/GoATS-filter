#include <stdlib.h>
#include <stdio.h>

#include "filter.h"

void clear_info(SeedInfo* info) {
	info->fort_count = 0;
	info->bast_count = 0;
	info->vill_count = 0;
	info->rp_count = 0;
	info->bt_count = 0;
	info->pool_count = 0;
	info->ship_count = 0;
	info->ravine_count = 0;
	info->jt_count = 0;
	info->dt_count = 0;
}

int parse_filter_code(Filter* filter, char* filter_code) {
	int version = strtol(filter_code, &filter_code, 10); // Minecraft version to filter

	int supported_versions[] = {
		MC_1_0,  MC_1_1,  MC_1_2,  MC_1_3,  MC_1_4,  MC_1_5,  MC_1_6,  MC_1_7,  MC_1_8,  MC_1_9,
		MC_1_10, MC_1_11, MC_1_12, MC_1_13, MC_1_14, MC_1_15, MC_1_16, MC_1_17, MC_1_18, MC_1_19,
		MC_1_20
	};

	filter->mc_version = supported_versions[version]; // Minecraft version converted to the proper enum value

	int64_t seed_req = strtol(filter_code, &filter_code, 10); // Number of seeds to filter
	if (seed_req < 0) {
		filter->seed_requirement = 0xFFFFFFFFFFFFFFFFLL - 1;
	} else {
		filter->seed_requirement = seed_req;
	}

	filter->output_seed_info = strtol(filter_code, &filter_code, 10); // Whether to write seed info to a file


	filter->BASTION = strtol(filter_code, &filter_code, 10); // Whether to filter bastion
	if (filter->BASTION) {
		filter->bast_range = strtol(filter_code, &filter_code, 10); // Bastion range
		filter->bast_types = strtol(filter_code, &filter_code, 10); // Bastion type code
		filter->bast_quadrants = strtol(filter_code, &filter_code, 10); // Bastion quadrant code
	}

	filter->FORTRESS = strtol(filter_code, &filter_code, 10); // Whether to filter fortress
	if (filter->FORTRESS) {
		filter->fort_range = strtol(filter_code, &filter_code, 10); // Fortress range
		filter->double_spawner = strtol(filter_code, &filter_code, 10); // Whether to filter double spawner
		filter->fort_quadrants = strtol(filter_code, &filter_code, 10); // Fortress quadrant code
	}


	filter->STRONGHOLD = strtol(filter_code, &filter_code, 10); // Whether to filter stronghold
	if (filter->STRONGHOLD) {
		filter->fsg_stronghold = strtol(filter_code, &filter_code, 10); // Whether to apply FSG stronghold filtering
		filter->sh_range_from_ref = strtol(filter_code, &filter_code, 10); // Stronghold range (both for FSG or spawn)
		filter->check_sh_ocean = strtol(filter_code, &filter_code, 10); // Whether to force deep ocean stronghold
	}


	filter->VILLAGE = strtol(filter_code, &filter_code, 10); // Whether to filter village
	if (filter->VILLAGE) {
		filter->village_range = strtol(filter_code, &filter_code, 10); // Village range
		filter->village_biome_code = strtol(filter_code, &filter_code, 10); // Binary code for the allowed village types
		filter->village_quadrants = strtol(filter_code, &filter_code, 10); // Village quadrant code
	}


	filter->RUINED_PORTAL = strtol(filter_code, &filter_code, 10); // Whether to filter ruined portal
	if (filter->RUINED_PORTAL) {
		filter->rp_range = strtol(filter_code, &filter_code, 10); // Ruined portal range

		filter->lava_portal = strtol(filter_code, &filter_code, 10); // Whether or not to force lava portal

		filter->rp_loot = malloc(sizeof(RPLootRequirements));

		filter->rp_loot->iron = strtol(filter_code, &filter_code, 10); // Iron requirement
		filter->rp_loot->gold = strtol(filter_code, &filter_code, 10); // Gold requirement
		filter->rp_loot->light = strtol(filter_code, &filter_code, 10); // Light requirement
		filter->rp_loot->obby = strtol(filter_code, &filter_code, 10); // Obby requirement
		filter->rp_loot->looting = strtol(filter_code, &filter_code, 10); // Looting level requirement

		filter->check_rp_loot = (
			filter->rp_loot->iron ||
			filter->rp_loot->gold ||
			filter->rp_loot->light ||
			filter->rp_loot->obby ||
			filter->rp_loot->looting
		); // Whether to run the loot function at all (FALSE if there's no requirements, TRUE if there are any at all)
	}


	filter->LAVA_POOL = strtol(filter_code, &filter_code, 10); // Type of lava pool filter
	if (filter->LAVA_POOL) {
		filter->lava_range = strtol(filter_code, &filter_code, 10); // Lava pool range
		filter->pool_minimum = strtol(filter_code, &filter_code, 10); // number of pools to return
	}


	filter->BURIED_TREASURE = strtol(filter_code, &filter_code, 10); // Whether to filter buried treasure
	if (filter->BURIED_TREASURE) {
		filter->bt_range = strtol(filter_code, &filter_code, 10); // BT inclusion range
		filter->bt_exclude_range  = strtol(filter_code, &filter_code, 10); // BT exclusion range
		filter->bt_minimum = strtol(filter_code, &filter_code, 10); // Minimum BTs to find in inclusion range
		filter->bt_maximum = strtol(filter_code, &filter_code, 10); // Maximum BTs to find in exclusion range

		filter->bt_loot = malloc(sizeof(BTLootRequirements));

		filter->bt_loot->iron = strtol(filter_code, &filter_code, 10); // Iron requirement
		filter->bt_loot->gold = strtol(filter_code, &filter_code, 10); // Gold requirement
		filter->bt_loot->tnt = strtol(filter_code, &filter_code, 10); // TNT requirement
		filter->bt_loot->emerald = strtol(filter_code, &filter_code, 10); // Emerald requirement
		filter->bt_loot->diamond = strtol(filter_code, &filter_code, 10); // Diamond requirement
		filter->bt_loot->food = strtol(filter_code, &filter_code, 10); // Fish requirement

		filter->check_bt_loot = (
			filter->bt_loot->iron ||
			filter->bt_loot->gold ||
			filter->bt_loot->tnt ||
			filter->bt_loot->emerald ||
			filter->bt_loot->diamond ||
			filter->bt_loot->food
		); // Whether to run the loot function at all (FALSE if there's no requirements, TRUE if there are any at all)

		filter->bt_quadrants = strtol(filter_code, &filter_code, 10); // BT quadrant code
	}


	filter->SPAWN_POINT = strtol(filter_code, &filter_code, 10); // Whether to filter spawn point
	if (filter->SPAWN_POINT) {
		filter->sp_range = strtol(filter_code, &filter_code, 10); // Spawn point range
		filter->sp_accuracy = strtol(filter_code, &filter_code, 10); // Spawn function accuracy level
	}


	filter->END_TOWERS = strtol(filter_code, &filter_code, 10); // Whether to filter zero towers
	if (filter->END_TOWERS) {
		filter->zero_direction = strtol(filter_code, &filter_code, 10); // Front/back dragon
		filter->zero_towers = strtol(filter_code, &filter_code, 10); // Allowed diagonal towers
	}


	filter->SHIPWRECK = strtol(filter_code, &filter_code, 10); // Whether to filter ships
	if (filter->SHIPWRECK) {
		filter->ship_range = strtol(filter_code, &filter_code, 10); // Shipwreck range
		filter->ship_types = strtol(filter_code, &filter_code, 10); // Ship type code

		filter->ship_loot = malloc(sizeof(ShipLootRequirements));

		filter->ship_loot->iron = strtol(filter_code, &filter_code, 10); // Iron requirement
		filter->ship_loot->gold = strtol(filter_code, &filter_code, 10); // Gold requirement
		filter->ship_loot->emerald = strtol(filter_code, &filter_code, 10); // Emerald requirement
		filter->ship_loot->diamond = strtol(filter_code, &filter_code, 10); // Diamond requirement

		filter->ship_loot->carrot = strtol(filter_code, &filter_code, 10); // Carrot requirement
		filter->ship_loot->wheat = strtol(filter_code, &filter_code, 10); // Carrot requirement
		filter->ship_loot->tnt = strtol(filter_code, &filter_code, 10); // TNT requirement

		int requires_front = (
			filter->ship_loot->carrot ||
			filter->ship_loot->wheat ||
			filter->ship_loot->tnt
		);

		int requires_back = (
			filter->ship_loot->iron ||
			filter->ship_loot->gold ||
			filter->ship_loot->emerald ||
			filter->ship_loot->diamond
		);

		// If the loot requirement needs front half, optimize by removing back-half-only from consideration
		if (requires_front && filter->ship_types & (1 << 1)) {
			filter->ship_types -= (1 << 1);
		}

		// If the loot requirement needs back half, optimize by removing front-half-only from consideration
		if (requires_back && filter->ship_types & (1 << 2)) {
			filter->ship_types -= (1 << 2);
		}

		// Whether to run the loot function at all (FALSE if there's no requirements, TRUE if there are any at all)
		filter->check_ship_loot = requires_front || requires_back;

		filter->ship_quadrants = strtol(filter_code, &filter_code, 10); // Shipwreck quadrant code
	}


	filter->MAGMA_RAVINE = strtol(filter_code, &filter_code, 10); // Whether to filter ravines
	if (filter->MAGMA_RAVINE) {
		filter->ravine_range = strtol(filter_code, &filter_code, 10); // Magma ravine range
		filter->ravine_quadrants = strtol(filter_code, &filter_code, 10); // Ravine quadrant code
	}


	filter->JUNGLE_TEMPLE = strtol(filter_code, &filter_code, 10); // Type of jungle temple filter
	if (filter->JUNGLE_TEMPLE) {
		filter->jt_range = strtol(filter_code, &filter_code, 10); // Jungle temple range
		filter->jt_quadrants = strtol(filter_code, &filter_code, 10); // Jungle temple quadrant code
	}


	filter->DESERT_TEMPLE = strtol(filter_code, &filter_code, 10); // Whether to filter for desert temple
	if (filter->DESERT_TEMPLE) {
		filter->dt_range = strtol(filter_code, &filter_code, 10); // Desert temple range

		filter->dt_loot = malloc(sizeof(BTLootRequirements));

		filter->dt_loot->iron = strtol(filter_code, &filter_code, 10); // Iron requirement
		filter->dt_loot->gold = strtol(filter_code, &filter_code, 10); // Gold requirement
		filter->dt_loot->diamond = strtol(filter_code, &filter_code, 10); // Diamond requirement
		filter->dt_loot->emerald = strtol(filter_code, &filter_code, 10); // Emerald requirement
		filter->dt_loot->gunpowder = strtol(filter_code, &filter_code, 10); // Gunpowder requirement
		filter->dt_loot->god_apple = strtol(filter_code, &filter_code, 10); // God apple requirement

		filter->check_dt_loot = (
			filter->dt_loot->iron ||
			filter->dt_loot->gold ||
			filter->dt_loot->diamond ||
			filter->dt_loot->emerald ||
			filter->dt_loot->gunpowder ||
			filter->dt_loot->god_apple
		); // Whether to run the loot function at all (FALSE if there's no requirements, TRUE if there are any at all)

		filter->dt_quadrants = strtol(filter_code, &filter_code, 10); // Desert temple quadrant code
	}


	filter->END_CAGE = strtol(filter_code, &filter_code, 10); // Whether to control the end cage
	if (filter->END_CAGE) {
		filter->cage_limit = strtol(filter_code, &filter_code, 10); // End cage max size
	}

	filter->BASTION &= filter->mc_version >= MC_1_16;
	filter->RUINED_PORTAL &= filter->mc_version >= MC_1_16;

	filter->SHIPWRECK &= filter->mc_version >= MC_1_13;
	filter->BURIED_TREASURE &= filter->mc_version >= MC_1_13;
	filter->MAGMA_RAVINE &= filter->mc_version >= MC_1_13;

	filter->END_TOWERS &= filter->mc_version >= MC_1_9;

	filter->JUNGLE_TEMPLE &= filter->mc_version >= MC_1_3;
	filter->DESERT_TEMPLE &= filter->mc_version >= MC_1_3;

	return 1;
}