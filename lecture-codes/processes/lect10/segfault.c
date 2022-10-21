/* Program: segfault.c
 * -------------------
 * This program immediately crashes with a segmentation fault (SIGSEGV);
 */

int main(int argc, char *argv[]) {
  return *(int *)0;
}
