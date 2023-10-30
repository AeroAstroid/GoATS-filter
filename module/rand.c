#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "../include/gcrypt.h"
#include "../module/rand.h"

#define MAXBUFLEN 60000

uint64_t random_seed(gcry_cipher_hd_t cipher, int bytes, unsigned char* seed_buffer) {
	size_t b_size = (size_t)bytes;

	gcry_error_t gcryError = gcry_cipher_encrypt(cipher, seed_buffer, b_size, NULL, 0);

	if (gcryError) {
		printf("gcry_cipher_decrypt failed:  %s/%s\n", 
			gcry_strsource(gcryError),
			gcry_strerror(gcryError));
		return 0;
	}

	return charToNum(seed_buffer, b_size);
}

uint64_t gen_nonce() {
	FILE* fp = fopen("/dev/urandom", "r");

	int c;
	uint64_t rv = 0;

	for (int i = 0; i < sizeof(rv); i++) {
		do {
			c = fgetc(fp);
		} while (c < 0);

		rv = (rv << 8) | (c & 0xff);
	}
	
	fclose(fp);
	return rv;
}

void setkey(gcry_cipher_hd_t* gcryCipherHd) {
	uint64_t timestamp = (uint64_t)time(NULL);
	FILE *fp = fopen("main.c", "rb");
	char source[MAXBUFLEN + 1];

	if (fp != NULL) {
		size_t newLen = fread(source, sizeof(char), MAXBUFLEN, fp);
		if (ferror(fp) != 0) {
			fputs("Error reading file", stderr);
		} else {
			source[newLen++] = '\0';
		}
		fclose(fp);
	}
	
	unsigned char *salsaKey;
	unsigned char *salsaKey2;

	gcry_md_hd_t h;
	gcry_md_open(&h, GCRY_MD_SHA256, GCRY_MD_FLAG_SECURE);	// initialise the hash context
	gcry_md_write(h, source, strlen(source));	// hash some text
	salsaKey = gcry_md_read(h, GCRY_MD_SHA256);	// get the result

	unsigned char stampString[8];
	int64ToChar(stampString, timestamp);

	unsigned i;
	unsigned char longInput[41];
	for (i = 0; i < 32; i++) {
		longInput[i] = salsaKey[i];
	}
	for (i = 0; i < 8; i++) {
		longInput[32 + i] = stampString[i];
	}

	gcry_md_hd_t h2;
	gcry_md_open(&h2, GCRY_MD_SHA256, GCRY_MD_FLAG_SECURE);	// initialise the hash context 
	gcry_md_write(h2, longInput, 40);	// hash some text 
	salsaKey2 = gcry_md_read(h2, GCRY_MD_SHA256);	// get the result 

	unsigned char iniVector[8];
	uint64_t NONCE = gen_nonce();
	int64ToChar(iniVector, NONCE);

	gcry_error_t gcryError;

	gcryError = gcry_cipher_open(
		gcryCipherHd,				// gcry_cipher_hd_t *
		GCRY_CIPHER_SALSA20,		// int
		GCRY_CIPHER_MODE_STREAM,	// int
		0							// unsigned int
	);          

	if (gcryError) {
		printf("gcry_cipher_open failed:  %s/%s\n", gcry_strsource(gcryError), gcry_strerror(gcryError));
		return;
	}

	gcryError = gcry_cipher_setkey(*gcryCipherHd, salsaKey2, 32);
	if (gcryError) {
		printf("gcry_cipher_setkey failed:  %s/%s\n", gcry_strsource(gcryError), gcry_strerror(gcryError));
		return;
	}

	gcryError = gcry_cipher_setiv(*gcryCipherHd, iniVector, 8);
	if (gcryError) {
		printf("gcry_cipher_setiv failed:  %s/%s\n", gcry_strsource(gcryError), gcry_strerror(gcryError));
		return;
	}
}

void int64ToChar(unsigned char a[], int64_t n) {
	a[0] = (n >> 56) & 0xFF;
	a[1] = (n >> 48) & 0xFF;
	a[2] = (n >> 40) & 0xFF;
	a[3] = (n >> 32) & 0xFF;
	a[4] = (n >> 24) & 0xFF;
	a[5] = (n >> 16) & 0xFF;
	a[6] = (n >> 8) & 0xFF;
	a[7] = n & 0xFF;
}

uint64_t charToNum(char a[], int bytes) {
	uint64_t n = 0;

	for (int b = 0; b < bytes; b++)
		n |= (unsigned long)(a[bytes-b-1] & 0xFF) << 8*b;
	
	return n;
}