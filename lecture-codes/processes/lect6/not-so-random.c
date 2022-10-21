/**
 * File: not-so-random.c
 * --------------------
 * An example that shows how deep the copy goes when you fork a process.
 */
#include <stdbool.h>  // for bool
#include <stdlib.h>   // for exit, srandom, random
#include <unistd.h>   // for fork
#include <stdio.h>    // for printf, etc
#include <time.h>     // for time
#include "exit-utils.h"

static const int kForkFailure = 1;

int main(int argc, char *argv[]) {
    // Initialize the random number with a "seed value"
    // this seed state is used to generate future random numbers
    srandom(time(NULL));

    printf("This program will make you question what 'randomness' means...\n");
    pid_t pidOrZero = fork();
    exitIf(pidOrZero == -1, kForkFailure, stderr, "Call to fork failed... aborting.\n");

    // Parent goes first - both processes *always* get the same roll (why?)
    if (pidOrZero != 0) {
        int diceRoll = (random() % 6) + 1;
        printf("I am the parent and I rolled a %d\n", diceRoll);
        sleep(1);
    } else {
        sleep(1);
        int diceRoll = (random() % 6) + 1;
        printf("I am the child and I'm guessing the parent rolled a %d\n", diceRoll);
    }

    return 0;
}
