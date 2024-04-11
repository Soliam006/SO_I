#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sigusr1(int signo)
{
  printf("El pare ha rebut el SIGUSR1\n");
}

void sigusr2(int signo)
{
  printf("El fill ha rebut el SIGUSR2\n");
}

int main(void)
{
  int ret, parent_pid, child_pid;

  ret = fork();

  if (ret == 0) {

    signal(SIGUSR2, sigusr2);
    parent_pid = getppid();

    while (1) {
      pause();
      printf("Fill espera 5 segons\n");
      sleep(5);
      printf("Envio SIGUSR1 al pare %d\n", parent_pid);
      kill(parent_pid, SIGUSR1);
    }

    exit(0);
  } 
  else {

    signal(SIGUSR1, sigusr1);
    child_pid = ret;

    while (1) {
      printf("Pare espera 5 segons\n");
      sleep(5);
      printf("Envio SIGUSR2 al fill %d\n", child_pid);
      kill(child_pid, SIGUSR2);
      pause();
    }

    exit(0);
  }

  return 0;
}
