#include <stdio.h>
#include "../util/filter.h"

void write_seed_info(int64_t seed, SeedInfo* seed_info, Filter* filter, FILE* seed_info_file) {
	fprintf(seed_info_file, "SEED\n%lld\n", seed);

	if (filter->SPAWN_POINT) {
		fprintf(seed_info_file, "SPAWN\n");
		fprintf(seed_info_file, "%d %d\n", seed_info->spawn_point.x, seed_info->spawn_point.z);
	}

	if (seed_info->bt_count) {
		fprintf(seed_info_file, "BT\n");

		for (int i = 0; i < seed_info->bt_count; i++) {
			fprintf(seed_info_file, "%d %d\n", seed_info->buried_treasures[i].x, seed_info->buried_treasures[i].z);
		}
	}

	if (seed_info->ship_count) {
		fprintf(seed_info_file, "SHIP\n");

		for (int i = 0; i < seed_info->ship_count; i++) {
			fprintf(seed_info_file, "%d %d\n", seed_info->shipwrecks[i].x, seed_info->shipwrecks[i].z);
		}
	}

	if (seed_info->rp_count) {
		fprintf(seed_info_file, "RUINED_PORTAL\n");

		for (int i = 0; i < seed_info->rp_count; i++) {
			fprintf(seed_info_file, "%d %d\n", seed_info->ruined_portal.x, seed_info->ruined_portal.z);
		}
	}

	if (seed_info->vill_count) {
		fprintf(seed_info_file, "VILLAGE\n");

		for (int i = 0; i < seed_info->vill_count; i++) {
			fprintf(seed_info_file, "%d %d\n", seed_info->villages[i].x, seed_info->villages[i].z);
		}
	}

	if (seed_info->jt_count) {
		fprintf(seed_info_file, "JUNGLE_TEMPLE\n");

		for (int i = 0; i < seed_info->jt_count; i++) {
			fprintf(seed_info_file, "%d %d\n", seed_info->jungle_temples[i].x, seed_info->jungle_temples[i].z);
		}
	}

	if (seed_info->dt_count) {
		fprintf(seed_info_file, "DESERT_TEMPLE\n");

		for (int i = 0; i < seed_info->dt_count; i++) {
			fprintf(seed_info_file, "%d %d\n", seed_info->desert_temples[i].x, seed_info->desert_temples[i].z);
		}
	}

	if (seed_info->ravine_count) {
		fprintf(seed_info_file, "MAGMA_RAVINE_START\n");

		for (int i = 0; i < seed_info->ravine_count; i++) {
			fprintf(seed_info_file, "%d %d\n", seed_info->magma_ravines[i].x, seed_info->magma_ravines[i].z);
		}
	}

	if (seed_info->pool_count) {
		fprintf(seed_info_file, "LAVA_POOL\n");

		for (int i = 0; i < seed_info->pool_count; i++) {
			fprintf(seed_info_file, "%d %d\n", seed_info->lava_pools[i].x, seed_info->lava_pools[i].z);
		}
	}

	if (seed_info->bast_count) {
		fprintf(seed_info_file, "BASTION\n");

		for (int i = 0; i < seed_info->bast_count; i++) {
			fprintf(seed_info_file, "%d %d\n", seed_info->bastions[i].x, seed_info->bastions[i].z);
		}
	}

	if (seed_info->fort_count) {
		fprintf(seed_info_file, "FORTRESS\n");

		for (int i = 0; i < seed_info->fort_count; i++) {
			fprintf(seed_info_file, "%d %d\n", seed_info->fortresses[i].x, seed_info->fortresses[i].z);
		}
	}

	fprintf(seed_info_file, "\n");
}