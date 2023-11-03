#include <stdint.h>

#include "../util/filter.h"
#include "../util/mathutils.h"

#include "../submodules/cubiomes/finders.h"
#include "../submodules/cubiomes/generator.h"
#include "../submodules/cubiomes/rng.h"

int shipwreck_type(int64_t lower48, SeedInfo* seed_info, int allowed_types) {
	int viable_ships = 0;

	for (int i = 0; i < seed_info->ship_count; i++) {
		Pos ship_pos = seed_info->shipwrecks[i];

		int chunkX = ship_pos.x >> 4;
		int chunkZ = ship_pos.z >> 4;

		int64_t finder_seed = lower48 ^ 0x5DEECE66DLL;

		int64_t carveX = nextLong(&finder_seed);
		int64_t carveZ = nextLong(&finder_seed);

		finder_seed = ((chunkX * carveX) ^ (chunkZ * carveZ) ^ lower48) ^ 0x5DEECE66DLL;
		finder_seed = finder_seed & 0xFFFFFFFFFFFFLL;

		int rotation = nextInt(&finder_seed, 4);

		// there are actually 20 ship types and not 10, but the degraded ones (10-19)
		// are functionally equal to their respective non-degraded counterparts (0-9)
		int ship_type = nextInt(&finder_seed, 10);

		// Piece configuration of every ship type
		// 0 = full, 1 = back half, 2 = front half
		int type_pieces[] = {
			0, 0, 2, 1, 0, 2, 1, 0, 2, 1
		};

		if (allowed_types & (1 << type_pieces[ship_type])) {
			seed_info->shipwrecks[viable_ships] = ship_pos;
			seed_info->ship_rotations[viable_ships] = rotation;
			seed_info->ship_types[viable_ships] = ship_type;
			viable_ships++;
		}
	}

	seed_info->ship_count = viable_ships;
	if (viable_ships) {
		return 1;
	}
	
	return 0;
}

int shipwreck_pos(int64_t lower48, SeedInfo* seed_info, int range, int quadrants, int mc_version) {
	int q = -1;
	
	// Iterate through regions in X and Z
	for (int rX = -1; rX <= 0; rX++) for (int rZ = -1; rZ <= 0; rZ++) {
		q++;

		if (!get_bit(quadrants, q)) {
			continue;
		}

		Pos ship_pos;

		if (!getStructurePos(Shipwreck, mc_version, lower48, rX, rZ, &ship_pos)) {
			continue;
		}

		if (ship_pos.x > range*16 || ship_pos.x < -range*16) {
			continue;
		}

		if (ship_pos.z > range*16 || ship_pos.z < -range*16) {
			continue;
		}

		seed_info->shipwrecks[seed_info->ship_count] = ship_pos;
		seed_info->ship_count++;
	}

	if (seed_info->ship_count) {
		return 1;
	}

	return 0;
}

int shipwreck_biome(int64_t seed, SeedInfo* seed_info, Generator* world_gen) {
	int viable_ships = 0;

	for (int s = 0; s < seed_info->ship_count; s++) {
		Pos ship_pos = seed_info->shipwrecks[s];

		int biome = getBiomeAt(world_gen, 4, (ship_pos.x >> 4) + 2, 63, (ship_pos.z >> 4) + 2);

		if (isOceanic(biome)) {
            seed_info->shipwrecks[viable_ships] = ship_pos;
			// we could also move the rotation and type info for the ship here but we don't need to
            viable_ships++;
        }
	}

	seed_info->ship_count = viable_ships;
	if (viable_ships) {
		return 1;
	}

	return 0;
}

int shipwreck_loot(int64_t lower48, SeedInfo* seed_info, ShipLootRequirements* ship_loot, int mc_version) {
	int salt = mc_version < MC_1_16 ? 30005 : 40006;

	// The offset of the chunk the treasure chest is located in relative to the main chunk
	// for each ship type and rotation. Necessary to know which chunk the chest is in
	int treasure_offset_table[] = {
	//	rot0	rot1	rot2	rot3
	//	x, z	x, z,	x, z,	x, z
		0, 1,	-1, 1,	0, 0, 	0, 0,
		0, 1,	-1, 0,	0, 0, 	0, 1,
		0, 0,	0, 0,	0, 0,	0, 0,	// NO CHEST
		0, 0,	0, 0,	0, 1,	0, 1,
		0, 1,	-1, 0,	0, 0,	0, 1,
		0, 0,	0, 0,	0, 0,	0, 0,	// NO CHEST
		0, 0,	0, 0,	0, 1,	0, 1,
		0, 1,	-1, 1,	0, 0, 	0, 0,
		0, 0,	0, 0,	0, 0,	0, 0,	// NO CHEST
		0, 0,	0, 1,	0, 1, 	0, 0
	};

	// Same thing as above but with the food chest
	// These two tables could potentially be simplified a bit but there's no clean way to do that
	// Plus, performance > memory anyway
	int food_offset_table[] = {
	//	rot0	rot1	rot2	rot3
	//	x, z	x, z,	x, z,	x, z
		0, 0,	0, 0,	0, 1, 	-1, 0,
		0, 0,	0, 0,	0, 1, 	-1, 0,
		0, 0,	0, 0,	0, 1, 	-1, 0,
		0, 0,	0, 0,	0, 0,	0, 0,	// NO CHEST
		0, 0,	0, 1,	0, 1, 	-1, 0,
		0, 0,	0, 1,	0, 1, 	-1, 0,
		0, 0,	0, 0,	0, 0,	0, 0,	// NO CHEST
		0, 0,	0, 0,	0, 1, 	-1, 0,
		0, 0,	0, 0,	0, 1, 	-1, 0,
		0, 0,	0, 0,	0, 0,	0, 0	// NO CHEST
	};

	// Depending on chest generation order within the chunk + other factors
	// the amount of useless calls we must skip to simulate the treasure chest can be different
	// This table encodes how many calls to skip for each ship type and rotation
	int treasure_skip_calls_116[] = {
	//	r0	r1	r2	r3
		6,	2,	6,	6,
		4,	2,	4,	4,
		0,	0,	0,	0,	// NO T.CHEST
		4,	4,	4,	2,
		4,	2,	4,	2,
		0,	0,	0,	0,	// NO T.CHEST
		4,	2,	4,	2,
		6,	2,	6,	6,
		0,	0,	0,	0,	// NO T.CHEST
		6,	6,	6,	2
	};
	// For pre-1.16 the table is remapped: any 6-call rotations get reduced to 2
	// and any 4 and 2-call rotations get reduced to 0. Not entirely sure why

	// The food chest always requires 2 skipped calls exactly in 1.16 and 0 before then

	// Piece configuration of every ship type
	// 0 = full, 1 = back half, 2 = front half
	int type_pieces[] = {
		0, 0, 2, 1, 0, 2, 1, 0, 2, 1
	};
	
	int viable_ships = 0;

	for (int ship = 0; ship < seed_info->ship_count; ship++) {

		Pos ship_pos = seed_info->shipwrecks[ship];
		int ship_rot = seed_info->ship_rotations[ship];
		int ship_type = seed_info->ship_types[ship];

		int carrot_count = 0;
		int wheat_count = 0;
		int tnt_count = 0;

		int iron_count = 0;
		int gold_count = 0;
		int emerald_count = 0;
		int diamond_count = 0;

		// FRONT HALF CHEST LOOT (FOOD CHEST)
		if (type_pieces[ship_type] != 1) {
			int64_t finder_seed = (lower48) ^ 0x5DEECE66DLL;

			int64_t carveX = nextLong(&finder_seed) | 1;
			int64_t carveZ = nextLong(&finder_seed) | 1;

			int x_off = food_offset_table[ship_type * 8 + ship_rot * 2];
			int z_off = food_offset_table[ship_type * 8 + ship_rot * 2 + 1];

			finder_seed = (int64_t)(ship_pos.x + x_off * 16) * carveX + (int64_t)(ship_pos.z + z_off * 16) * carveZ;
			finder_seed = finder_seed ^ lower48 & 0xFFFFFFFFFFFFLL;
			finder_seed = (finder_seed + salt) ^ 0x5DEECE66DLL;

			// amount of skipped calls changes with version but is constant w/ ship type
			int skipped_calls = (mc_version < MC_1_16) ? 0 : 2;

			skipNextN(&finder_seed, skipped_calls);

			int64_t loot_table_seed = (int64_t)nextLong(&finder_seed) ^ 0x5DEECE66DLL;

			// Food chest loot table
			int loot_table[] = {
			//	wei	min	max
				8,	1,	12,	// paper
				7,	2,	6,	// potato
				7,	1,	4,	// moss (1.17+)
				7,	2,	6,	// poisoned potato
				7,	4,	8,	// carrot
				7,	8,	21,	// wheat
				10,	0,	0,	// suspicious stew
				6,	2,	8,	// coal
				5,	5,	24,	// rotten flesh
				2,	1,	3,	// pumpkin
				2,	1,	3,	// bamboo
				3,	1,	5,	// gunpowder
				1,	1,	2,	// tnt
				3,	0,	0,	// leather helmet
				3,	0,	0,	// leather chestplate
				3,	0,	0,	// leather leggings
				3,	0,	0	// leather boots
			};

			// sum of all item weights
			int food_chest_weight = 84;

			if (mc_version < MC_1_17) {
				// remove moss from the possible outcomes
				food_chest_weight = 77;
				loot_table[3*2] = 0;
			}

			// holds the number of enchants (e_n) each enchantable item can have
			// for each item, list all the single-level enchant IDs for which the level call must be ignored
			int enchant_table[] = {
			//	e_n		1-level enchants
				11,		5,	7,	9,	10,		// leather helmet
				9,		5,	7,	8,	-1,		// leather chestplate
				9,		5,	7,	8,	-1,		// leather leggings
				13,		8,	11,	12,	-1		// leather boots
			};

			// for pre-1.16, the boot enchantments change due to no soul speed
			if (mc_version < MC_1_16) {
				enchant_table[5*3]--; // minus one to number of possible enchants
				enchant_table[5*3+2]--; // mending gets pushed back 1 ID since soul speed is removed
				enchant_table[5*3+3]--; // same with curse of vanishing
			}

			int loot_rolls = 3 + nextInt(&loot_table_seed, 8); // 3-10 calls to the table

			for (int roll = 0; roll < loot_rolls; roll++) {
				int loot_value = nextInt(&loot_table_seed, food_chest_weight);
				int item_id;

				for (item_id = 0; loot_value > 0; ) {
					loot_value -= loot_table[3 * item_id];

					if (loot_value >= 0) {
						item_id++;
					}
				}

				int item_count = 1;

				if (loot_table[3 * item_id + 1] != 0) {
					int min_count = loot_table[3 * item_id + 1];
					int max_count = loot_table[3 * item_id + 2];

					item_count = min_count + nextInt(&loot_table_seed, max_count - min_count + 1);
				}

				if (item_id == 6) { // suspicious stew
					skipNextN(&loot_table_seed, 2); // stew effect roll
				}

				if (item_id >= 13) {
					int e_n_index = 5 * (item_id - 13);
					int enchant_id = nextInt(&loot_table_seed, enchant_table[e_n_index]);

					if (enchant_id != enchant_table[e_n_index + 1] &&
						enchant_id != enchant_table[e_n_index + 2] &&
						enchant_id != enchant_table[e_n_index + 3] &&
						enchant_id != enchant_table[e_n_index + 4]) {
						
						skipNextN(&loot_table_seed, 1); // levels call, we don't care
					}
				}

				if (item_id == 4) {
					carrot_count += item_count;
				}

				if (item_id == 5) {
					wheat_count += item_count;
				}

				if (item_id == 12) {
					tnt_count += item_count;
				}
			}
		}

		// BACK HALF CHEST LOOT (TREASURE CHEST)
		if (type_pieces[ship_type] != 2) {
			int64_t finder_seed = (lower48) ^ 0x5DEECE66DLL;

			int64_t carveX = nextLong(&finder_seed) | 1;
			int64_t carveZ = nextLong(&finder_seed) | 1;

			int x_off = treasure_offset_table[ship_type * 8 + ship_rot * 2];
			int z_off = treasure_offset_table[ship_type * 8 + ship_rot * 2 + 1];

			finder_seed = (int64_t)(ship_pos.x + x_off * 16) * carveX + (int64_t)(ship_pos.z + z_off * 16) * carveZ;
			finder_seed = finder_seed ^ lower48 & 0xFFFFFFFFFFFFLL;
			finder_seed = (finder_seed + salt) ^ 0x5DEECE66DLL;

			// advance the appropriate number of calls depending on ship and version
			int skipped_calls = treasure_skip_calls_116[ship_type * 4 + ship_rot];

			if (mc_version < MC_1_16) { // remove calls for pre-1.16
				skipped_calls = (skipped_calls == 6) ? 2 : 0;
			}

			skipNextN(&finder_seed, skipped_calls);

			int64_t loot_table_seed = (int64_t)nextLong(&finder_seed) ^ 0x5DEECE66DLL;

			int iron_nugget_count = 0;
			int gold_nugget_count = 0;

			// Loot table 1: treasure
			int loot_table_1[] = {
			//	wei	min	max
				90,	1,	5,	// iron ingot
				10,	1,	5,	// gold ingot
				40,	1,	5,	// emerald
				5,	0,	0,	// diamond
				5,	0,	0,	// xp bottle
			};

			int loot_rolls = 3 + nextInt(&loot_table_seed, 4); // 3-6 calls to loot table 1

			for (int roll = 0; roll < loot_rolls; roll++) {
				int loot_value = nextInt(&loot_table_seed, 150);
				int item_id;

				for (item_id = 0; loot_value > 0; ) {
					loot_value -= loot_table_1[3 * item_id];

					if (loot_value >= 0) {
						item_id++;
					}
				}

				int item_count = 1;

				if (item_id < 3) {
					int min_count = loot_table_1[3 * item_id + 1];
					int max_count = loot_table_1[3 * item_id + 2];

					item_count = min_count + nextInt(&loot_table_seed, max_count - min_count + 1);
				}

				if (item_id == 0) {
					iron_count += item_count;
				}
				if (item_id == 1) {
					gold_count += item_count;
				}
				if (item_id == 2) {
					emerald_count += item_count;
				}
				if (item_id == 3) {
					diamond_count += item_count;
				}
			}

			// Loot table 2: nuggets and lapis
			int loot_table_2[] = {
			//	wei	min	max
				50,	1,	10,	// iron nugget
				10,	1,	10,	// gold nugget
				20,	1,	10,	// lapis
			};

			loot_rolls = 2 + nextInt(&loot_table_seed, 4); // 2-5 calls to loot table 2

			for (int roll = 0; roll < loot_rolls; roll++) {
				int loot_value = nextInt(&loot_table_seed, 80);
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
					iron_nugget_count += item_count;
				}
				if (item_id == 1) {
					gold_nugget_count += item_count;
				}
			}

			iron_count += iron_nugget_count / 9;
			gold_count += gold_nugget_count / 9;
		}

		if (iron_count < ship_loot->iron) {
			continue;
		}
		if (gold_count < ship_loot->gold) {
			continue;
		}
		if (emerald_count < ship_loot->emerald) {
			continue;
		}
		if (diamond_count < ship_loot->diamond) {
			continue;
		}
		if (carrot_count < ship_loot->carrot) {
			continue;
		}
		if (wheat_count < ship_loot->wheat) {
			continue;
		}
		if (tnt_count < ship_loot->tnt) {
			continue;
		}

		seed_info->shipwrecks[viable_ships] = ship_pos;
		// we could also move the rotation and type info for the ship here but we don't need to
		viable_ships++;
	}

	if (viable_ships) {
		seed_info->ship_count = viable_ships;
		return 1;
	}

	return 0;
}