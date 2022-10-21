/**
 * File: server-socket.cc
 * ----------------------
 * Presents the implementation of the createServerSocket function as described in
 * server-socket.h
 */

#include "server-socket.h"
#include <unistd.h>                // for close
#include <sys/socket.h>            // for socket, bind, accept, listen, etc.
#include <arpa/inet.h>             // for htonl, htons, etc.
#include <cstring>                 // for memset

static const int kReuseAddresses = 1;
int createServerSocket(unsigned short port, int backlog) {
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) return kServerSocketFailure;
  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &kReuseAddresses, sizeof(int)) < 0) {
    close(s);
    return kServerSocketFailure;
  }
  
  struct sockaddr_in address; // IPv4-style socket address
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(port);

  if (bind(s, (struct sockaddr *)&address, sizeof(address)) == 0 && listen(s, backlog) == 0) return s;
  
  close(s);
  return kServerSocketFailure;
}
