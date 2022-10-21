/**
 * File: ice-cream-store-utils.h
 * ------------------------------
 * Defines a collection of utility functions
 * to assist in the implementation of the
 * ice-cream-store simulation.
 */

#pragma once

/**
 * Utility function: getNumCones
 * -----------------------------
 * Returns a small random number of ice cream
 * cones to be made.
 */
size_t getNumCones();

/**
 * Utility functions: browse and makeCone
 * --------------------------------------
 * browse and makeCone are called by customers and
 * clerks to stall and/or emulate the time it might take
 * to do something while working and waiting in an ice cream store.
 */
void browse();
void makeCone(size_t coneID, size_t customerID);

/**
 * Utility function: inspectCone
 * -----------------------------
 * Emulates the work done by a manager to inspect
 * a single ice cream cone and come back with an
 * arbitrary yes or no as to whether the ice cream
 * code is good enough to be served.
 */
bool inspectCone();
