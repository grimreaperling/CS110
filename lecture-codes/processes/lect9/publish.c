#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static void publish(const char *name) {
  printf("Publishing date and time to file named \"%s\".\n", name);
  int outfile = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  dup2(outfile, STDOUT_FILENO);
  close(outfile);
  if (fork() > 0) return;
  char *argv[] = { "date", NULL };
  execvp(argv[0], argv);
}

int main(int argc, char *argv[]) {
  for (size_t i = 1; i < argc; i++) publish(argv[i]);
  return 0;
}
