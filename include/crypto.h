#ifndef CRYPTO_H
#define CRYPTO_H

#include <openssl/sha.h>

#define SHA256_HASH_SIZE SHA256_DIGEST_LENGTH
#define SHA256_STRING_SIZE (SHA256_DIGEST_LENGTH * 2 + 1)

int calculate_sha256(const char *filename, unsigned char hash[SHA256_HASH_SIZE]);
void sha256_to_string(const unsigned char hash[SHA256_HASH_SIZE], char output[SHA256_STRING_SIZE]);
int compare_sha256(const unsigned char hash1[SHA256_HASH_SIZE], const unsigned char hash2[SHA256_HASH_SIZE]);

#endif
