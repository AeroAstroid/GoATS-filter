#include <stdint.h>

#include "../util/filter.h"
#include "../util/mathutils.h"

#include "../submodules/cubiomes/finders.h"
#include "../submodules/cubiomes/generator.h"
#include "../submodules/cubiomes/rng.h"

// Finds a desert temple within a certain distance of origin
int desert_temple_pos(int64_t lower48, SeedInfo* seed_info, int range, int quadrants, int mc_version) {
	int q = -1;
	
	// Iterate through regions in X and Z
	for (int rX = -1; rX <= 0; rX++) for (int rZ = -1; rZ <= 0; rZ++) {
		q++;

		if (!get_bit(quadrants, q)) {
			continue;
		}

		Pos dt_pos;
		
		if (!getStructurePos(Desert_Pyramid, mc_version, lower48, rX, rZ, &dt_pos)) {
			continue;
		}

		if (dt_pos.x > range*16 || dt_pos.x < -range*16) {
			continue;
		}

		if (dt_pos.z > range*16 || dt_pos.z < -range*16) {
			continue;
		}

		seed_info->desert_temples[seed_info->dt_count] = dt_pos;
		seed_info->dt_count++;
	}

	if (seed_info->dt_count) {
		return 1;
	}

	return 0;
}

// Biome check for a potential desert temple
int desert_temple_biome(int64_t seed, SeedInfo* seed_info, Generator* world_gen) {
	int viable_dts = 0;

	for (int dt = 0; dt < seed_info->dt_count; dt++) {
		Pos dt_pos = seed_info->desert_temples[dt];

		if(isViableStructurePos(Desert_Pyramid, world_gen, dt_pos.x, dt_pos.z, 0)) {
			seed_info->desert_temples[viable_dts] = dt_pos;
			viable_dts++;
		}
	}

	if (viable_dts) {
		seed_info->dt_count = viable_dts;
		return 1;
	}

	return 0;
}

int desert_temple_loot(int64_t lower48, SeedInfo* seed_info, DTLootRequirements* dt_loot, int mc_version) {
	int viable_dts = 0;

	for (int dt = 0; dt < seed_info->dt_count; dt++) {
		int64_t finder_seed = lower48 ^ 0x5DEECE66DUL;

		//1.13+
		long carveX = nextLong(&finder_seed) | 1L;
		long carveZ = nextLong(&finder_seed) | 1L;

		Pos dt_pos = seed_info->desert_temples[dt];
		
		finder_seed = ((long)dt_pos.x * carveX + (long)dt_pos.z * carveZ ^ lower48) & 0xFFFFFFFFFFFFLL;

		int salt = mc_version >= MC_1_16 ? 40003 : 30002;
		finder_seed = (finder_seed + salt) ^ 0x5DEECE66DUL;
		
		int64_t loot_table_seeds[4];

		for (int i = 0; i < 4; i++) {
			// Fill in each loot table seed while sequentially updating finder_seed
			loot_table_seeds[i] = (int64_t)nextLong(&finder_seed) ^ 0x5DEECE66DUL;
		}

		int iron_count = 0;
		int gold_count = 0;
		int diamond_count = 0;
		int emerald_count = 0;
		int gp_count = 0;
		int god_apple_count = 0;

		// roll for each of the 4 chests
		for (int chest = 0; chest < 4; chest++) {
			int skip_chest = 0;

			// Loot pool 1
			int loot_table_1[] = {
			//	wei	min	max
				5,	1,	3,	// diamond
				15,	1,	5,	// iron
				15,	2,	7,	// gold
				15,	1,	3,	// emerald
				25,	4,	6,	// bone
				25,	1,	3,	// spider eye
				25,	3,	7,	// rotten flesh
				// 0-6 are items with variable stack sizes
				// 7+ don't roll stack sizes
				20,	0,	0,	// saddle
				15,	0,	0,	// iron horse armor
				10,	0,	0,	// golden horse armor
				5,	0,	0,	// diamond horse armor
				20,	0,	0,	// enchanted book (11)
				20,	0,	0,	// golden apple
				2,	0,	0,	// god apple
				15,	0,	0	// nothing
			};

			/*int enchant_max_level[] = {
				4,	4,	4,	4,	4,	3,	1,	3,
				3,	// 1.8+
				2,	// 1.9+
				1,	// 1.11+
				3,	// 1.16+
				5,	5,	5,	2,	2,	3,
				3,	// 1.11+
				5,	1,	3,	3,	5,	2,	1,	1,
				3,	3,	// 1.8+
				3,	5,	3,	1,	// 1.13+
				1,	3,	4,	// 1.14+
				1,	1
			};*/

			int loot_rolls = 2 + nextInt(&loot_table_seeds[chest], 3); // 2-4 calls to loot table 1

			for (int roll = 0; roll < loot_rolls; roll++) {
				int loot_value = nextInt(&loot_table_seeds[chest], 232);
				int item_id;

				for (item_id = 0; loot_value > 0; ) {
					loot_value -= loot_table_1[3 * item_id];

					if (loot_value >= 0) {
						item_id++;
					}
				}

				int item_count;

				if (item_id < 7) {
					int min_count = loot_table_1[3 * item_id + 1];
					int max_count = loot_table_1[3 * item_id + 2];

					item_count = min_count + nextInt(&loot_table_seeds[chest], max_count - min_count + 1);
					
				} else {
					item_count = 1;
				}

				if (item_id == 11) {
					// book enchantments are VERY weird
					// as of right now the exact behavior of minecraft's book enchanting is not understood(?)
					// not even java featureutils or mc_feature_java get it right
					
					// I'm taking advantage of how I've made the loot filter - it always looks for AT LEAST
					// a certain amount of each item, therefore if I just give up on this chest after the first enchanted
					// book call (because the following calls can't be trusted), I'll never be undercounting

					// it's slightly less efficient, but keeps the false-positive rate at 0%.

					skip_chest = 1;
					break;

					/*
					int enchant_number = nextInt(&loot_table_seeds[chest], 38);
					int max_level = enchant_max_level[enchant_number];

					if (max_level > 1) {
						int level = 1 + nextInt(&loot_table_seeds[chest], max_level);
					}
					*/
				}

				if (item_id == 13) {
					god_apple_count++;
				}

				if (item_id == 0) {
					diamond_count += item_count;
				}
				if (item_id == 1) {
					iron_count += item_count;
				}
				if (item_id == 2) {
					gold_count += item_count;
				}
				if (item_id == 3) {
					emerald_count += item_count;
				}
			}

			if (skip_chest) {
				continue;
			}

			// Loot pool 2
			int loot_table_2[] = {
			//	wei	min	max
				10,	1,	8,	// bone
				10,	1,	8,	// gunpowder
				10,	1,	8,	// rotten flesh
				10,	1,	8,	// string
				10,	1,	8	// sand
			};

			loot_rolls = 4; // Always 4 calls to loot table 2

			for (int roll = 0; roll < loot_rolls; roll++) {
				int loot_value = nextInt(&loot_table_seeds[chest], 50);
				int item_id;

				for (item_id = 0; loot_value > 0; ) {
					loot_value -= loot_table_2[3 * item_id];

					if (loot_value >= 0) {
						item_id++;
					}
				}

				int item_count;

				int min_count = loot_table_2[3 * item_id + 1];
				int max_count = loot_table_2[3 * item_id + 2];

				item_count = min_count + nextInt(&loot_table_seeds[chest], max_count - min_count + 1);

				if (item_id == 1) {
					gp_count += item_count;
				}
			}
		}

		if (iron_count < dt_loot->iron) {
			continue;
		}
		if (gold_count < dt_loot->gold) {
			continue;
		}
		if (diamond_count < dt_loot->diamond) {
			continue;
		}
		if (emerald_count < dt_loot->emerald) {
			continue;
		}
		if (gp_count < dt_loot->gunpowder) {
			continue;
		}
		if (god_apple_count < dt_loot->god_apple) {
			continue;
		}

		seed_info->desert_temples[viable_dts] = dt_pos;
		viable_dts++;
	}

	seed_info->dt_count = viable_dts;

	if (viable_dts) {
		return 1;
	}

	return 0;
}