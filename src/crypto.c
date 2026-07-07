#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>

#include "config.h"
#include "crypto.h"

int calculate_sha256(const char *filename, unsigned char hash[SHA256_HASH_SIZE]) {
    FILE *file = fopen(filename, "rb");
    unsigned char buffer[BUFFER_SIZE];
    size_t bytes_read;
    unsigned int hash_length = 0;

    EVP_MD_CTX *context = EVP_MD_CTX_new();

    if (file == NULL || context == NULL) {
        if (file != NULL) {
            fclose(file);
        }
        if (context != NULL) {
            EVP_MD_CTX_free(context);
        }
        return -1;
    }

    if (EVP_DigestInit_ex(context, EVP_sha256(), NULL) != 1) {
        fclose(file);
        EVP_MD_CTX_free(context);
        return -1;
    }

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (EVP_DigestUpdate(context, buffer, bytes_read) != 1) {
            fclose(file);
            EVP_MD_CTX_free(context);
            return -1;
        }
    }

    if (EVP_DigestFinal_ex(context, hash, &hash_length) != 1) {
        fclose(file);
        EVP_MD_CTX_free(context);
        return -1;
    }

    fclose(file);
    EVP_MD_CTX_free(context);

    return 0;
}

void sha256_to_string(const unsigned char hash[SHA256_HASH_SIZE], char output[SHA256_STRING_SIZE]) {
    for (int i = 0; i < SHA256_HASH_SIZE; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }

    output[SHA256_STRING_SIZE - 1] = '\0';
}

int compare_sha256(const unsigned char hash1[SHA256_HASH_SIZE], const unsigned char hash2[SHA256_HASH_SIZE]) {
    return memcmp(hash1, hash2, SHA256_HASH_SIZE);
}
