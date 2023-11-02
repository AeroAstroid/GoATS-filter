#include <stdint.h>
#include <stdio.h>
#include "../submodules/cubiomes/finders.h"
#include "../util/mathutils.h"
#include "../util/filter.h"

// Finds whether there's a stronghold at the correct angle for FSG stronghold
// Technically also checks spawn dist for that SH instead of just angle. Overfiltering, but way faster than an arctan
// Only needs to check the first stronghold, hence doesn't require full seed(?)
int fsg_stronghold_angle(int64_t seed, SeedInfo* seed_info, int max_dist, int mc_version) {
	// signs for x/z in each region ->  0  |  1  |  2  |  3
	int x_mult[4] = {1, -1, 1, -1}; // pos | neg | pos | neg
	int z_mult[4] = {1, 1, -1, -1}; // pos | pos | neg | neg

	// Whether each region has an intended fortress
	int fort_r = 0b0000;

	for (int f = 0; f < seed_info->fort_count; f++) {
		int region = 0;

		if (seed_info->fortresses[f].x < 0) {
			region += 1;
		}

		if (seed_info->fortresses[f].z < 0) {
			region += 2;
		}

		fort_r |= (1 << region);
	}

	// If there's no intended fortress (i.e. fortress filtering is off)
	// Automatically default to pos-pos
	if (!fort_r) {
		fort_r = 1;
	}

	StrongholdIter sh_iterator;
	Pos pos_sh = initFirstStronghold(&sh_iterator, mc_version, seed);
	
	// Distances for each stronghold
	long dist1, dist2, dist3;
	
	for (int i = 0; i < 4; i++) {
		// if this region does not have an intended fortress, skip
		if (!get_bit(fort_r, i)) {
			continue;
		}
		
		dist1 = dist_sq(pos_sh.x, pos_sh.z, x_mult[i]*1200L, z_mult[i]*1200L);
		dist2 = dist_sq(pos_sh.x, pos_sh.z, -x_mult[i]*1639L, z_mult[i]*439L);
		dist3 = dist_sq(pos_sh.x, pos_sh.z, x_mult[i]*439L, -z_mult[i]*1639L);

		// if even 1 matching stronghold for one of the regions is found, it passes
		if (
			(dist1 <= max_dist * max_dist) ||
			(dist2 <= max_dist * max_dist) || 
			(dist3 <= max_dist * max_dist)) {
			return 1;
		}
	}

	return 0;
}

// Finds if a stronghold is within a certain distance of the FSG blind coords
// Requires checking 3 strongholds, hence requires full seed(?)
int fsg_stronghold_pos(int64_t seed, SeedInfo* seed_info, int max_dist, int ocean_check, int mc_version, Generator* world_gen) {
	// signs for x/z in each region ->  0  |  1  |  2  |  3
	int x_mult[4] = {1, -1, 1, -1}; // pos | neg | pos | neg
	int z_mult[4] = {1, 1, -1, -1}; // pos | pos | neg | neg

	// Whether each region has an intended fortress
	int fort_r = 0b0000;

	for (int f = 0; f < seed_info->fort_count; f++) {
		int region = 0;

		if (seed_info->fortresses[f].x < 0) {
			region += 1;
		}

		if (seed_info->fortresses[f].z < 0) {
			region += 2;
		}

		fort_r |= (1 << region);
	}

	// If there's no intended fortress (i.e. fortress filtering is off)
	// Automatically default to pos-pos
	if (!fort_r) {
		fort_r = 1;
	}

	StrongholdIter sh;
	Pos _ = initFirstStronghold(&sh, mc_version, seed);

	long sh_dist_sq = 0xFFFFFF;
	int dist = 0;
	Pos best_sh;

	for (int j = 0; j < 3; j++) {
		if (nextStronghold(&sh, world_gen) <= 0) {
			break;
		}

		for (int i = 0; i < 4; i++) {
			// if this region does not have an intended fortress, skip
			if (!get_bit(fort_r, i)) {
				continue;
			}
			
			dist = dist_sq(sh.pos.x, sh.pos.z, x_mult[i]*1200L, z_mult[i]*1200L);

			if (dist < sh_dist_sq) {
				sh_dist_sq = dist;
				best_sh = sh.pos;
			}
		}
	}

	if (sh_dist_sq > max_dist * max_dist) {
		return 0;
	}

	// *Might* be underfiltering to check biome only at the end (after only 1 candidate is left)
	// But only really applies for ridiculously impractical values of max_dist (like 900+) so whatever
	// Biome checks are very slow so it's worth the tiny risk of rolling a false negative
	if (ocean_check) {
		int sh_biome = getBiomeAt(world_gen, 1, best_sh.x, 63, best_sh.z);

		if (!isDeepOcean(sh_biome)) {
			return 0;
		}
	}

	return 1;
}

// Finds if a stronghold is within max_dist of the spawn
// Requires checking 3 strongholds, hence requires full seed(?)
int close_stronghold_pos(int64_t seed, int max_dist, int ocean_check, int mc_version, Generator* world_gen) {
	StrongholdIter sh;
	Pos _ = initFirstStronghold(&sh, mc_version, seed);

	for (int i = 0; i < 3; i++) {
		if (nextStronghold(&sh, world_gen) <= 0) {
			break;
		}
		
		// Distance matches
		if (sh.pos.x * sh.pos.x + sh.pos.z * sh.pos.z <= max_dist * max_dist) {

			if (ocean_check) { // Potential deep ocean check
				int sh_biome = getBiomeAt(world_gen, 1, sh.pos.x, 63, sh.pos.z);
				if (!isDeepOcean(sh_biome)) {
					return 0;
				}
			}

			return 1;
		}
	}

	return 0;
}