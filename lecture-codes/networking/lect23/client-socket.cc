/**
 * File: client-socket.cc
 * ----------------------
 * Presents the implementation of the createClientSocket, which is used for a
 * a good number of our networking examples.
 */

#include "client-socket.h"
#include <netdb.h>                // for gethostbyname
#include <sys/socket.h>           // for socket, AF_INET
#include <sys/types.h>            // for SOCK_STREAM
#include <unistd.h>               // for close
#include <cstring>                // for memset
using namespace std;

int createClientSocket(const string& host, unsigned short port) {
  struct hostent *he = gethostbyname(host.c_str());
  if (he == NULL) return kClientSocketError;
  
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) return kClientSocketError;
  
  struct sockaddr_in address;
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr = *((struct in_addr *)he->h_addr);
  
  if (connect(s, (struct sockaddr *) &address, sizeof(address)) == 0) return s;

  close(s);
  return kClientSocketError;
}
