#include "client.h"
#include "enc.h"
#include "error.h"
#include "server.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool parse_args(int argc, char **argv, int *server_port, char *remote,
                char *password);
int main(int argc, char **argv) {
  if (argc < 5)
    USAGE

  int server_port;
  char remote[64] = {0}, password[64] = {0};
  bool client = parse_args(argc, argv, &server_port, remote, password);

  aes_init(password);
  if (client)
    main__client(remote);
  else
    main__server(server_port);
}

bool parse_args(int argc, char **argv, int *server_port, char *remote,
                char *password) {
  bool ret;
  int opt;
  while ((opt = getopt(argc, argv, "c:s:p:")) != -1) {
    switch (opt) {
    case 'c':
      ret = true;
      if (!optarg)
        USAGE
      strcpy(remote, optarg);
      break;
    case 's':
      ret = false;
      if (!optarg)
        USAGE
      *server_port = atoi(optarg);
      break;
    case 'p':
      if (!optarg)
        USAGE
      strcpy(password, optarg);
      break;
    default:
      USAGE
    }
  }
  return ret;
}