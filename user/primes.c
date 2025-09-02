#include "kernel/types.h"
#include "user/user.h"

void primes(int left_read) __attribute__((noreturn));

void primes(int left_read) {
  int prime;
  int n;
  int p[2];
  int pid;
  
  // 读取第一个数字作为质数
  if (read(left_read, &prime, sizeof(prime)) == 0) {
    close(left_read);
    exit(0);
  }
  printf("prime %d\n", prime);

  // 创建管道用于传递过滤后的数字
  if (pipe(p) < 0) {
    fprintf(2, "pipe failed\n");
    exit(1);
  }

  pid = fork();
  if (pid < 0) {
    fprintf(2, "fork failed\n");
    exit(1);
  }
  
  if (pid == 0) {
    // 子进程：处理过滤后的数字
    close(p[1]);
    close(left_read);
    primes(p[0]);
    exit(0);
  } else {
    // 父进程：过滤数字并传递给子进程
    close(p[0]);
    while (read(left_read, &n, sizeof(n)) == sizeof(n)) {
      if (n % prime != 0) {
        write(p[1], &n, sizeof(n));
      }
    }
    close(left_read);
    close(p[1]);
    wait(0);
    exit(0);
  }
}

int main() {
  int p[2];
  int pid;
  
  if (pipe(p) < 0) {
    fprintf(2, "pipe failed\n");
    exit(1);
  }
  
  pid = fork();
  if (pid < 0) {
    fprintf(2, "fork failed\n");
    exit(1);
  }
  
  if (pid == 0) {
    // 子进程：开始筛选质数
    close(p[1]);
    primes(p[0]);
    exit(0);
  } else {
    // 父进程：生成 2 到 280 的数字
    close(p[0]);
    for (int i = 2; i <= 280; i++) {
      if (write(p[1], &i, sizeof(i)) != sizeof(i)) {
        fprintf(2, "write failed\n");
        break;
      }
    }
    close(p[1]);
    wait(0);
    exit(0);
  }
}
