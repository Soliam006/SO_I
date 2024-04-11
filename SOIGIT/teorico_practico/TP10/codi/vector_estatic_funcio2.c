#include <stdio.h>

int funcio(void)
{
  int a[10];
  int i;

  printf("Entro al bucle\n");

  for(i = 12; i < 20; i++)
  {
    a[i] = 2 * i;
    printf("a[%d] = %d\n", i, a[i]);

  }

  printf("Surto del bucle\n");

  return  0;
}

int main(void)
{
  printf("Entro a la funcio\n");
  funcio();
  printf("Surto de la funcio\n");
}
