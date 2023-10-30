#include "../include/rng.h"
#include "../module/filter.h"
#include "../module/mathutils.h"
#include <stdio.h>

// Whether a given chunk attempts to generate a BT
int bt_at_chunk(int64_t lower48, int chunkX, int chunkZ){
	int64_t salt = 10387320L;		// salt for buried treasure
	int64_t cX = 341873128712L;	// constant for 1 region offset in the X direction
	int64_t cZ = 132897987541L;	// constant for 1 region offset in the Z direction

	// Salt and offset the seed
	int64_t finder_seed = (lower48 + salt + chunkX * cX + chunkZ * cZ);

	finder_seed = finder_seed ^ 0x5DEECE66DUL; // Initialize the java Random

	int generates = nextFloat(&finder_seed) < 0.01; // 1/100 chance per chunk

	return generates;
}

// Sweep through the range chunk by chunk to find structure seed BTs
int buried_treasure(int64_t lower48, SeedInfo* seed_info, int bt_min, int exc_range, int allowed_quadrants) {
	int quadrant = -1;

	// Iterate through regions in X and Z
	for (int regX = -1; regX <= 0; regX++) for (int regZ = -1; regZ <= 0; regZ++) {
		quadrant++;

		if (!get_bit(allowed_quadrants, quadrant)) {
			continue;
		}

		// Iterate through chunks in X and Z
		for (int cX = 0; cX < exc_range; cX++) for (int cZ = 0; cZ < exc_range; cZ++) {
			Pos bt_pos;
			int chunkX = cX + regX * exc_range;
			int chunkZ = cZ + regZ * exc_range;

			if (bt_at_chunk(lower48, chunkX, chunkZ)) {
				if (seed_info->bt_count < 8) {
					
					bt_pos.x = chunkX * 16 + 9;
					bt_pos.z = chunkZ * 16 + 9;

					seed_info->buried_treasures[seed_info->bt_count] = bt_pos;
					seed_info->bt_count++;
				}
			}
		}
	}

	if (seed_info->bt_count >= bt_min) {
		return 1;
	}

	return 0;
}

// Check current viable BTs for the proper biome
int bt_biome(int64_t seed, SeedInfo* seed_info, int inc_range, int bt_min, int bt_max, int mc_version, Generator* world_gen) {
	int viable_bts = 0;
	int buffer_bts = 0;

	for (int bt = 0; bt < seed_info->bt_count; bt++) {
		Pos bt_pos = seed_info->buried_treasures[bt];
		Pos probing_point = bt_pos;

		if (isViableStructurePos(Treasure, world_gen, bt_pos.x, bt_pos.z, 0)) {
			int in_exclusion_zone = 0;

			if (bt_pos.x > inc_range*16 || bt_pos.x < -inc_range*16) {
				in_exclusion_zone = 1;
			}
			if (bt_pos.z > inc_range*16 || bt_pos.z < -inc_range*16) {
				in_exclusion_zone = 1;
			}

			buffer_bts++;

			if (!in_exclusion_zone) {
				seed_info->buried_treasures[viable_bts] = bt_pos;
				viable_bts++;
			}
		}
	}

	if (viable_bts >= bt_min && buffer_bts <= bt_max) {
		seed_info->bt_count = viable_bts;
		return 1;
	}

	return 0;
}

int buried_treasure_loot(int64_t lower48, SeedInfo* seed_info, int bt_minimum, BTLootRequirements* bt_loot) {
	int viable_bts = 0;

	for (int bt = 0; bt < seed_info->bt_count; bt++) {
		int64_t finder_seed = (lower48) ^ 0x5DEECE66DUL;

		long carveX = nextLong(&finder_seed) | 1;
		long carveZ = nextLong(&finder_seed) | 1;

		Pos bt_pos = seed_info->buried_treasures[bt];
		
		finder_seed = ((long)(bt_pos.x - 9) * carveX + (long)(bt_pos.z - 9) * carveZ) ^ lower48 & 0xFFFFFFFFFFFFLL;
		finder_seed = (finder_seed + 30001) ^ 0x5DEECE66DUL;

		int64_t loot_table_seed = (int64_t)nextLong(&finder_seed) ^ 0x5DEECE66DUL;

		int iron_count = 0;
		int gold_count = 0;
		int tnt_count = 0;
		int emerald_count = 0;
		int diamond_count = 0;
		int food_count = 0;

		// Loot pool 1: Heart of the sea
		// Doesn't need a table - only 1 possible item, so no random calls are even used

		// Loot pool 2: Ingots and TNT
		int loot_table_2[] = { // 3 possible items
		//	wei	min	max	
			20,	1,	4,	// iron
			10,	1,	4,	// gold
			5,	1,	2	// TNT
		};

		int loot_rolls = 5 + nextInt(&loot_table_seed, 4); // 5-8 calls to loot table 2

		for (int roll = 0; roll < loot_rolls; roll++) {
			int loot_value = nextInt(&loot_table_seed, 35);
			int item_id;

			for (item_id = 0; loot_value > 0; ) {
				loot_value -= loot_table_2[3 * item_id];

				if (loot_value >= 0) {
					item_id++;
				}
			}
			
			int min_count = loot_table_2[3 * item_id + 1];
			int max_count = loot_table_2[3 * item_id + 2];

			int item_count = min_count + nextInt(&loot_table_seed, max_count - min_count + 1);

			if (item_id == 0) {
				iron_count += item_count;
			}
			if (item_id == 1) {
				gold_count += item_count;
			}
			if (item_id == 2) {
				tnt_count += item_count;
			}
		}

		// Loot pool 3: Emerald, diamond, prismarine
		int loot_table_3[] = { // 3 possible items
		//	wei	min	max	
			5,	4,	8,	// emerald
			5,	1,	2,	// diamond
			5,	1,	5	// prismarine
		};

		loot_rolls = 1 + nextInt(&loot_table_seed, 3); // 1-3 calls to loot pool 3

		for (int roll = 0; roll < loot_rolls; roll++) {
			int loot_value = nextInt(&loot_table_seed, 15);
			int item_id;

			for (item_id = 0; loot_value > 0; ) {
				loot_value -= loot_table_3[3 * item_id];

				if (loot_value >= 0) {
					item_id++;
				}
			}
			
			int min_count = loot_table_3[3 * item_id + 1];
			int max_count = loot_table_3[3 * item_id + 2];

			int item_count = min_count + nextInt(&loot_table_seed, max_count - min_count + 1);

			if (item_id == 0) {
				emerald_count += item_count;
			}
			if (item_id == 1) {
				diamond_count += item_count;
			}
		}

		// Loot pool 4: Chestplate and sword
		// Doesn't need a table - 2 possible items with the same weight
		loot_rolls = nextInt(&loot_table_seed, 2); // 0-1 call to loot pool 4

		for (int roll = 0; roll < loot_rolls; roll++) {
			int item_id = nextInt(&loot_table_seed, 2);

			// Maybe implement chestplate and iron sword tracking one day?
			// For now, doesn't feel useful
		}

		// Loot pool 5: Fish
		// Also doesn't need a table - 2 possible items with the same weight
		loot_rolls = 2;

		for (int roll = 0; roll < loot_rolls; roll++) {
			int item_id = nextInt(&loot_table_seed, 2);
			int item_count = 2 + nextInt(&loot_table_seed, 3); // 2-4 fish

			food_count += item_count; // Doesn't differentiate between cod and salmon
		}
		
		if (iron_count < bt_loot->iron) {
			continue;
		}
		if (gold_count < bt_loot->gold) {
			continue;
		}
		if (tnt_count < bt_loot->tnt) {
			continue;
		}
		if (emerald_count < bt_loot->emerald) {
			continue;
		}
		if (diamond_count < bt_loot->diamond) {
			continue;
		}
		if (food_count < bt_loot->food) {
			continue;
		}
		
		seed_info->buried_treasures[viable_bts] = bt_pos;
		viable_bts++;
	}

	seed_info->bt_count = viable_bts;

	if (viable_bts < bt_minimum) {
		return 0;
	}

	return 1;
}