/**
 * File: fork-copy.c
 * -----------------
 * This program creates a child process where both the
 * parent and child seemingly access or modify the same
 * string at the same address.  But the string is different
 * for both the parent and child.  How is this?
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char str[128];
    strcpy(str, "Hello");
    printf("str's address is %p\n", str);
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // The child should modify str
        printf("I am the child. str's address is %p\n", str);
        strcpy(str, "Howdy");
        printf("I am the child and I changed str to %s. str's address is still %p\n", str, str);
    } else {
        // The parent should sleep and print out str
        printf("I am the parent. str's address is %p\n", str);
        printf("I am the parent, and I'm going to sleep for 2 seconds.\n");
        sleep(2);
        printf("I am the parent. I just woke up. str's address is %p, and its value is %s\n", str, str);
    }

    return 0;
}
