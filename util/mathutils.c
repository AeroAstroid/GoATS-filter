#include <stdint.h>
#include "../submodules/cubiomes/rng.h"

void shuffle(uint64_t *seed, int *ind, int n_ind) {
    for (int i = n_ind; i > 1; i--) {
        int temp = ind[i-1];
        int chosen_ind = nextInt(seed, i);

        ind[i-1] = ind[chosen_ind];
        ind[chosen_ind] = temp;
    }
}

long dist_sq(long x1, long z1, long x2, long z2) {
	return (x1 - x2) * (x1 - x2) + (z1 - z2) * (z1 - z2);
}

int get_bit(int n, int bit) {
	return (n & (1 << bit)) >> bit;
}