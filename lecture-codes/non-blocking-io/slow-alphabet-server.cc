/**
 * File: slow-alphabet-server.cc
 * -----------------------------
 * Presents the implementation of a server that, for
 * every client connection, just publishes the 26 letters of
 * the alphabet, one letter per tenth of a second, 
 * over 2.6 seconds.
 */

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cassert>
#include "server-socket.h"
#include "thread-pool-reference.h"
#include "socket++/sockstream.h"
using namespace std;
using release::ThreadPool;

static const string kAlphabet = "abcdefghijklmnopqrstuvwxyz";
static const useconds_t kDelay = 100000; //  100000 microseconds is 100 ms is 0.1 seconds
static void handleRequest(int client) {
  sockbuf sb(client);
  iosockstream ss(&sb);
  for (size_t i = 0; i < kAlphabet.size(); i++) {
    ss << kAlphabet[i] << flush;
    usleep(kDelay);
  }
}
  
static const unsigned short kSlowAlphabetServerPort = 41411;
int main(int argc, char *argv[]) {
  int server = createServerSocket(kSlowAlphabetServerPort);
  assert(server != kServerSocketFailure);
  ThreadPool pool(128);
  while (true) {
    int client = accept(server, NULL, NULL);
    pool.schedule([client]() { handleRequest(client); });
  }
  return 0;
}
