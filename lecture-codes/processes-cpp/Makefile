# CS110 Multiprocessing examples, but in C++
PROGS = xargs five-children confused-pentuplets pentuplets five-nsync-children guitar
CXX = /usr/bin/g++

CXX_WARNINGS = -Wall -pedantic -Wno-vla
CXX_DEPS = -MMD -MF $(@:.o=.d)
CXX_DEFINES =
CXX_INCLUDES = -I/afs/ir.stanford.edu/class/cs110/local/include

CXXFLAGS = -g $(CXX_WARNINGS) -O0 -std=c++0x $(CXX_DEPS) $(CXX_DEFINES) $(CXX_INCLUDES)
LDFLAGS = 

PROGS_SRC = $(patsubst %,%.cc,$(PROGS))
PROGS_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(PROGS_SRC)))
PROGS_DEP = $(patsubst %.o,%.d,$(PROGS_OBJ))

all:: $(PROGS)

$(PROGS): %:%.o
	$(CXX) $^ $(LDFLAGS) -o $@

clean::
	rm -f $(PROGS) $(PROGS_OBJ) $(PROGS_DEP)

spartan:: clean
	\rm -fr *~

.PHONY: all clean spartan

-include $(PROGS_DEP)
