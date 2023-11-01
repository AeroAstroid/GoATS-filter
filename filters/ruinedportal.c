#include <stdint.h>
#include "../libs/cubiomes/finders.h"
#include "../module/filter.h"

// Ruined portal within max_dist of spawn in pos pos region
int ruined_portal_pos(int64_t seed, SeedInfo* seed_info, int max_dist, int mc_version) {
	Pos rp_pos;
	
	// Use return code to deduce success
	if (!getStructurePos(Ruined_Portal, mc_version, seed, 0, 0, &rp_pos)) {
		return 0;
	}

	if (rp_pos.x > max_dist*16 || rp_pos.x < 0) {
		return 0;
	}

	if (rp_pos.z > max_dist*16 || rp_pos.z < 0) {
		return 0;
	}

	long chunkX = rp_pos.x >> 4;
	long chunkZ = rp_pos.z >> 4;

	int64_t finder_seed = seed ^ 0x5DEECE66DL;
	int64_t carveX = nextLong(&finder_seed);
	int64_t carveZ = nextLong(&finder_seed);

	finder_seed = ((chunkX * carveX) ^ (chunkZ * carveZ) ^ seed) ^ 0x5DEECE66DL;
	finder_seed = finder_seed & 0xFFFFFFFFFFFF;

	// Roll for whether or not the portal is buried (50/50)
	if (nextFloat(&finder_seed) < 0.5) {
		return 0;
	}

	seed_info->ruined_portal = rp_pos;
	seed_info->rp_count = 1;

	return 1;
}

// Returns only portal types with a substantial amount of lava
int lava_portal(int64_t seed, SeedInfo* seed_info) {
	long chunkX = seed_info->ruined_portal.x >> 4;
	long chunkZ = seed_info->ruined_portal.z >> 4;

	int64_t finder_seed = seed ^ 0x5DEECE66DL;

	long carveX = nextLong(&finder_seed);
	long carveZ = nextLong(&finder_seed);
	
	finder_seed = ((chunkX * carveX) ^ (chunkZ * carveZ) ^ seed) ^ 0x5DEECE66DL;
	finder_seed = finder_seed & 0xFFFFFFFFFFFFL;

	nextFloat(&finder_seed); // Underground/aboveground roll (already checked in the position function)
	nextFloat(&finder_seed); // Air pocket roll (not relevant)

	// Portal size roll
	if (nextFloat(&finder_seed) < 0.05) {
		return 1; // All 3 big portals have lava, so pass this instantly
	}

	// Portal type roll for the small portals
	int portal_type = nextInt(&finder_seed, 10); 

	if (portal_type == 0 || portal_type == 2 || portal_type == 3 ||
		portal_type == 4 || portal_type == 5 || portal_type == 8) {
		return 0; // These small portal types don't have lava
	}

	return 1;
}

// Ruined portal biome check
int portal_biome(int64_t seed, SeedInfo* seed_info, int mc_version, Generator* world_gen) {
	Pos rp_pos = seed_info->ruined_portal;

	int biome = getBiomeAt(world_gen, 1, rp_pos.x, 63, rp_pos.z);

	if (isOceanic(biome) || areSimilar(mc_version, biome, desert) ||
	areSimilar(mc_version, biome, snowy_tundra) || 
	areSimilar(mc_version, biome, jungle) || biome == swamp
	) {
		return 0; // these biomes mess with ruined portal behavior
	}

	return 1;
}

// Generate the loot in the ruined portal chest
int portal_loot(int64_t lower48, SeedInfo* seed_info, RPLootRequirements* rp_loot) {
	int64_t finder_seed = (lower48) ^ 0x5DEECE66DL;

	long carveX = nextLong(&finder_seed) | 1;
	long carveZ = nextLong(&finder_seed) | 1;

	Pos rp_pos = seed_info->ruined_portal;

	finder_seed = ((long)rp_pos.x * carveX + (long)rp_pos.z * carveZ) ^ lower48 & 0xFFFFFFFFFFFFLL;
	finder_seed = (finder_seed + 40005) ^ 0x5DEECE66DL;

	int64_t loot_table_seed = (int64_t)nextLong(&finder_seed) ^ 0x5DEECE66DL;

	int iron_nugget_count = 0;
	int gold_nugget_count = 0;
	int has_light = 0;
	int flint_count = 0;
	int obby_count = 0;
	int looting_level = 0;

	int loot_table[100] = { // 25 possible items
	//	wei	cls	min	max		wei	cls	min	max		wei	cls	min	max		wei	cls	min	max		wei	cls	min	max
		40,	1,	1,	2,		40,	1,	1,	4,		40,	1,	9,	18,		40,	0,	0,	0,		40,	0,	0,	0,
		15,	0,	0,	0,		15,	1,	4,	24,		15,	2,	0,	0,		15,	2,	1,	0,		15,	2,	2,	0,
		15,	2,	3,	0,		15,	2,	4,	0,		15,	2,	5,	0,		15,	2,	6,	0,		15,	2,	7,	0,
		15,	2,	8,	0,		5,	1,	4,	12,		5,	0,	0,	0,		5,	0,	0,	0,		5,	1,	4,	12,
		5,	0,	0,	0,		5,	1,	2,	8,		1,	0,	0,	0,		1,	0,	0,	0,		1,	1,	1,	2
	};

	// holds the number of enchants (e_n) each enchantable item can have
	// for each item, list all the single-level enchant IDs for which the level call must be ignored
	int enchant_table[] = {
	//	e_n		1-level enchants
		10,		8,	9,	-1,	-1,		// gold sword
		9,		4,	7,	8,	-1,		// gold axe
		6,		1,	4,	5,	-1,		// gold hoe
		6,		1,	4,	5,	-1,		// gold shovel
		6,		1,	4,	5,	-1,		// gold pick
		13,		8,	11,	12,	-1,		// gold boots
		9,		5,	7,	8,	-1,		// gold chestplate
		11,		5,	7,	9,	10,		// gold helmet
		9,		5,	7,	8,	-1		// gold leggings
	};

	int loot_rolls = 4 + nextInt(&loot_table_seed, 5); // 4-8 loot rolls

	for (int roll = 0; roll < loot_rolls; roll++) {
		// Random number that determines the item to be picked
		int loot_value = nextInt(&loot_table_seed, 398);
		int item_id;

		// Traverse the table item weights to find the corresponding item
		for (item_id = 0; loot_value > 0; ) {
			loot_value = loot_value - loot_table[4 * item_id];

			if (loot_value >= 0) {
				item_id++;
			}
		}

		int roll_class = loot_table[4 * item_id + 1];
		int item_count;

		// CLASS 0 ITEMS
		if (roll_class == 0) {
			item_count = 1; // Class 0 always rolls 1 item at a time

			// Flint and steel or fire charge
			if (item_id == 3 || item_id == 4) {
				has_light = 1;
			}

			continue;
		}

		// CLASS 1 ITEMS
		if (roll_class == 1) {
			int min_count = loot_table[4 * item_id + 2];
			int max_count = loot_table[4 * item_id + 3];
			item_count = min_count + nextInt(&loot_table_seed, max_count - min_count + 1);

			if (item_id == 0) { // Obsidian
				obby_count += item_count;
			}

			if (item_id == 1) { // Flint
				flint_count += item_count;
			}

			if (item_id == 2) { // Iron nuggets
				iron_nugget_count += item_count;
			}

			if (item_id == 6) { // Gold nuggets
				gold_nugget_count += item_count;
			}

			if (item_id == 21) { // Gold ingots
				gold_nugget_count += 9 * item_count;
			}

			if (item_id == 24) { // Gold blocks
				gold_nugget_count += 81 * item_count;
			}
			
			continue;
		}

		// CLASS 2 ITEMS
		if (roll_class == 2) {
			int e_n_index = 5 * loot_table[4 * item_id + 2];
			int enchant_id = nextInt(&loot_table_seed, enchant_table[e_n_index]);

			if (enchant_id != enchant_table[e_n_index + 1] &&
				enchant_id != enchant_table[e_n_index + 2] &&
				enchant_id != enchant_table[e_n_index + 3] &&
				enchant_id != enchant_table[e_n_index + 4]) {
				
				// not every enchant maxes at level 3 but we only care about looting anyway
				int enchant_level = nextInt(&loot_table_seed, 3) + 1;

				// Gold sword with looting
				if (item_id == 7 && enchant_id == 5) {
					looting_level = enchant_level;
				}
			}
		}
	}
	
	int iron_ingot_count = iron_nugget_count / 9;
	int gold_ingot_count = gold_nugget_count / 9;

	if (rp_loot->light) {
		// If there's no light, but one can be made with an RP iron, remove an iron
		if (has_light == 0 && flint_count > 0 && iron_ingot_count > 0) {
			iron_ingot_count--;
			has_light = 1;
		}

		if (!has_light) {
			return 0;
		}
	}

	if (iron_ingot_count < rp_loot->iron) {
		return 0;
	}

	if (gold_ingot_count < rp_loot->gold) {
		return 0;
	}

	if (obby_count < rp_loot->obby) {
		return 0;
	}

	if (looting_level < rp_loot->looting) {
		return 0;
	}

	return 1;
}