/**
 * Implements the classic reader-writer thread example, where
 * one thread writes to a shared buffer and a second thread reads
 * from it.  This version uses thread coordination to ensure that the
 * reader only reads valid data and the writer doesn't overwrite unread
 * data.
 */

#include <mutex>
#include <thread>
#include <iostream>
#include <string.h>
#include "ostreamlock.h"
#include "random-generator.h"
#include "semaphore.h"
#include "thread-utils.h"
using namespace std;

// The size of the buffer, in bytes
static const unsigned int kNumBufferSlots = 8;

// The number of times the reader and write should loop through the buffer.
static const unsigned int kNumIterations = 3;

// The bounds for how much the reader could sleep for when processing data
static const unsigned int kLowProcessTimeMS = 20;
static const unsigned int kHighProcessTimeMS = 120;

// The bounds for how much the writer could sleep for when preparing data
static const unsigned int kLowPrepareTimeMS = 10;
static const unsigned int kHighPrepareTimeMS = 100;

static mutex rgenLock;
static RandomGenerator rgen;

/* Locks around the shared random generator and returns a random value
 * between low and high to sleep for (in milliseconds).
 */
static unsigned int getSleepDuration(unsigned int low, unsigned int high) {
  lock_guard<mutex> lockGuard(rgenLock);
  return rgen.getNextInt(low, high);
}

/* Simulates "preparing data" by sleeping for a random amount of time,
 * and then return a random character representing the data produced.
 */
static char prepareData() {
  sleep_for(getSleepDuration(kLowPrepareTimeMS, kHighPrepareTimeMS));
  lock_guard<mutex> lockGuard(rgenLock);
  return rgen.getNextInt('A', 'Z');
}

// Simulates "processing data" by sleeping for a random amount of time.
static void processData(char ch) {
  sleep_for(getSleepDuration(kLowProcessTimeMS, kHighProcessTimeMS));
}

/* Prints the contents of the given buffer in the format |A|B|C|....
 * Performs no concurrency/coordination.
 */
static void printBuffer(char buffer[], size_t bufferSize) {
  cout << oslock << "BUFFER: |";
  for (size_t i = 0; i < bufferSize; i++) {
    cout << buffer[i] << "|";
  }

  cout << endl << osunlock;
}


/* Function: writeToBuffer
 * ------------------------
 * Parameters:
 * - buffer: the character buffer we should write contents to, in sequential
 *           order, from front to back.  
 * - bufferSize: the size of the buffer (# slots), in bytes
 * - iterations: how many times we should loop through the buffer, writing
 *              random contents to each slot.
 * - fullBufferSlots: a semaphore coordinating when there is data to be read
 * - emptyBufferSlots: a semaphore coordinating when there is space for new data
 *          to be written.
 *
 * Returns: N/A
 */
static void writeToBuffer(char buffer[], size_t bufferSize, size_t iterations,
  semaphore& fullBufferSlots, semaphore& emptyBufferSlots) {

  cout << oslock << "Writer: ready to write." << endl << osunlock;
  for (size_t i = 0; i < iterations * bufferSize; i++) {
    char ch = prepareData();

    // Wait for space before writing
    emptyBufferSlots.wait();

    buffer[i % bufferSize] = ch;

    // Signal that there is content to read
    fullBufferSlots.signal();

    cout << oslock << "Writer: published data packet with character '" 
      << ch << "'.\t\t" << osunlock;
    printBuffer(buffer, bufferSize);
  }
}

/* Function: readFromBuffer
 * ------------------------
 * Parameters:
 * - buffer: the character buffer we should read contents from, in sequential
 *           order, from front to back.  
 * - bufferSize: the size of the buffer (# slots), in bytes
 * - iterations: how many times we should loop through the buffer, reading it
 * - fullBufferSlots: a semaphore coordinating when there is data to be read
 * - emptyBufferSlots: a semaphore coordinating when there is space for new data
 *          to be written.
 *
 * Returns: N/A
 */
static void readFromBuffer(char buffer[], size_t bufferSize, size_t iterations,
  semaphore& fullBufferSlots, semaphore& emptyBufferSlots) {

  cout << oslock << "Reader: ready to read." << endl << osunlock;
  for (size_t i = 0; i < iterations * bufferSize; i++) {

    // Wait for content before reading
    fullBufferSlots.wait();

    // Read and process the data
    char ch = buffer[i % bufferSize];
    processData(ch); // sleep to simulate work
    buffer[i % bufferSize] = ' ';

    // Signal that there is space to write more content
    emptyBufferSlots.signal();

    cout << oslock << "Reader: consumed data packet " 
      << "with character '" << ch << "'.\t\t" << osunlock;
    printBuffer(buffer, bufferSize);
  }
}

int main(int argc, const char *argv[]) {
  // Create an empty buffer
  char buffer[kNumBufferSlots];
  memset(buffer, ' ', sizeof(buffer));

  // Semaphores representing how many "permits" are available for reading/writing
  semaphore fullBufferSlots;
  semaphore emptyBufferSlots(sizeof(buffer));

  thread writer(writeToBuffer, buffer, sizeof(buffer), kNumIterations, 
    ref(fullBufferSlots), ref(emptyBufferSlots));
  thread reader(readFromBuffer, buffer, sizeof(buffer), kNumIterations, 
    ref(fullBufferSlots), ref(emptyBufferSlots));
  writer.join();
  reader.join();
  return 0;
}
