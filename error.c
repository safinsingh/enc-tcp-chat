#include <openssl/err.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>

int err(int res, char *msg) {
  if (res < 0) {
    perror(msg);
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
  return res;
}

void *err_ptr(void *res, char *msg) {
  if (res == NULL) {
    perror(msg);
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
  return res;
}

void err_ssl(int res, EVP_CIPHER_CTX *ctx) {
  if (!res) {
    fprintf(stderr, "openssl error: %s\n",
            ERR_error_string(ERR_get_error(), NULL));
    EVP_CIPHER_CTX_cleanup(ctx);
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
}

void print_ubyte_array(unsigned char *arr, int len) {
  for (int i = 0; i < len; i++)
    printf("%02x ", *(arr + i));
  printf("\n");
}

void dbg_ubyte_array(char *dbg, unsigned char *arr, int len) {
  printf("%s[%d]: ", dbg, len);
  print_ubyte_array(arr, len);
}