#pragma once

#define MAX_MSG_LEN (128)
// 128 is 3 digits
#define MSG_LEN_OFFSET (3)

#define MSG_EXIT ("!exit")

// raw: <enc_len><iv><enc>

#define MSG_ENC_LEN(PLAINTEXT_LEN)                                             \
  (MSG_LEN_OFFSET + AES_BLOCK_SIZE + (PLAINTEXT_LEN) + AES_BLOCK_SIZE)

#define MAX_ENC_LEN MSG_ENC_LEN(MAX_MSG_LEN)

int decrypt_msg(char *raw, char *plaintext_buf);
int encrypt_msg(char *plaintext, unsigned char **msg_outptr);
void encrypt_write(int streamfd, char *plaintext);