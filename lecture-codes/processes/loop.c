/**
 * File: loop.c
 * ------------
 * Simple program that just runs forever.
 */

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  while (true) {
    printf("I am going to sleep!\n");
    sleep(10);
    printf("I am awake!\n");
    sleep(1);
  }
  return 0;
}
