#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "logic/logic.h"
#include "logic/seedinfodump.h"

#include "util/filter.h"
#include "util/mathutils.h"
#include "util/timedelta.h"

#include "submodules/sfmt/SFMT.h"

#define LOCK_THREADS pthread_mutex_lock(&mutex)
#define UNLOCK_THREADS pthread_mutex_unlock(&mutex)

// Global variable for the filter data, accessible to all threads
Filter* filter;

int64_t seeds_found = 0; // Total seeds that passed all the filters
int64_t seeds_checked = 0; // Total seeds ran through the filters
int64_t seeds_discarded = 0; // Total seeds discarded, counts all sister seeds

// Mutex for editing the above three variables, printing seeds and seed info, etc
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// TODO: revisit this limit and figure out how to automatically determine whether
// lower or higher is better for performance depending on the filter parameters
// (optimize seeds output per second)
int64_t biome_fail_limit = 100;

sfmt_t sfmt;

void* seedfinding_thread(void* seed_info_pointer) {
	FILE* seed_info_file = (FILE*) seed_info_pointer;

	// Set up the generator object for this MC version
	Generator world_gen;
	setupGenerator(&world_gen, filter->mc_version, 0);

	// Struct holding seed data / filter triggers on the current seed
	SeedInfo* seed_info = malloc(sizeof(SeedInfo));
	SeedInfo* temp_info = malloc(sizeof(SeedInfo));

	int found_structure_seed = 0;
	int consecutive_biome_fails = 0;

	int64_t lower48;
	int64_t upper16;
	int64_t seed;

	while (seeds_found < filter->seed_requirement) {
		seed = sfmt_genrand_uint64(&sfmt);

		LOCK_THREADS;
			if (seeds_found >= filter->seed_requirement) {
				break;
			}

			seeds_checked++;
			seeds_discarded++;
		UNLOCK_THREADS;

		// Structure based checks
		if (!found_structure_seed) {

			// reset saved structure info
			clear_info(seed_info);

			// get lower 48 bits of the newly generated seed
			lower48 = seed & 0xFFFFFFFFFFFFLL;

			int result = structureFilterLogic(lower48, seed_info, filter);

			if (result > 0) {
				found_structure_seed = 1;
				consecutive_biome_fails = 0;
			}
		} 
		
		// Biome based checks
		if (found_structure_seed) {
			
			// get upper 16 bits of the newly generated seed
			upper16 = seed >> 48;

			// combine the upper and lower bits to make the full seed
			seed = lower48 | (upper16 << 48);

			// use a temporary seed_info variable because it might get modified during biome checks
			// therefore we ensure the original is preserved if we must run many biome checks in a row
			*temp_info = *seed_info;

			int result = biomeFilterLogic(seed, temp_info, filter, &world_gen);

			// Seed failed a check that fails for every structure seed (e.g. bastion basalt)
			if (result < 0) {
				found_structure_seed = 0;
			
			// Seed has failed a biome check
			} else if (result == 0) {
				consecutive_biome_fails++;

				// If enough consecutive biome checks fail, assume the lower48 is no good and scrap it
				if (consecutive_biome_fails > biome_fail_limit) {
					consecutive_biome_fails = 0;
					found_structure_seed = 0;

				} else {
					continue;
				}

			// Seed passed the biome filter and is done with the filtering process
			} else {

				LOCK_THREADS;
					if (seeds_found < filter->seed_requirement) {
						printf("%lld\n", seed);

						if (filter->output_seed_info) {
							*seed_info = *temp_info;
							write_seed_info(seed, seed_info, filter, seed_info_file);
						}

						seeds_found++;
						
					} else {
						// don't count seeds parsed after filtering ended
						// even if those seeds were valid outputs
						seeds_checked--;
						seeds_discarded--;
						break;
					}
					
				UNLOCK_THREADS;

				// after finding a seed, continue normally
				found_structure_seed = 0;
				continue;
			}
		}

		LOCK_THREADS;
			// potential breakpoint for the thread if filtering's already ended
			if (seeds_found >= filter->seed_requirement) {
				// don't count seeds parsed after filtering ended
				seeds_checked--;
				seeds_discarded--;
				break;
			}

			// this point is only reached if this structure seed has been discarded
			seeds_discarded = 65536 * (seeds_discarded / 65536 + 1);
		UNLOCK_THREADS;
	}

	free(seed_info);
	UNLOCK_THREADS;
}

int main(int argc, char *argv[]) {
	int thread_count;

	if (argc > 1) {
		thread_count = strtol(argv[1], &(argv[1]), 10);

		// TODO: reimplement this limit somehow maybe
		if (thread_count > 20) {
			thread_count = 20;
		}
		
	} else {
		thread_count = 1;
	}

	// Read filter code in its original string form
	FILE* filter_code_file = fopen("data/filter.txt", "r");
	char* filter_code = 0;
	int length;
	
	if (filter_code_file) {
		fseek(filter_code_file, 0, SEEK_END);
		length = ftell(filter_code_file);
		fseek(filter_code_file, 0, SEEK_SET);

		filter_code = malloc(length);
		if (filter_code) {
			size_t _ = fread(filter_code, 1, length, filter_code_file);
		}

		fclose(filter_code_file);
	}

	if (!filter_code) {
		printf("Failed to read filter code from filter.txt");
		return 1;
	}

	filter = malloc(sizeof(Filter));

	if (!parse_filter_code(filter, filter_code)) {
		printf("Error in the filter code! Make sure it's formatted correctly, try not to manually edit filter.txt unless you know what you're doing!");
		return 1;
	}

	free(filter_code);

	FILE* seed_info_file = fopen("data/seedinfo.txt", "w");

	if (!seed_info_file) {
		printf("Cannot open the seed info file!");
		return 1;
	}

	initBiomes();

	printf("Starting to filter...\n");
	printf("Selected thread count: %d\n", thread_count);

	struct timespec start, finish, delta;
	clock_gettime(CLOCK_REALTIME, &start);
	sfmt_init_gen_rand(&sfmt, start.tv_nsec);

	pthread_t threads[thread_count];

	for (int i = 0; i < thread_count; ++i) {
		pthread_create(&threads[i], NULL, seedfinding_thread, (void*) seed_info_file);
	}

	for (int i = 0; i < thread_count; ++i) {
		pthread_join(threads[i], NULL);
	}

	if (filter->output_seed_info) {
		fprintf(seed_info_file, "END_SEEDS");
	}
	
	fclose(seed_info_file);

	clock_gettime(CLOCK_REALTIME, &finish);
	sub_timespec(start, finish, &delta);

	double elapsed = delta.tv_sec + (double)delta.tv_nsec/1000000000L;

	printf("\nEnded filter. Stats:\n");
	printf("-> Output %lld seeds (%.2f per second)\n", seeds_found, seeds_found/elapsed);
	printf("-> Checked %lld seeds (%.2f per second)\n", seeds_checked, seeds_checked/elapsed);
	printf("-> Exhausted %lld seeds (%.2f per second)\n", seeds_discarded, seeds_discarded/elapsed);
	printf("-> Total time: %f seconds\n", elapsed);
	printf("-> %.9f%% of seeds checked succeeded\n", 100 * (double)seeds_found / seeds_checked);

	if (filter->SHIPWRECK) free(filter->ship_loot);
	if (filter->BURIED_TREASURE) free(filter->bt_loot);
	if (filter->DESERT_TEMPLE) free(filter->dt_loot);
	if (filter->RUINED_PORTAL) free(filter->rp_loot);

	free(filter);

	return 0;
}