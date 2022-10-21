/**
 * File: execvp-demo.c
 * ----------------
 * Demonstrates the build-in execvp() function.
 */

#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char *args[] = {"/bin/ls", "-l", "/usr/class/cs110/lecture-examples", NULL};
    execvp(args[0], args);
    printf("This only prints if an error occurred.\n");
    return 0;
}
