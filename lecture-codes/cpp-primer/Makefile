# CS110 C++ Crash Course Makefile 
CXX = /usr/bin/g++

# The CXXFLAGS variable sets compile flags for gcc:
#  -g                         compile with debug information
#  -Wall                      give all diagnostic warnings
#  -pedantic                  require compliance with ANSI standard
#  -O0                        do not optimize generated code
#  -std=c++0x                 go with the c++0x extensions for thread support, unordered maps, et
CXXFLAGS = -g -Wall -pedantic -O0 -std=c++0x -I/usr/class/cs110/local/include -I/usr/include/libxml2
LDFLAGS = -lxml2

# In this section, you list the files that are part of the project.
# If you add/change names of header/source files, here is where you
# edit the Makefile.
CLASSES = couple.cc stream-tokenizer.cc
HEADERS = $(CLASSES:.cc=.h)
PROGRAMS = dinner-party.cc find.cc stl-algorithm-showcase.cc vector-showcase.cc map-showcase.cc
SOURCES = $(CLASSES) $(PROGRAMS)
OBJECTS = $(SOURCES:.cc=.o)
TARGETS = $(PROGRAMS:.cc=)

default: $(TARGETS)

dinner-party: dinner-party.o couple.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

find: find.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

stl-algorithm-showcase: stl-algorithm-showcase.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

vector-showcase: vector-showcase.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

map-showcase: map-showcase.o stream-tokenizer.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# In make's default rules, a .o automatically depends on its .c file
# (so editing the .cc will cause recompilation into its .o file).
# The line below creates additional dependencies, most notably that it
# will cause the .c to recompiled if any included .h file changes.

Makefile.dependencies:: $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -MM $(SOURCES) > Makefile.dependencies

-include Makefile.dependencies

# Phony means not a "real" target, it doesn't build anything
# The phony target "clean" is used to remove all compiled object files.
# The phony target "spartan" is used to remove all compilation products and extra backup files. 

.PHONY: clean spartan

clean:
	@rm -f $(TARGETS) $(OBJECTS) core Makefile.dependencies

spartan: clean
	@rm -f *~
