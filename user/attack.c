#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int
main(int argc, char *argv[])
{
  #define PGSIZE 4096
  char *buf = malloc(PGSIZE);
  for (int i = 0; i < PGSIZE - 8; i++) {
    write(2, buf + i, 8);
  }
  free(buf);
  exit(0);
}
