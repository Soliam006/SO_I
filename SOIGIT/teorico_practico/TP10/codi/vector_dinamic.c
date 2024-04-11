#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  int i;
  int *a;

  a = malloc(10 * sizeof(int));

  i = 0;
  while (1)
  {
    a[i] = 2 * i;
    printf("a[%d] = %d\n", i, a[i]);
    i++;
  }
  
  free(a);

  return 0;
}
