#ifndef FILTER_H
#define FILTER_H

#include "../include/finders.h"

typedef struct {
	Pos fortresses[4];
	int fort_count;

	Pos bastions[4];
	int bast_count;

	Pos villages[4];
	int vill_count;

	Pos jungle_temples[4];
	int jt_count;

	Pos ruined_portal;
	int rp_count;

	Pos buried_treasures[8];
	int bt_count;

	Pos lava_pools[10];
	int pool_count;

	Pos shipwrecks[4];
	int ship_rotations[4];
	int ship_types[4];
	int ship_count;

	Pos magma_ravines[5];
	Pos ravine_centers[5];
	int ravine_count;

	Pos desert_temples[4];
	int dt_count;

	Pos spawn_point;

} SeedInfo;

typedef struct {
	int iron;
	int gold;
	int light;
	int obby;
	int looting;
} RPLootRequirements;

typedef struct {
	int iron;
	int gold;
	int tnt;
	int emerald;
	int diamond;
	int food;
} BTLootRequirements;

typedef struct {
	int iron;
	int gold;
	int emerald;
	int diamond;
	int carrot;
	int wheat;
	int tnt;
} ShipLootRequirements;

typedef struct {
	int iron;
	int gold;
	int diamond;
	int emerald;
	int gunpowder;
	int god_apple;
} DTLootRequirements;

typedef struct {
	int mc_version;
	uint64_t seed_requirement;
	int output_seed_info;

	int BASTION;
	int bast_range;
	int bast_types;
	int bast_quadrants;

	int FORTRESS;
	int fort_range;
	int double_spawner;
	int fort_quadrants;

	int STRONGHOLD;
	int fsg_stronghold;
	int sh_range_from_ref;
	int check_sh_ocean;

	int VILLAGE;
	int village_range;
	int village_biome_code;
	int village_quadrants;

	int RUINED_PORTAL;
	int rp_range;
	int lava_portal;
	int check_rp_loot;
	RPLootRequirements* rp_loot;

	int LAVA_POOL;
	int lava_range;
	int pool_minimum;

	int BURIED_TREASURE;
	int bt_range;
	int bt_exclude_range;
	int bt_minimum;
	int bt_maximum;
	int check_bt_loot;
	BTLootRequirements* bt_loot;
	int bt_quadrants;

	int SPAWN_POINT;
	int sp_range;
	int sp_accuracy;

	int END_TOWERS;
	int zero_direction;
	int zero_towers;

	int END_CAGE;
	int cage_limit;

	int SHIPWRECK;
	int ship_range;
	int ship_types;
	int check_ship_loot;
	ShipLootRequirements* ship_loot;
	int ship_quadrants;

	int MAGMA_RAVINE;
	int ravine_range;
	int ravine_quadrants;

	int JUNGLE_TEMPLE;
	int jt_range;
	int jt_quadrants;

	int DESERT_TEMPLE;
	int dt_range;
	int check_dt_loot;
	DTLootRequirements* dt_loot;
	int dt_quadrants;

} Filter;

void clear_info(SeedInfo* info);
int parse_filter_code(Filter* filter, char* filter_code);

#endif