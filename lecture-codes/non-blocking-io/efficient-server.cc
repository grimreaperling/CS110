/**
 * File: efficient-server.cc
 * -------------------------
 * Simple application that relies on nonblocking
 * I/O and the suite of epoll functions to
 * implement an event-driven web-server.
 */

#include <iostream>                    // for cerr
#include <map>                         // for map
#include <string>                      // for string
#include <unistd.h>                    // for close
#include <sys/epoll.h>                 // for epoll functions
#include <sys/types.h>                 // for accept
#include <sys/socket.h>                // for accept
#include <string.h>                    // for strerror
#include <cassert>
#include "server-socket.h"             // for createServerSocket
#include "non-blocking-utils.h"        // for setAsNonBlocking
using namespace std;

/**
 * Function: acceptNewConnections
 * ------------------------------
 * Called when the kernel detects a read-oriented event
 * on the server socket (which is always in the watch set).
 *
 * In theory, many incoming requests (e.g. one or more) may have
 * accumulated in the time it took for the kernel to detect even the first one,
 * and because the server socket was registered for edge-triggered
 * event notification (e.g. via the EPOLLET bit being set), we are
 * required to accept each and every one of those incoming connections
 * before returning.  Each of those client connections needs to made nonblocking
 * and then added to the watch set (in edge-triggered mode, initially for
 * read events, since we need to ingest the request header before responding).
 */
static void acceptNewConnections(int ws, int server) {
  while (true) {
    int clientSocket = accept4(server, NULL, NULL, SOCK_NONBLOCK);
    if (clientSocket == -1) return;
    struct epoll_event info = {.events = EPOLLIN | EPOLLET, .data = {.fd = clientSocket}};
    epoll_ctl(ws, EPOLL_CTL_ADD, clientSocket, &info);
  }
}

/**
 * Function: consumeAvailableData
 * ------------------------------
 * Reads in as much available data from the supplied client socket
 * until it either would have blocked, or until we have enough of the
 * response header (e.g. we've read up through a "\r\n\r\n") to respond.
 * Because the client sockets were registered for edge-triggered read events,
 * we need to consume *all* available data before returning, else epoll_wait
 * will never surface this client socket again.
 */
static const size_t kBufferSize = 1;
static const string kRequestHeaderEnding("\r\n\r\n");
static void consumeAvailableData(int ws, int client) {
  static map<int, string> requests; // tracks what's been read in thus far over each client socket
  size_t pos = string::npos;
  while (pos == string::npos) {
    char buffer[kBufferSize];
    ssize_t count = read(client, buffer, kBufferSize);
    if (count == -1 && errno == EWOULDBLOCK) return; // not done reading everything yet, so return
    if (count <= 0) { close(client); break; } // passes? then bail on connection, as it's borked
    requests[client] += string(buffer, buffer + count);
    pos = requests[client].find(kRequestHeaderEnding);
    if (pos == string::npos) continue;
    cout << "Num Active Connections: " << requests.size() << endl;
    cout << requests[client].substr(0, pos + kRequestHeaderEnding.size()) << flush;
    struct epoll_event info = {.events = EPOLLOUT | EPOLLET, .data = {.fd = client}};
    epoll_ctl(ws, EPOLL_CTL_MOD, client, &info); // MOD == modify existing event
  }

  requests.erase(client);
}

/**
 * Function: publishResponse
 * -------------------------
 * Called on behalf of the specified client socket whenever the
 * kernel detects that we're able to write to it (and we're interested
 * in writing to it).
 */
static const string kResponseString("HTTP/1.1 200 OK\r\n\r\n<b>"
              "Thank you for your request! We're working on it! No, really!</b><br/>"
              "<br/><img src=\"http://vignette3.wikia.nocookie.net/p__/images/e/e0/"
              "Agnes_Unicorn.png/revision/latest?cb=20160221214120&path-prefix=protagonist\"/>");
static void publishResponse(int client) {
  static map<int, size_t> responses;
  responses[client]; // insert a 0 if key isn't present
  while (responses[client] < kResponseString.size()) {
    ssize_t count = write(client, kResponseString.c_str() + responses[client],
                          kResponseString.size() - responses[client]);
    if (count == -1 && errno == EWOULDBLOCK) return;
    if (count == -1) break;
    assert(count > 0);
    responses[client] += count;
  }

  responses.erase(client);
  close(client);
}

/**
 * Function: buildInitialWatchSet
 * ------------------------------
 * Creates an epoll watch set around the supplied server socket.  We
 * register an event to show our interest in being notified when the server socket is
 * available for read (and accept) operations via EPOLLIN, and we also
 * note that the event notificiations should be edge triggered (EPOLLET)
 * which means that we'd only like to be notified that data is available
 * to be read when the kernel is certain there is data.
 */
static int buildInitialWatchSet(int server) {
  int ws = epoll_create1(0);
  struct epoll_event info = {.events = EPOLLIN | EPOLLET, .data = {.fd = server}};
  epoll_ctl(ws, EPOLL_CTL_ADD, server, &info);
  return ws;
}

/**
 * Function: runServer
 * -------------------
 * Converts the supplied server socket to be nonblocking, constructs
 * the initial watch set around the server socket, and then enter the
 * wait/response loop, blocking with each iteration until the kernel
 * detects something interesting happened to one or more of the
 * descriptors residing within the watch set.  The call to epoll_wait
 * is the only blocking system call in the entire (single-thread-of-execution)
 * web server.
 */
static const int kMaxEvents = 64;
static void runServer(int server) {
  setAsNonBlocking(server);
  int ws = buildInitialWatchSet(server);
  struct epoll_event events[kMaxEvents];
  while (true) {
    int numEvents = epoll_wait(ws, events, kMaxEvents, /* timeout = */ -1);
    for (int i = 0; i < numEvents; i++) {
      if (events[i].data.fd == server) {
        acceptNewConnections(ws, server);
      } else if (events[i].events & EPOLLIN) { // we're still reading the client's request
        consumeAvailableData(ws, events[i].data.fd);
      } else { // events[i].events & EPOLLOUT
        publishResponse(events[i].data.fd);
      }
    }
  }
}

/**
 * Function: main
 * --------------
 * Provides the entry point for the entire server.  The implementation
 * of main just passes the buck to runServer.
 */
static const unsigned short kDefaultPort = 33333;
int main(int argc, char **argv) {
  int server = createServerSocket(kDefaultPort);
  if (server == kServerSocketFailure) {
    cerr << "Failed to start server.  Port " << kDefaultPort << " is probably already in use." << endl;
    return 1;
  }

  cout << "Server listening on port " << kDefaultPort << endl;
  runServer(server);
  return 0;
}
