#include <stdio.h>
#include <stdint.h>
#include "../util/filter.h"
#include "../util/mathutils.h"

// Finds a fortress within a certain distance of origin
int fortress_pos(int64_t lower48, SeedInfo* seed_info, int range, int allowed_quadrants, int mc_version) {
	int q = -1;
	
	// Iterate through regions in X and Z
	for (int rX = -1; rX <= 0; rX++) for (int rZ = -1; rZ <= 0; rZ++) {
		q++;

		if (!get_bit(allowed_quadrants, q)) {
			continue;
		}

		Pos fort_pos;
		
		if (!getStructurePos(Fortress, mc_version, lower48, rX, rZ, &fort_pos)) {
			continue;
		}

		if (fort_pos.x > range*16 || fort_pos.x < -range*16) {
			continue;
		}

		if (fort_pos.z > range*16 || fort_pos.z < -range*16) {
			continue;
		}

		seed_info->fortresses[seed_info->fort_count] = fort_pos;
		seed_info->fort_count++;
	}

	if (seed_info->fort_count) {
		return 1;
	}

	return 0;
}

int double_spawner(int64_t lower48, SeedInfo* seed_info, int mc_version) {
	Piece* fort_pieces = malloc(sizeof(Piece) * 300);

	int double_spawner_forts = 0;

	for (int f = 0; f < seed_info->fort_count; f++) {
		Pos fort_pos = seed_info->fortresses[f];

		int n = getFortressPieces(fort_pieces, 300, mc_version, lower48, fort_pos.x >> 4, fort_pos.z >> 4);

		Pos3 spawners[2];
		int spawner_count = 0;

		for (int i = 0; i < n; i++) {
			Piece fort_piece = fort_pieces[i];

			// Blaze spawner fort piece
			if (fort_piece.type == 5) {
				spawners[spawner_count] = fort_piece.pos;
				spawner_count++;
			}
		}

		if (spawner_count == 2) {
			int distX = spawners[0].x - spawners[1].x;
			int distZ = spawners[0].z - spawners[1].z;

			if (distX > 12 || distX < -12) {
				continue;
			}
			if (distZ > 12 || distZ < -12) {
				continue;
			}

			seed_info->fortresses[double_spawner_forts] = fort_pos;
			double_spawner_forts++;
		}
	}

	free(fort_pieces);

	if (double_spawner_forts) {
		seed_info->fort_count = double_spawner_forts;
		return 1;
	}

	return 0;
}