#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  int i;
  int *a, *b;

  a = malloc(100 * sizeof(int));

  for(i = 0; i < 100; i++)
    a[i] = 2 * i;

  free(a);

  b = malloc(1000 * sizeof(int));

  b[50] = 1000;
  printf("a[50] = %d\n", a[50]);

  free(b);

  return 0;
}
