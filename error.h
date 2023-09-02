#pragma once

#include <openssl/evp.h>

int err(int res, char *msg);
void *err_ptr(void *res, char *msg);
void err_ssl(int res, EVP_CIPHER_CTX *ctx);
void print_ubyte_array(char *arr, int len);
void dbg_ubyte_array(char *dbg, unsigned char *arr, int len);

#define USAGE                                                                  \
  {                                                                            \
    fprintf(stderr, "Usage: %s [-c <host:port>/-s <port>] -p <password>\n",    \
            argv[0]);                                                          \
    exit(EXIT_FAILURE);                                                        \
  }