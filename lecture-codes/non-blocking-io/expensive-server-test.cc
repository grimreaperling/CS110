/**
 * File: expensive-server-test.cc
 * ------------------------------
 * This is in place to levy many, many simulatanous requests
 * from our expensive server.
 */

#include <cstdlib>       // for system
#include "thread-pool-reference.h"
using namespace std;
using namespace release;

int main(int argc, char *argv[]) {
  ThreadPool requestPool(64);
  for (size_t i = 0; i < 10000; i++) {
    requestPool.schedule([] {
      system("wget --quiet --delete-after localhost:12345");
    });
  }

  requestPool.wait();
  return 0;
}
