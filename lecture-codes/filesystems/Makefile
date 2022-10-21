# CS110 Filesystem Makefile 
PROGS = copy copy-soln copy-soln-full copy-extended copy-extended-soln copy-extended-soln-full touch
CC = gcc

CC_WARNINGS = -Wall -pedantic
CC_DEPS = -MMD -MF $(@:.o=.d)
CC_DEFINES =
CC_INCLUDES = -I/usr/class/cs110/local/include/

CFLAGS = -g $(CC_WARNINGS) -O0 -std=gnu99 $(CC_DEPS) $(CC_DEFINES) $(CC_INCLUDES)
LDFLAGS =

PROGS_SRC = $(patsubst %,%.c,$(PROGS))
PROGS_OBJ = $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(PROGS_SRC)))
PROGS_DEP = $(patsubst %.o,%.d,$(PROGS_OBJ))

all: $(PROGS)

$(PROGS): %:%.o
	$(CC) $^ $(LDFLAGS) -o $@

clean::
	rm -f $(PROGS) $(PROGS_OBJ) $(PROGS_DEP) empty

spartan:: clean
	\rm -fr *~

.PHONY: all clean spartan

-include $(PROGS_DEP)
