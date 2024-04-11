#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void function(int signo)
{
  if (signo == SIGTERM)
    printf("SIGTERM received\n");

  if (signo == SIGINT)
    printf("SIGINT received\n");

  printf("Emergency exit\n");

  exit(1);
}

int main(void)
{
  signal(SIGTERM, function);
  signal(SIGINT, function);

  printf("Waiting for signal!\n");
  pause(); 

  return 0;
}
