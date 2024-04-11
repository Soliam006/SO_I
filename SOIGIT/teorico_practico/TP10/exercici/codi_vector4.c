#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  int *a;

  a = malloc(10 * sizeof(int));

  a[5] = 10;
  printf("Valor d'a[5]: %d\n", a[5]);

  return 0;
}
