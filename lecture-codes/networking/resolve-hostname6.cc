/**
 * File: resolve-hostname.cc
 * -------------------------
 * This is a simple little program that helps resolve IPv6 addresses.  It's operationally
 * the same as resolve-hostname.cc, save for the faact that all IP addresses are expressed
 * as IPv6 addresses.
 */

#include <iostream>        // for cout
#include <cassert>         // assert macro
#include <netdb.h>         // for gethostbyname2
#include <sys/socket.h>    // for inet_ntop
#include <netinet/in.h>    // for inet_ntop
#include <arpa/inet.h>     // for inet_ntop
#include "string-utils.h"
using namespace std;

/**
 * Function: publishIPv6AddressInfo
 * ------------------------------
 * Relies on the services of the inet_ntop and gethostbyname2 functions to produce
 * the official host name for the supplied one (sometimes the same, and sometimes
 * different) and all of the IPv6 addresses that the supplied host name resolves to.
 */
static void publishIPv6AddressInfo(const string& host) {
  struct hostent *he = gethostbyname2(host.c_str(), AF_INET6);
  if (he == NULL) { // NULL return value means resolution attempt failed
    cout << host << " could not be resolved to an address.  Did you mistype it?" << endl;
    return;
  }
  
  assert(he->h_addrtype == AF_INET6);
  assert(he->h_length == sizeof(struct in6_addr));
  
  cout << "Official name is \"" << he->h_name << "\"" << endl;
  cout << "IPv6 Addresses: " << endl;
  struct in6_addr **addressList = (struct in6_addr **) he->h_addr_list;
  while (*addressList != NULL) {
    char str[INET6_ADDRSTRLEN];
    cout << "+ " << inet_ntop(AF_INET6, *addressList, str, INET6_ADDRSTRLEN) << endl;
    addressList++;
  }
}

/**
 * Function: resolveHostnames
 * --------------------------
 * Straightforward query loop asking for the user to type in what's expected to look like a host name
 * (e.g. "www.facebook.com") or an IPv6 address (e.g. "2a03:2880:f131:83:face:b00c:0:25de")
 */
static void resolveHostnames() {
  while (true) {
    cout << "Enter a host name: ";
    string host;
    getline(cin, host);
    trim(host);
    if (host.empty()) break;
    publishIPv6AddressInfo(host);
  }  
  cout << "All done!" << endl;
}

/**
 * Function: main
 * --------------
 * Straightforward wrapper around resolveHostname, 
 * which does all of the interesting work.
 */
int main(int argc, char *argv[]) {
  cout << "Welcome to the IPv6 address resolver!" << endl;
  resolveHostnames();
  return 0;
}
