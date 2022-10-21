/** 
 * File: ice-cream-store.cc
 * -------------------------
 * A larger example used to show a combination of synchronization techniques 
 * (binary locks, generalized counters, coordination semaphores) in a more
 * complicated arrangement.
 *
 * This is the "ice cream store" simulation.  There are customers who want
 * to buy ice cream cones, clerks who make the cones, the manager who
 * checks a clerk's work, and the cashier who takes a customer's
 * money.  There are a many different interactions that need to be modeled:
 * the customers dispatching several clerks (one for each cone they 
 * are buying), the clerks who need the manager to approve their work,
 * the cashier who rings up each customer in a first-in-first-out order,
 * and so on.
 */

#include <thread>
#include <mutex>
#include <vector>
#include <iostream>
#include <atomic>
#include "semaphore.h"
#include "ostreamlock.h"
#include "ice-cream-store-utils.h"
using namespace std;


// The number of customers spawned to purchase ice cream
static const size_t kNumCustomers = 15;

/**
 * Here, we declare a new struct type to coordinate interactions between
 * the clerks and the manager.  A clerk must acquire the `available` lock before
 * accessing any fields inside this struct.  This ensures that one clerk is
 * requesting approval at a time.  When a clerk requests approval, it signals
 * to the manager via `requested` that it wants a cone inspected, and then
 * waits on `finished`.  The manager inspects the cone, puts its approval
 * decision in `passed`, and signals the clerk that it is done via `finished`.
 */
struct inspection_t {
  mutex available;
  semaphore requested;
  semaphore finished;
  bool passed;
};

/**
 * Here, we declare a type to coordinate interactions between the customers
 * and the cashier.  It's initialized with its `nextPlaceInLine` counter at 0.
 * `nextPlaceInLine` is an atomic counter type that guarantees ++ and -- are atomic.
 * We use this to give each customer a unique place in line.  `waitingCustomers`
 * is used to signal the cashier that someone is waiting in line to check out.
 * `customers` lets the cashier signal each individual customer when it's done
 * checking them out so they can leave.
 */
struct checkout_t {
  atomic<size_t> nextPlaceInLine{0};
  semaphore customers[kNumCustomers];
  semaphore waitingCustomers;
};

/* Function: clerk
 * ---------------
 * Parameters:
 *    - coneID: the unique cone ID within this customer's orders.
 *    - customerID: the unique ID for the customer ordering this cone.
 *    - inspection: the struct containing fields to communicate with the manager
 *                  to get an ice cream cone approved.
 *
 * Returns: N/A
 *
 * A clerk is responsible for making a single cone for a single customer.
 * A clerk must get its cone inspected by the manager; the clerk will make a
 * cone and then request approval for it from the manager.  If the manager
 * rejects the cone, the clerk repeats the process.  Otherwise, it returns.
 */
static void clerk(size_t coneID, size_t customerID, inspection_t& inspection) {
  // Try making a cone until it's approved
  bool success = false;
  while (!success) {
    makeCone(coneID, customerID);  // utility function, sleeps (see ice-cream-store-utils.h)

    // We must be the only one requesting approval
    inspection.available.lock();

    // Let the manager know we are requesting approval
    inspection.requested.signal();

    // Wait for the manager to finish
    inspection.finished.wait();

    // If the manager is finished, it has put its approval decision into "passed"
    success = inspection.passed;

    // We're done requesting approval, so unlock for someone else
    inspection.available.unlock();
  }
}

/**
 * Function: cashier
 * -----------------
 * Parameters:
 *    - checkout: the struct containing fields to communicate with each
 *                customer checking out.
 *
 * Returns: N/A
 * 
 * The cashier (assumes only 1) is responsible for letting customers check out
 * with their completed orders.  For each customer, it waits for a customer
 * to be ready to check out, then signals just them to let them know their
 * order has been paid for.
 */
static void cashier(checkout_t& checkout) {
  cout << oslock << "      Cashier is ready to help customers check out." 
       << endl << osunlock;

  // We check out all customers
  for (size_t i = 0; i < kNumCustomers; i++) {
    // Wait for someone to let us know they are ready to check out
    checkout.waitingCustomers.wait();
    cout << oslock << "      Cashier rings up customer " << i << "." 
         << endl << osunlock;

    // Let the ith customer know that they can leave.
    checkout.customers[i].signal();
  }

  cout << oslock << "      Cashier is all done and can go home."
       << endl << osunlock;
}

/* Function: manager
 * -----------------
 * Parameters:
 *    - numConesNeeded: the total number of cones that must be approved before
 *        the manager is done.
 *    - inspection: the struct containing fields to communicate with each clerk
 *                  to approve their ice cream cone.
 *
 * Returns: N/A
 *
 * The manager (assumes only 1) is responsible for approving cones made by
 * clerks.  It is spawned with a specific number of cones to approve, after
 * which it returns.  It waits for someone to signal it to approve a cone,
 * and then it inspects the cone, decides whether to approve it, and puts
 * its decision into the inspection struct before signaling that it is done.
 */
static void manager(size_t numConesNeeded, inspection_t& inspection) {
  // Keep track of cones submitted/approved (local vars, so no locks needed)
  size_t numConesAttempted = 0;
  size_t numConesApproved = 0;

  // Continue inspecting cones until we have met our quota
  while (numConesApproved < numConesNeeded) {
    // Wait for someone to request an inspection
    inspection.requested.wait();

    /* Inspect the cone - this takes some random amount of time, after
     * which the passed field in the inspection struct will store a boolean
     * for whether or not we approved the cone. (see ice-cream-store-utils.h)
     */
    inspection.passed = inspectCone();

    // Let them know we have finished inspecting
    inspection.finished.signal();

    // Update our counters
    numConesAttempted++;
    if (inspection.passed) numConesApproved++;
  }
  
  cout << oslock << "  Manager inspected a total of " << numConesAttempted 
       << " ice cream cones before approving a total of " << numConesNeeded 
       << "." << endl << "  Manager leaves the ice cream store."
       << endl << osunlock;
}

/* Function: customer
 * ------------------
 * Parameters:
 *    - id: the unique id number for this customer
 *    - numConesWanted: the number of cones this customer should order
 *    - inspection: the struct containing fields for a clerk to communicate
 *                   with the manager to get an ice cream cone approved.
 *    - checkout: the struct containing fields for each customer to communicate
 *                 to the cashier about checking out.
 *
 * Returns: N/A
 *
 * A customer spawns a clerk thread for each cone it orders.  It waits around
 * for the clerks to finish making the cones, and then gets on line to check
 * out.
 */
static void customer(size_t id, size_t numConesWanted,
                     inspection_t& inspection, checkout_t& checkout) {
  // Make a vector of clerk threads, one per cone to be ordered
  vector<thread> clerks(numConesWanted);
  for (size_t i = 0; i < clerks.size(); i++) {
    clerks[i] = thread(clerk, i, id, ref(inspection));
  }

  // The customer browses for some amount of time, then joins the clerks.
  browse(); // utility function, sleeps (see ice-cream-store-utils.h)
  for (thread& clerk: clerks) clerk.join();

  /* Now we are ready to check out.  Get our unique place in line -
   * note that ++ is atomic because nextPlaceInLine is atomic.  
   * This entire line is *not* atomic, but that's ok because the operation 
   * of getting a unique place in line is.
   */
  size_t place = checkout.nextPlaceInLine++;
  cout << oslock << "Customer " << id << " assumes position #" << place
       << " at the checkout counter." << endl << osunlock;

  // Tell the cashier that we are ready to check out
  checkout.waitingCustomers.signal();

  // Wait on our unique semaphore so we know when it is our turn
  checkout.customers[place].wait();
  cout << oslock << "Customer " << id
       << " has checked out and leaves the ice cream store." 
       << endl << osunlock;
}

int main(int argc, const char *argv[]) {
  // Make an array of customer threads, and add up how many cones they order
  size_t totalConesOrdered = 0;
  thread customers[kNumCustomers];

  /* The structs to package up variables needed for cone inspection and 
   * customer checkout
   */
  inspection_t inspection;
  checkout_t checkout;

  for (size_t i = 0; i < kNumCustomers; i++) {
    // utility function, random (see ice-cream-store-utils.h)
    size_t numConesWanted = getNumCones();
    customers[i] = thread(customer, i, numConesWanted, 
      ref(inspection), ref(checkout));
    totalConesOrdered += numConesWanted;
  }

  /* Make the manager and cashier threads to approve cones / checkout customers.
   * Tell the manager how many cones will be ordered in total. */
  thread managerThread(manager, totalConesOrdered, ref(inspection));
  thread cashierThread(cashier, ref(checkout));
  
  for (thread& customer: customers) customer.join();
  cashierThread.join();
  managerThread.join();

  return 0;
}
