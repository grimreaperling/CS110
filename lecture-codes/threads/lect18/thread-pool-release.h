/**
 * File: thread-pool-release.h
 * ---------------------------
 * Exports a ThreadPool implementation, which manages a finite collection
 * of worker threads that collaboratively work through a sequence of tasks.
 * As each task is scheduled, the ThreadPool waits for at least
 * one worker thread to be free and then assigns that task to that worker.  
 * Threads are scheduled and served in a FIFO manner, and tasks need to
 * take the form of thunks, which are zero-argument thread routines.
 */

#ifndef _release_thread_pool_
#define _release_thread_pool_
    
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <vector>
#include <atomic>
#include <functional>
#include "semaphore.h"

namespace secret {
  class ThreadPool;
}

namespace release {

class ThreadPool {
public:
    
/**
 * Constructs a ThreadPool configured to spawn up to the specified
 * number of threads.
 */
  ThreadPool(size_t numThreads);
        
/**
 * Schedules the provided thunk (which is something that can
 * be invoked as a zero-argument function without a return value)
 * to be executed by one of the ThreadPool's threads as soon as
 * all previously scheduled thunks have been handled.
 */
  void schedule(const std::function<void(void)>& thunk);
        
/**
 * Blocks and waits until all previously scheduled thunks
 * have been executed in full.
 */
  void wait();

/**
 * Waits for all thunks to have been executed, and then waits
 * for all threads to be be destroyed, and then otherwise brings
 * down all resources associated with the ThreadPool.
 */
  ~ThreadPool();

 private:
  class secret::ThreadPool *secret;

  ThreadPool(const ThreadPool& original) = delete;
  ThreadPool& operator=(const ThreadPool& rhs) = delete;
};

#endif

}
