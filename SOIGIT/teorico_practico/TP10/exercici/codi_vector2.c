#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  int *a;

  a = malloc(10 * sizeof(int));

  printf("Escric a la posicio 5\n");
  a[5] = 10;

  free(a);

  printf("Torno a escriure a la posicio 5\n");
  a[5] = 10;

  return 0;
}
