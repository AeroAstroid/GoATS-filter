long dist_sq(long x1, long z1, long x2, long z2) {
	return (x1 - x2) * (x1 - x2) + (z1 - z2) * (z1 - z2);
}

int get_bit(int n, int bit) {
	return (n & (1 << bit)) >> bit;
}