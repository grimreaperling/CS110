/**
 * File: outbound-file.cc
 * ----------------------
 * Presents the implementation of the OutboundFile class.
 */

#include "outbound-file.h"
#include "non-blocking-utils.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <sys/socket.h>
#include <signal.h>
#include <cassert>
using namespace std;

OutboundFile::OutboundFile() {
  isSending = false;
}

void OutboundFile::initialize(const string& source, int sink) {
  this->source = open(source.c_str(), O_RDONLY | O_NONBLOCK);
  this->sink = sink;
  setAsNonBlocking(this->sink);
  numBytesAvailable = numBytesSent = 0;
  isSending = true;
}

bool OutboundFile::sendMoreData() {
  if (!isSending) return !allDataFlushed();  
  if (!dataReadyToBeSent()) {
    readMoreData();
    if (!dataReadyToBeSent()) 
      return true;
  }  
  writeMoreData();
  return true;
}

bool OutboundFile::dataReadyToBeSent() const {
  return numBytesSent < numBytesAvailable;
}

void OutboundFile::readMoreData() {
  ssize_t incomingCount = read(source, buffer, kBufferSize);
  if (incomingCount == -1) {
    assert(errno == EWOULDBLOCK);
    return;
  }

  numBytesAvailable = incomingCount;
  numBytesSent = 0;
  if (numBytesAvailable > 0) return;
  
  close(source);
  if (isSocketDescriptor(sink)) shutdown(sink, SHUT_WR);
  else setAsBlocking(sink);
  isSending = false;
}

void OutboundFile::writeMoreData() {
  auto old = signal(SIGPIPE, SIG_IGN);
  ssize_t outgoingCount = write(sink, buffer + numBytesSent, 
                                numBytesAvailable - numBytesSent);
  signal(SIGPIPE, old);
  if (outgoingCount == -1) {
    if (errno == EPIPE) {
      isSending = false;
    } else {
      assert(errno == EWOULDBLOCK);
    } 
  } else {
    numBytesSent += outgoingCount;
  }
}

bool OutboundFile::allDataFlushed() {
  bool allBytesFlushed;
  if (isSocketDescriptor(sink)) {
    assert(isNonBlocking(sink));
    ssize_t count = read(sink, buffer, sizeof(buffer));
    allBytesFlushed = count == 0;
  } else {
    assert(isBlocking(sink));
    int numOutstandingBytes = 0;
    ioctl(sink, SIOCOUTQ, &numOutstandingBytes);
    allBytesFlushed = numOutstandingBytes == 0;
  }
  
  if (allBytesFlushed) close(sink);
  return allBytesFlushed;
}
