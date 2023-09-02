#include "enc.h"
#include "error.h"
#include "msg.h"
#include "server.h"
#include "shared.h"

#include <arpa/inet.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

volatile int streamfd__client = -1;

void register_exit__client() {
  if (streamfd__client > 0) {
    encrypt_write(streamfd__client, MSG_EXIT);
    close(streamfd__client);
  }

  printf("exiting...\n");
  exit(0);
}

void setup_streamfd__client(char *remote) {
  char *host;
  char *port;

  host = strtok(remote, ":");
  port = strtok(NULL, ":");

  if (!host || !port)
    err(-1, "failed to parse host:port");

  struct sockaddr_in remote_addr;
  int remote_port;

  if (inet_pton(AF_INET, host, &(remote_addr.sin_addr)) != 1)
    err(-1, "failed to parse host address");
  if ((remote_port = atoi(port)) == 0)
    err(-1, "failed to parse host port");

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  err(sockfd, "failed to create tcp socket");

  remote_addr.sin_family = AF_INET;
  remote_addr.sin_port = htons(remote_port);

  err(connect(sockfd, (struct sockaddr *)&remote_addr, sizeof(remote_addr)),
      "failed to connect");

  streamfd__client = sockfd;
}

void main__client(char *remote) {
  signal(SIGINT, register_exit__client);

  setup_streamfd__client(remote);
  printf("connected to %s, \"!exit\" to exit\n", remote);

  spawn_rw_join((int *)&streamfd__client);
}