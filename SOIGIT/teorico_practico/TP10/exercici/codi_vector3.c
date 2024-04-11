#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  int *a;

  a = malloc(10 * sizeof(int));

  printf("Valor d'a[5]: %d\n", a[5]);

  free(a);

  return 0;
}
