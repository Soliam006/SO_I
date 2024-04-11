#include <stdio.h>
#include <stdlib.h>

#define NCOLS 256

void funcio(int nrows)
{
  char matriu[nrows][NCOLS];

  printf("Faig assignacio i operacio\n");
  // Aixo es fa perque el compilador no es queixi
  // que no es fa servir la variable matriu
  matriu[0][0] = 0;
  matriu[0][0] = matriu[0][0] + 1;
  printf("Surto assignacio i he fet operacio\n");
}

int main(int argc, char **argv)
{
  int nrows;

  if (argc != 2) {
    printf("Us: %s <nrow>\n", argv[0]);
    exit(1);
  }

  nrows = atoi(argv[1]);

  printf("Valor de nrows: %d\n", nrows);
  printf("Crido a funcio\n");

  funcio(nrows);

  printf("Surto de funcio, tot ha anat be!\n");

  return 0;
} 
