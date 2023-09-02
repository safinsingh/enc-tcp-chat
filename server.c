#include "enc.h"
#include "error.h"
#include "msg.h"
#include "shared.h"

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

volatile int streamfd__server = -1;

void register_exit__server() {
  if (streamfd__server > 0) {
    encrypt_write(streamfd__server, MSG_EXIT);
    close(streamfd__server);
  }

  printf("exiting...\n");
  exit(0);
}

int setup_sockfd__server(int server_port) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  err(sockfd, "failed to create tcp socket");

  err(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)),
      "setsockopt");

  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(server_port);

  err(bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)),
      "failed to bind socket");

  err(listen(sockfd, 16), "failed to listen for connections");
  printf("listening on 0.0.0.0:%d\n", server_port);

  return sockfd;
}

void main__server(int server_port) {
  signal(SIGINT, register_exit__server);

  int sockfd = setup_sockfd__server(server_port);

  for (;;) {
    streamfd__server = accept(sockfd, NULL, 0);
    err(streamfd__server, "failed to accept connection");
    printf("got connection, \"!exit\" to exit\n");

    spawn_rw_join((int *)&streamfd__server);
  }
}