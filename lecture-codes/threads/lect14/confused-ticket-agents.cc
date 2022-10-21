/**
 * File: confused-ticket-agents.cc
 * -----------------
 * An example of thread-level parallelism in selling tickets.
 */

#include <thread>         // prvides for C++11 threads
#include <iostream>       // provides cout, endl
#include <cstdlib>        // provides srandom, random (which we pretend are thread-safe, even though they aren't)
#include "ostreamlock.h"  // provides custom CS110 iomanipulators for locking down streams
#include "thread-utils.h" // for CS110 sleep_for function
#include <mutex>
using namespace std;

// Number of threads to spawn to sell tickets
const size_t kNumTicketAgents = 10;

static void sellTickets(size_t id, size_t& remainingTickets) {
    while (remainingTickets > 0) {
        sleep_for(500);  // simulate "selling a ticket"
        remainingTickets--;
        cout << oslock << "Thread #" << id << " sold a ticket (" << remainingTickets
             << " remain)." << endl << osunlock;
    }
    cout << oslock << "Thread #" << id << " sees no remaining tickets to sell and exits."
         << endl << osunlock;
}

int main(int argc, const char *argv[]) {
    thread ticketAgents[kNumTicketAgents];
    size_t remainingTickets = 250;
    
    for (size_t i = 0; i < kNumTicketAgents; i++) {
        ticketAgents[i] = thread(sellTickets, i, ref(remainingTickets));
    }
    for (thread& ticketAgent: ticketAgents) {
        ticketAgent.join();
    }
    
    cout << "Ticket selling done!" << endl;
    return 0;
}
