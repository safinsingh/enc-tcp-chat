#include "shared.h"
#include "error.h"
#include "msg.h"

#include <openssl/aes.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

bool is_exit(char *plaintext) {
  return memcmp(plaintext, MSG_EXIT, strlen(MSG_EXIT)) == 0;
}

void spawn_reader(int *streamfd) {
  char msg_buf[MAX_ENC_LEN];
  char plaintext[MAX_MSG_LEN];

  for (;;) {
    bzero(msg_buf, MAX_ENC_LEN);
    err(read(*streamfd, msg_buf, MAX_MSG_LEN), "failed to read from socket");

    bzero(plaintext, MAX_MSG_LEN);
    if (decrypt_msg(msg_buf, plaintext) < 0) {
      printf("got invalid message\n");
      continue;
    }

    if (is_exit(plaintext)) {
      printf("connection terminated\n");
      break;
    }

    // just in case
    plaintext[sizeof(plaintext) - 1] = '\0';
    printf("> %s", plaintext);
  }
}

void spawn_writer(int *streamfd) {
  char buf[MAX_MSG_LEN];

  for (;;) {
    fgets(buf, sizeof(buf), stdin);
    encrypt_write(*streamfd, buf);

    if (is_exit(buf)) {
      exit(0);
    }
  }
}

void spawn_rw_join(int *streamfd) {
  pthread_t reader;
  err(pthread_create(&reader, NULL, (void *)spawn_reader, (void *)streamfd),
      "failed to create reader thread");

  pthread_t writer;
  err(pthread_create(&writer, NULL, (void *)spawn_writer, (void *)streamfd),
      "failed to create writer thread");

  err(pthread_join(reader, NULL), "reader thread");
  err(pthread_cancel(writer), "writer thread");
}