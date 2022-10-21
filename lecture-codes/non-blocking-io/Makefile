# CS110 C++ Threading and Concurrency Examples

CXX = clang++-10

# The CPPFLAGS variable sets compile flags for g++:
#  -g          compile with debug information
#  -Wall       give all diagnostic warnings
#  -Wno-vla    allow array declarations like char buffer[expression]
#  -pedantic   require compliance with ANSI standard
#  -O0         do not optimize generated code
#  -std=c++20  go with the c++20 extensions for thread support (and other nifty things)
#  -D_GLIBCXX_USE_NANOSLEEP included for this_thread::sleep_for and this_thread::sleep_until support
#  -D_GLIBCXX_USE_SCHED_YIELD included for this_thread::yield support
CPPFLAGS = -g -Wall -Wno-vla -Wno-pedantic -O0 -std=c++20 -D_GLIBCXX_USE_NANOSLEEP -D_GLIBCXX_USE_SCHED_YIELD -I/usr/class/cs110/local/include/ -I/usr/class/cs110/include/

# The LDFLAGS variable sets flags for linker
#  -lm       link in libm (math library)
#  -lpthread link in libpthread (thread library) to back C++20 extensions
LDFLAGS = -lm -pthread -L/usr/class/cs110/local/lib -lthreadpoolrelease -L/usr/class/cs110/local/lib/ -lthreads -lrand -L/usr/class/cs110/lib/socket++ -lsocket++ -Wl,-rpath=/usr/class/cs110/lib/socket++

# In this section, you list the files that are part of the project.
# If you add/change names of header/source files, here is where you
# edit the Makefile.
HEADERS = server-socket.h client-socket.h non-blocking-utils.h outbound-file.h thread-pool.h
SOURCES = slow-alphabet-server.cc blocking-alphabet-client.cc non-blocking-alphabet-client.cc \
	  load-large-file-blocking-descriptors.cc load-large-file-non-blocking-descriptors.cc \
	  outbound-file-test.cc expensive-server.cc expensive-server-test.cc \
	  steppenwolf-server.cc efficient-server.cc efficient-server-test.cc
EXTRAS = server-socket.cc client-socket.cc non-blocking-utils.cc outbound-file.cc
OBJECTS = $(SOURCES:.cc=.o) $(EXTRAS:.cc=.o)
TARGETS = $(SOURCES:.cc=)

default: $(TARGETS)

slow-alphabet-server: slow-alphabet-server.o non-blocking-utils.o server-socket.o
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

blocking-alphabet-client: blocking-alphabet-client.o client-socket.o non-blocking-utils.o
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

non-blocking-alphabet-client: non-blocking-alphabet-client.o client-socket.o non-blocking-utils.o
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

load-large-file-blocking-descriptors: load-large-file-blocking-descriptors.o non-blocking-utils.o
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

load-large-file-non-blocking-descriptors: load-large-file-non-blocking-descriptors.o non-blocking-utils.o
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

outbound-file-test: outbound-file-test.o outbound-file.o non-blocking-utils.o
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

expensive-server: expensive-server.o outbound-file.o non-blocking-utils.o server-socket.o
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)	
	rm -f expensive-server.cc.html
	echo -e 'HTTP/1.0 200 OK\r' >> expensive-server.cc.html
	echo -e '\r' >> expensive-server.cc.html
	cat expensive-server.cc >> expensive-server.cc.html

expensive-server-test: expensive-server-test.o
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

steppenwolf-server: steppenwolf-server.o non-blocking-utils.o server-socket.o
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

efficient-server: efficient-server.o non-blocking-utils.o server-socket.o
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

efficient-server-test: efficient-server-test.o client-socket.o
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

# In make's default rules, a .o automatically depends on its .cc file
# (so editing the .cc will cause recompilation into its .o file).
# The line below creates additional dependencies, most notably that it
# will cause the .cc to recompiled if any included .h file changes.

Makefile.dependencies:: $(SOURCES) $(HEADERS)
	$(CXX) $(CPPFLAGS) -MM $(SOURCES) > Makefile.dependencies

-include Makefile.dependencies

# Phony means not a "real" target, it doesn't build anything
# The phony target "clean" is used to remove all compiled object files.
# The phony target "spartan" is used to remove all compilation products and extra backup files. 

.PHONY: clean spartan

clean:
	@rm -f $(TARGETS) $(OBJECTS) core Makefile.dependencies expensive-server.cc.html

spartan: clean
	@rm -f *~
