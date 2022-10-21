/**
 * File: parent-child.c
 * --------------------
 * Basic example to introduce the fork and waitpid functions working together.
 * They're so neat. :)
 */

#include <stdbool.h>  // for bool
#include <stdlib.h>   // for exit, srandom, random
#include <unistd.h>   // for fork
#include <stdio.h>    // for printf, etc
#include <sys/wait.h> // for waitpid
#include <time.h>     // for time

int main(int argc, char *argv[]) {
  srandom(time(NULL));
  printf("I get printed once!\n");
  pid_t pid = fork(); // returns 0 within child, returns pid of child within fork
  bool parent = pid != 0;
  if ((random() % 2 == 0) == parent) {
    sleep(1); // force exactly one of the two to sleep
    printf("Ah, naps are the best!\n"); // brag
  }
  if (parent) waitpid(pid, NULL, 0); // parent shouldn't exit until it knows its child has finished
  printf("I get printed twice (this one is being printed from the %s).\n", parent  ? "parent" : "child");
  return 0;
}
