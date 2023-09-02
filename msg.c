#include "msg.h"
#include "enc.h"
#include "error.h"
#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

int decrypt_msg(char *raw, char *plaintext_buf) {
  // get enc len
  char enc_len_tmp[MSG_LEN_OFFSET + 1] = {0};
  memcpy(enc_len_tmp, raw, MSG_LEN_OFFSET);
  int enc_len = atoi(enc_len_tmp);
  if (enc_len == 0)
    return -1;

  unsigned char iv[16];
  memcpy(iv, raw + MSG_LEN_OFFSET, AES_BLOCK_SIZE);

  // <enc_len><iv><enc>
  aes_dec((unsigned char *)(raw + MSG_LEN_OFFSET + AES_BLOCK_SIZE), enc_len,
          plaintext_buf, iv);

  return 0;
}

int encrypt_msg(char *plaintext, unsigned char **msg_outptr) {
  unsigned char iv[16];
  rand16(iv);

  // todo streaming aes
  // incl. null byte
  int msg_len = MSG_ENC_LEN(strlen(plaintext) + 1);
  *msg_outptr = err_ptr(malloc(msg_len), "malloc");

  unsigned char *enc = *msg_outptr;
  memcpy(enc + MSG_LEN_OFFSET, iv, sizeof(iv));

  int enc_offset = MSG_LEN_OFFSET + AES_BLOCK_SIZE;
  int enc_len = aes_enc(plaintext, enc + enc_offset, iv);

  enc[0] = '0' + (enc_len / 100);
  enc[1] = '0' + (enc_len % 100) / 10;
  enc[2] = '0' + (enc_len % 10);

  return msg_len;
}

void encrypt_write(int streamfd, char *plaintext) {
  unsigned char *msg;
  int msg_len = encrypt_msg(plaintext, &msg);
  err(write(streamfd, msg, msg_len), "write");
  free(msg);
}