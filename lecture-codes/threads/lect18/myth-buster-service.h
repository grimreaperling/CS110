/**
 * File: myth-buster-service.h
 * ---------------------------
 * Presents the interface to a function that can return the 
 * number of processes being run on a myth machine by CS110 
 * students (or whatever students are identified by SUNet ID in
 * the provided set).
 */

#ifndef _myth_buster_service_
#define _myth_buster_service_

#include <unordered_set>
#include <string>

/**
 * Returns the number of processes running on the specific myth 
 * machine (e.g. 55 would be provided on behalf of myth55), filtered to
 * include only for SUNet IDs contained in the provided set.
 * If the host isn't reachable or requires authentication not normally
 * required when called from another myth machine, -1 is returned to denote
 * failure.
 */

int getNumProcesses(int num, const std::unordered_set<std::string>& sunetIDs);

#endif
