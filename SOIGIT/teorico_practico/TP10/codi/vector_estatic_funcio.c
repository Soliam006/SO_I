#include <stdio.h>

void funcio()
{
  int a[1000];
  int b;

  b = 5;

  printf("Direccio de b: %lx\n", (unsigned long) &b);
  printf("Valor de b: %d\n", b);

  printf("Direcció de a[0]: %lx\n", (unsigned long) &a[0]);
  printf("Direccio de a[-1]: %lx\n", (unsigned long) &a[-1]);

  a[-1] = 10;

  printf("Valor de b: %d\n", b);
}

int main(void)
{
  funcio();
}
