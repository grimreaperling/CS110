/* This program is an example of one thread waiting on multiple threads to complete
 * some work before proceeding (general coordination).  It uses a semaphore
 * initialized to a negative number to do this.
 */

#include <mutex>
#include <thread>
#include <iostream>
#include "ostreamlock.h"
#include "semaphore.h"
#include "thread-utils.h"
using namespace std;

void writer(int i, semaphore &s) {
    cout << oslock << "Sending signal " << i << endl << osunlock;
    s.signal();
}

void read_after_ten(semaphore &s) {
    s.wait();
    cout << oslock << "Got enough signals to continue!" << endl << osunlock;
}

int main(int argc, const char *argv[]) {
    semaphore negSemaphore(-9);
    thread writers[10];
    for (size_t i = 0; i < 10; i++) {
        writers[i] = thread(writer, i, ref(negSemaphore));
    }
    thread r(read_after_ten, ref(negSemaphore));
    for (thread &t : writers) t.join();
    r.join();
    return 0;
}