/**
 * File: non-blocking-utils.h
 * --------------------------
 * Exports the setAsNonBlocking routine.
 */

#pragma once

/**
 * Function: isSocket
 * ------------------
 * Returns true if and only if the provided file desciptor is a socket
 * descriptor.
 */
bool isSocketDescriptor(int descriptor);

/**
 * Function: setAsNonBlocking
 * --------------------------
 * Assumes the provided descriptor is open and in a good place,
 * and converts it to be a nonblocking descriptor, which means
 * that read and write calls on the descriptor will only
 * do as much as they can *immediately* and return without any blocking
 * whatsoever.
 */
void setAsNonBlocking(int descriptor);

/**
 * Function: setAsBlocking
 * -----------------------
 * Assumes the provided descriptor is open and in a good place,
 * and converts it to be a blocking descriptor, which means that
 * read and write calls on the descriptor will block if data isn't
 * available yet or if the output buffers are congested.
 */
void setAsBlocking(int descriptor);

/**
 * Function: isNonBlocking
 * -----------------------
 * Returns true if and only if the presented descriptor is a nonblocking one.
 */
bool isNonBlocking(int descriptor);

/**
 * Function: isBlocking
 * --------------------
 * Returns true if and only if the presented descriptor is a blocking one.
 */
bool isBlocking(int descriptor);
