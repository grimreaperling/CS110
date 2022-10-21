/**
 * File: outbound-file.h
 * ---------------------
 * Defines the OutboundFile class, which understands how to
 * syndicate the contents of a local file to a remote machine
 * using non-blocking I/O.
 */

#pragma once
#include "server-socket.h"
#include <cstddef>
#include <string>

class OutboundFile {
 public:
/**
 * Constructor: OutboundFile
 * -------------------------
 * Defaultly constructs the OutboundFile object.  A defaultly
 * constructed OutboundFile object isn't associated with any local
 * file or any form of client connection to a client.  Those
 * associations are established via the initialize method below.
 */
  OutboundFile();

/**
 * Method: initialize
 * ------------------
 * Configures the presumably idle and disconnected OutboundFile
 * so that it can push the contents of the specified (local) file 
 * out over the supplied, previously configured connection.
 *
 * The file contents are accessed via a nonblocking file descriptor,
 * and the supplied socket connection (expressed via sink) is converted
 * to a nonblocking socket as well.
 *
 * No data is actually sent as a result of this method call; it simply
 * sets the OutboundFile object up so that sendReadilyAvailableData can
 * be called and executed as quickly as possible.
 */
  void initialize(const std::string& source, int sink);

/**
 * Method: sendMoreData
 * --------------------
 * Sends as much data as can possibly be sent without blocking
 * on any I/O whatsoever.  Returns true if there's more data to be sent
 * and more calls to sendMoreData are needed, and false
 * if all data (e.g. the full contents of the local file presented 
 * to initialize) has been sent and reached the client.
 */
  bool sendMoreData();
  
 private:
  int sink, source;
  static const size_t kBufferSize = 128;
  char buffer[kBufferSize];
  size_t numBytesAvailable;
  size_t numBytesSent;
  bool isSending;

  bool dataReadyToBeSent() const;
  void readMoreData();
  void writeMoreData();
  bool allDataFlushed();
};
