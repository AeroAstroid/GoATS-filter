#include <stdio.h>
#include <stdint.h>
#include "../include/gcrypt.h"

uint64_t random_seed(gcry_cipher_hd_t cipher, int bits, unsigned char* encBuffer);
uint64_t gen_nonce();
void setkey(gcry_cipher_hd_t* gcryCipherHd);
void int64ToChar(unsigned char a[], int64_t n);
uint64_t charToNum(char a[], int bytes);