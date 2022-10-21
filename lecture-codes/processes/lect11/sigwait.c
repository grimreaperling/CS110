/* Program: sigwait.c
 * -----------------
 * This program is the same as sigint.c but instead of using a signal handler
 * to handle SIGINT it waits for SIGTSTP signals using sigwait and then prints
 * out a message to the user instead of the default behavior of terminating the program.  
 * (Use Ctl-c instead to stop the program).
 */

#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

static char *taunts[] = {
  "I'm ignoring you.",
  "Nice try!  I'm not going anywhere.",
  "You think you can stop me that easily?",
  "Hah!  Is that all you got?",
  "'tis but a scratch!",
  "I could do this all day."
};

static char *getRandomTaunt() {
  return taunts[random() % (sizeof(taunts) / sizeof(taunts[0]))];
}

int main(int argc, char *argv[]) {
  // Initialize the random number with a "seed value"
  // this seed state is used to generate future random numbers
  srandom(time(NULL));

  sigset_t monitoredSignals;
  sigemptyset(&monitoredSignals);
  sigaddset(&monitoredSignals, SIGTSTP);
  sigprocmask(SIG_BLOCK, &monitoredSignals, NULL);

  printf("Just try to Ctl-z me!\n");
  while (true) {
    int delivered;
    sigwait(&monitoredSignals, &delivered);
    printf("\nReceived signal %d: %s\n", delivered, strsignal(delivered));
    printf("%s\n", getRandomTaunt());
  }

  return 0;
}
