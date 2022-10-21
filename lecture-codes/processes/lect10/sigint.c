/* Program: sigint.c
 * -----------------
 * This program installs a SIGINT handler to taunt the user instead
 * of the default behavior of terminating the program.  (Use Ctl-z instead
 * to stop the program, and then kill -SIGKILL [PID] to terminate it).
 */

#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

static char *taunts[] = {
  "I'm ignoring you.",
  "Nice try!  I'm not going anywhere.",
  "You think you can terminate me that easily?",
  "Hah!  Is that all you got?",
  "'tis but a scratch!",
  "I could do this all day."
};

static char *getRandomTaunt() {
  return taunts[random() % (sizeof(taunts) / sizeof(taunts[0]))];
}

static void handleSIGINT(int sig) {
  printf("\n%s\n", getRandomTaunt());
}

int main(int argc, char *argv[]) {
  // Initialize the random number with a "seed value"
  // this seed state is used to generate future random numbers
  srandom(time(NULL));

  signal(SIGINT, handleSIGINT);
  printf("Just try to interrupt me!\n");
  while (true) {
    sleep(1);
  }
  return 0;
}