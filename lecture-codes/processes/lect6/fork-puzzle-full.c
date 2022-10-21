/**
 * File: fork-puzzle-full.c
 * -------------------
 * Larger example illustrating fork and the potential for
 * many possible outcomes that result from the concurrency
 * and the various ways all of the children, grandchildren,
 * etc. can be scheduled.
 */

#include <unistd.h>     // for fork
#include <stdio.h>      // for printf, etc
#include <string.h>     // for strlen
#include <assert.h>     // for assert

static const char *kTrail = "abcd";

int main(int argc, char *argv[]) {
    for (int i = 0; i < strlen(kTrail); i++) {
        printf("%c\n", kTrail[i]);
        pid_t pidOrZero = fork();
        assert(pidOrZero >= 0);
    }
    return 0;
}
