#include "enc.h"
#include "error.h"

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

EVP_CIPHER_CTX *ctx = NULL;
unsigned char aes_key[16];

void rand16(unsigned char *buf) {
  if (RAND_bytes(buf, 16) != 1) {
    err(-1, "salt generation");
  }
}

void aes_init(char *password) {
  ctx = err_ptr(EVP_CIPHER_CTX_new(), "openssl ctxt");

  // we do not need a salt - Drakon
  unsigned char salt[16] = {
      0x0, 0x2, 0x4, 0x6, 0x8, 0xA, 0xC, 0xE,
      0x1, 0x3, 0x5, 0x7, 0x9, 0xB, 0xD, 0xF,
  };

  int itns = 10000;
  err(PKCS5_PBKDF2_HMAC(password, strlen(password), salt, sizeof(salt), itns,
                        EVP_sha256(), AES_128_KEY_SIZE, aes_key),
      "aes key generation");
}

void aes_cleanup() {
  if (ctx)
    EVP_CIPHER_CTX_free(ctx);
}

int aes_enc_dec(unsigned char *in, int in_len, unsigned char *out,
                unsigned char *iv, bool should_enc) {
  err_ssl(
      EVP_CipherInit_ex(ctx, EVP_aes_128_cbc(), NULL, aes_key, iv, should_enc),
      ctx);

  int len, out_len;

  err_ssl(EVP_CipherUpdate(ctx, out, &len, in, in_len), ctx);
  out_len = len;

  err_ssl(EVP_CipherFinal_ex(ctx, out + len, &len), ctx);
  out_len += len;

  EVP_CIPHER_CTX_cleanup(ctx);
  return out_len;
}

int aes_enc(char *plaintext, unsigned char *enc, unsigned char *iv) {
  int enc_len =
      aes_enc_dec((unsigned char *)plaintext, strlen(plaintext), enc, iv, true);

  return enc_len;
}

void aes_dec(unsigned char *enc, int enc_len, char *plaintext,
             unsigned char *iv) {
  aes_enc_dec(enc, enc_len, (unsigned char *)plaintext, iv, false);
}
