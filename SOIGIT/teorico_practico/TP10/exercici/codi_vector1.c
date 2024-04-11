#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  int i;
  int *a;

  a = malloc(10 * sizeof(int));

  for(i = 0; i < 20; i++) {
      printf("Escric a la posicio %d\n", i);
      a[i] = i;
  }

  free(a);

  return 0;
}
