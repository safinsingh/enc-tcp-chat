#pragma once

#include "error.h"

#include <openssl/evp.h>
#include <stdbool.h>

#define AES_128_KEY_SIZE 16
#define AES_BLOCK_SIZE 16

extern unsigned char aes_key[16];

void rand16(unsigned char *buf);

void aes_init();
void aes_cleanup();

int aes_enc_dec(unsigned char *in, int in_len, unsigned char *out,
                unsigned char *iv, bool should_enc);

// note: key must have len AES_128_KEY_SIZE
int aes_enc(char *plaintext, unsigned char *enc, unsigned char *iv);
void aes_dec(unsigned char *enc, int enc_len, char *plaintext,
             unsigned char *iv);