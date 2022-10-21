/**
 * File: blocking-alphabet-client.cc
 * ---------------------------------
 * This program uses traditional networking and 
 * low-level (but otherwise traditional) I/O to 
 * ingest the entire alphabet in from the
 * slow-alphabet-server.
 */

#include <fcntl.h>    // for open
#include <string>     // for C++ string class
#include <iostream>   // for cout, cerr, endl
#include <cassert>    // for assert macro
#include <unistd.h>   // for close
#include "client-socket.h"
#include "non-blocking-utils.h"
using namespace std;

static const unsigned short kSlowAlphabetServerPort = 41411;
int main(int argc, char *argv[]) {
    int client = createClientSocket("localhost", kSlowAlphabetServerPort);
    assert(client != -1);
    assert(isBlocking(client));
    
    size_t numReads = 0, numSuccessfulReads = 0;
    while (true) {
        char ch;
        ssize_t count = read(client, &ch, 1);
        assert(count != -1);
        numReads++;
        if (count == 0) break;
        assert(count == 1);
        numSuccessfulReads++;
        cout << ch << flush;
    }
    close(client);
    cout << endl;
    cout << "Number of reads: " << numReads << endl;    
    cout << "Alphabet length: " << numSuccessfulReads << endl;
    return 0;
}
