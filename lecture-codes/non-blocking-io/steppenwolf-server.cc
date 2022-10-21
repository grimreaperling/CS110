/**
 * File: steppenwolf-server.cc
 * ---------------------------
 * Simple application that relies on epoll functions to
 * implement an event-driven server that just tells all
 * clients to go away the instant they connect.
 */

#include <iostream>                    // for cerr
#include <string>                      // for string
#include <unistd.h>                    // for close
#include <sys/epoll.h>                 // for epoll functions
#include <sys/types.h>                 // for accept
#include <sys/socket.h>                // for accept
#include <netinet/in.h>                // for struct sockaddr_in
#include <arpa/inet.h>                 // for inet_ntop
#include <socket++/sockstream.h>       // for sockbuf, iosockstream
#include <cassert>                     // for assert
#include "server-socket.h"             // for createServerSocket
#include "non-blocking-utils.h"        // for setAsNonBlocking
using namespace std;

/**
 * Function: acceptAllNewConnections
 * ---------------------------------
 * Called when the kernel detects a read-oriented event
 * on the server socket.
 */
const string kResponsePayload = "<b>Go away and never come back!</b>";
const string kResponseHeader = "HTTP/1.0 200 OK\r\nContent-Length: " + to_string(kResponsePayload.size()) + "\r\n\r\n";
const string kResponse = kResponseHeader + kResponsePayload;
static void acceptAllNewConnections(int server) {
  while (true) {
    int client = accept(server, NULL, NULL);
    if (client == -1) return;
    sockbuf sb(client);
    iosockstream ss(&sb);
    ss << kResponse << flush;
  }
}

/**
 * Function: createWatchSet
 * ------------------------
 * Creates an epoll watch set around the supplied server socket.  We
 * register an interest in being notified when the server socket is
 * available for accept operations via EPOLLIN | EPOLLET.
 */
static int createWatchSet(int server) {
  int ws = epoll_create1(0);
  struct epoll_event info = {.events = EPOLLIN | EPOLLET, .data = {.fd = server}};
  epoll_ctl(ws, EPOLL_CTL_ADD, server, &info);
  return ws;
}

/**
 * Function: runServer
 * -------------------
 * Converts the supplied server socket to be nonblocking, constructs
 * a new watch set around the server socket, and then enters a
 * wait/response loop, blocking with each iteration until the kernel
 * detects one or more pending connections.
 */
static void runServer(int server) {
  setAsNonBlocking(server);
  int ws = createWatchSet(server);
  while (true) {
    struct epoll_event event;
    epoll_wait(ws, &event, 1, /* timeout = */ -1);
    assert(event.data.fd == server);
    acceptAllNewConnections(server);
  }
}

/**
 * Function: main
 * --------------
 * Provides the entry point for our curt server.  The implementation
 * of main just passes the buck to runServer.
 */
static const unsigned short kDefaultPort = 22222;
int main(int argc, char *argv[]) {
  int server = createServerSocket(kDefaultPort);
  if (server < 0) return 1;
  cout << "Server listening on port " << kDefaultPort << endl;
  runServer(server);
  return 0;
}
