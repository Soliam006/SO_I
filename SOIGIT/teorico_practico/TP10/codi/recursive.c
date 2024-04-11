#include <stdio.h>

void funcio(int a)
{
  int b[1000];
  b[0]=0; // Evitem que es queixi el compilador
  b[0]=b[0]+2; // Evitem que es queixi el compilador
  printf("%d\n", a);
  funcio(a+1);
}

int main(void)
{
  funcio(0);

  return 0;
}
