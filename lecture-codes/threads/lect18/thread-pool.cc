/**
 * File: thread-pool.cc
 * --------------------
 * Presents the implementation of the ThreadPool class.
 */

#include "thread-pool.h"

using namespace std;
using develop::ThreadPool;

ThreadPool::ThreadPool(size_t numThreads) {}
void ThreadPool::schedule(const std::function<void(void)>& thunk) {}
void ThreadPool::wait() {}
ThreadPool::~ThreadPool() {}

