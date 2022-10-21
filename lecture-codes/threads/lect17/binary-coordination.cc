/* This program is an example of one thread waiting on another to complete
 * some work before proceeding (binary coordination).  It uses a semaphore
 * initialized to 0 to do this.
 */

#include <mutex>
#include <thread>
#include <iostream>
#include "ostreamlock.h"
#include "semaphore.h"
#include "thread-utils.h"
using namespace std;

void create(int creationCount, semaphore &s) {
    for (int i = 0; i < creationCount; i++) {
        cout << oslock << "Now creating " << i << endl << osunlock;
        s.signal();
    }
}

void consume_after_create(int consumeCount, semaphore &s) {
    for (int i = 0; i < consumeCount; i++) {
        s.wait();
        cout << oslock << "Now consuming " << i << endl << osunlock;
    }
}

int main(int argc, const char *argv[]) {
    semaphore zeroSemaphore; // can omit (0), since default initializes to 0
    int numIterations = 5;
    thread thread_waited_on(create, numIterations, ref(zeroSemaphore));
    thread waiting_thread(consume_after_create, numIterations, ref(zeroSemaphore));
    thread_waited_on.join();
    waiting_thread.join();
    return 0;
}