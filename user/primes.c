#include "kernel/types.h"
#include "user/user.h"

void primes(int left_read) __attribute__((noreturn));
void primes(int left_read) {
  int prime;
  if (read(left_read, &prime, sizeof(prime)) != sizeof(prime)) {
    close(left_read);
    exit(0);
  }
  printf("prime %d\n", prime);

  int p[2];
  int n;
  int has_child = 0;
  pipe(p);
  while (read(left_read, &n, sizeof(n)) == sizeof(n)) {
    if (n % prime != 0) {
      if (!has_child) {
        if (fork() == 0) {
          close(p[1]); // 子进程关闭写端
          close(left_read); // 子进程关闭父管道读端
          primes(p[0]);
        }
        has_child = 1;
        close(p[0]); // 父进程关闭新管道读端
      }
      write(p[1], &n, sizeof(n));
    }
  }
  close(left_read); // 父进程关闭父管道读端
  close(p[1]); // 父进程关闭新管道写端
  if (has_child) wait(0); // 等待子进程
  exit(0);
}

int main() {
  int p[2];
  pipe(p);
  if (fork() == 0) {
    close(p[1]); // 子进程关闭写端
    primes(p[0]);
  }
  close(p[0]); // 父进程关闭读端
  for (int i = 2; i <= 280; i++) {
    write(p[1], &i, sizeof(i));
  }
  close(p[1]); // 父进程关闭写端
  wait(0); // 等待子进程
  exit(0);
}
