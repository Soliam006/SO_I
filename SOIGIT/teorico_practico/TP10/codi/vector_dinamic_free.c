#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  int i;
  int *a;

  a = malloc(100 * sizeof(int));

  for(i = 0; i < 100; i++)
    a[i] = 2 * i;

  free(a);

  printf("a[50] = %d\n", a[50]);

  return 0;
}
