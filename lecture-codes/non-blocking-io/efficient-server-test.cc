/**
 * File: expensive-server-test.cc
 * ------------------------------
 * This is in place to levy many, many simulatanous requests
 * from our expensive server.
 */

#include <cstdlib>
#include <cassert>
#include <string>
#include <iostream>
#include <unistd.h>
#include "socket++/sockstream.h"
#include "client-socket.h"
#include "thread-pool-reference.h"
using namespace std;
using namespace release;

const string kSmallHTTPRequest = "GET /default.aspx HTTP/1.1\r\nHost: myth24.stanford.edu\r\n\r\n";
static void downloadPage() {
  int client = createClientSocket("localhost", 33333);
  assert(client >= 0);
  sockbuf sb(client);
  iosockstream ss(&sb);
  for (size_t i = 0; i < kSmallHTTPRequest.size(); i++) {
    ss << kSmallHTTPRequest[i] << flush;
    if (i % 5 == 0) usleep(10000);
  }

  string line;
  while (!ss.fail()) getline(ss, line); // just ingest the payload and move on
}

int main(int argc, char **argv) {
  ThreadPool requestPool(64);
  for (size_t i = 0; i < 10000; i++) 
    requestPool.schedule([] { downloadPage(); });
  requestPool.wait();
  return 0;
}
