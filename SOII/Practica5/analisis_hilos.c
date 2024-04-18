#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#define MAXCHAR 500
#define LEN_CODE_AIRPORT 3
#define STR_CODE_AIRPORT (LEN_CODE_AIRPORT + 1)
#define NUM_AIRPORTS 303
#define COL_ORIGIN_AIRPORT 17
#define COL_DESTINATION_AIRPORT 18
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int extract_fields_airport(char *origin, char *destination, char *line) 
{
  /*Recorre la linea por caracteres*/
  char caracter;
  /* i sirve para recorrer la linea
   * iterator es para copiar el substring de la linea a char
   * coma_count es el contador de comas
   */
  int i, iterator, coma_count;
  /* start indica donde empieza el substring a copiar
   * end indica donde termina el substring a copiar
   * len indica la longitud del substring
   */
  int start, end, len;
  /* invalid nos permite saber si todos los campos son correctos
   * 1 hay error, 0 no hay error 
   */
  int invalid = 0;
  /* found se utiliza para saber si hemos encontrado los dos campos:
   * origen y destino
   */
  int found = 0;
  /*
   * eow es el caracter de fin de palabra
   */
  char eow = '\0';
  /*
   * contenedor del substring a copiar
   */
  char word[STR_CODE_AIRPORT];
  /*
   * Inicializamos los valores de las variables
   */
  start = 0;
  end = -1;
  i = 0;
  coma_count = 0;
  /*
   * Empezamos a contar comas
   */
  do {
    caracter = line[i++];
    if (caracter == ',') {
      coma_count ++;
      /*
       * Cogemos el valor de end
       */
      end = i;
      /*
       * Si es uno de los campos que queremos procedemos a copiar el substring
       */
      if (coma_count ==  COL_ORIGIN_AIRPORT || coma_count == COL_DESTINATION_AIRPORT) {
        /*
         * Calculamos la longitud, si es mayor que 1 es que tenemos 
         * algo que copiar
         */
        len = end - start;

        if (len > 1) {

          if (len > STR_CODE_AIRPORT) {
            printf("ERROR len code airport\n");
            exit(1);
          }

          /*
           * Copiamos el substring
           */
          for(iterator = start; iterator < end-1; iterator ++){
            word[iterator-start] = line[iterator];
          }
          /*
           * Introducimos el caracter de fin de palabra
           */
          word[iterator-start] = eow;
          /*
           * Comprobamos que el campo no sea NA (Not Available) 
           */
          if (strcmp("NA", word) == 0)
            invalid = 1;
          else {
            switch (coma_count) {
              case COL_ORIGIN_AIRPORT:
                strcpy(origin, word);
                found++;
                break;
              case COL_DESTINATION_AIRPORT:
                strcpy(destination, word);
                found++;
                break;
              default:
                printf("ERROR in coma_count\n");
                exit(1);
            }
          }

        } else {
          /*
           * Si el campo esta vacio invalidamos la linea entera 
           */

          invalid = 1;
        }
      }
      start = end;
    }
  } while (caracter && invalid==0);

  if (found != 2)
    invalid = 1;

  return invalid;
}

int get_index_airport(char *code, char **airports)
{
  int i;

  for(i = 0; i < NUM_AIRPORTS; i++) 
    if (strcmp(code, airports[i]) == 0)
      return i;

  return -1;
}

void print_num_flights_summary(int **num_flights, char **airports)
{
  int i, j, num;

  for(i = 0; i < NUM_AIRPORTS; i++) 
  {
    num = 0;

    for(j = 0; j < NUM_AIRPORTS; j++)
    {
      if (num_flights[i][j] > 0)
        num++;
    }

    printf("Origin: %s -- Number of different destinations: %d\n", airports[i], num);
  }
}

void read_airports(char **airports, char *fname) 
{
  int i;
  char line[MAXCHAR];

  FILE *fp;

  /*
   * eow es el caracter de fin de palabra
   */
  char eow = '\0';

  fp = fopen(fname, "r");
  if (!fp) {
    printf("ERROR: could not open file '%s'\n", fname);
    exit(1);
  }

  i = 0;
  while (i < NUM_AIRPORTS)
  {
    fgets(line, 100, fp);
    line[3] = eow; 

    /* Copiamos los datos al vector */
    strcpy(airports[i], line);

    i++;
  }

  fclose(fp);
}

// Reserva espacio para una matriz de tamaño nrow x ncol
void **malloc_matrix(int nrow, int ncol, size_t size) {
    int i;
    void **ptr;
    ptr = malloc(sizeof(void *) * nrow);
    for (i = 0; i < nrow; i++)
        ptr[i] = malloc(size * ncol);
    return ptr;
}

// Libera una matriz de tamaño con nrow filas
void free_matrix(void **matrix, int nrow) {
    int i;
    for (i = 0; i < nrow; i++)
        free(matrix[i]);
    free(matrix);
}

// Estructura para pasar argumentos a los hilos secundarios
struct ThreadArgs {
    int n_lines;
    char **airports;
    int **num_flights;
    FILE *file;
};

// Función que será ejecutada por los hilos secundarios
void *process_data_block(void *args) {
    struct ThreadArgs *thread_args = (struct ThreadArgs *)args;

    char line[MAXCHAR];
    char origin[STR_CODE_AIRPORT], destination[STR_CODE_AIRPORT];
    int invalid, index_origin, index_destination;
    int lines_read =0;
    int lineas_asignadas = thread_args->n_lines;

    // Lee el bloque de líneas asignado al hilo y solo mientras haya una linea que leer
    while (fgets(line, MAXCHAR, thread_args->file) != NULL && lines_read < lineas_asignadas) {

        invalid = extract_fields_airport(origin, destination, line);

        if (!invalid) {
            index_origin = get_index_airport(origin, thread_args->airports);
            index_destination = get_index_airport(destination, thread_args->airports);

            if ((index_origin >= 0) && (index_destination >= 0)){
                pthread_mutex_lock(&mutex);
                thread_args->num_flights[index_origin][index_destination]++;
                pthread_mutex_unlock(&mutex);
            }
        }
        lines_read++;
        //Si ya leímos la cantidad de líneas asignadas al hilo, pero aún no se ha acabado
        //seguimos leyendo las líneas.
        if (lines_read == lineas_asignadas){ lines_read=0;}
    }

    pthread_exit(NULL);
}

void read_airports_data(int **num_flights, char **airports, char *fname, int H, int lines_per_thread) 
{

    char line[MAXCHAR];
    char origin[STR_CODE_AIRPORT], destination[STR_CODE_AIRPORT];
    int invalid, index_origin, index_destination;

    FILE *fp;

    fp = fopen(fname, "r");
    if (!fp) {
    printf("ERROR: could not open '%s'\n", fname);
    exit(1);
    }

    /* Leemos la cabecera del fichero */
    fgets(line, MAXCHAR, fp);

    pthread_t threads[H];
    struct ThreadArgs thread_args[H];

    // Crea los hilos secundarios
    for (int i = 0; i < H; i++) {
        thread_args[i].n_lines = lines_per_thread;
        thread_args[i].airports = airports;
        thread_args[i].num_flights = num_flights;
        thread_args[i].file = fp;

        pthread_create(&threads[i], NULL, process_data_block, (void *)&thread_args[i]);
    }

    // Espera a que los hilos secundarios terminen
    for (int i = 0; i < H; i++) {
        pthread_join(threads[i], NULL);
    }

    fclose(fp);
}


int main(int argc, char **argv) {

    if (argc != 5) {
        printf("%s <airport.csv> <flights.csv> <N_Hilos> <N_Lines> \n", argv[0]);
        exit(1);
    }

    char **airports;
    int **num_flights;
    int H = atoi(argv[3]);  // Número de hilos
    int lines_per_thread = atoi(argv[4]); 

    airports = (char **)malloc_matrix(NUM_AIRPORTS, STR_CODE_AIRPORT, sizeof(char));
    num_flights = (int **)malloc_matrix(NUM_AIRPORTS, NUM_AIRPORTS, sizeof(int));

    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);


    // Lee los codigos de los aeropuertos 
    read_airports(airports, argv[1]);

    // Lee los datos de los vuelos
    read_airports_data(num_flights, airports, argv[2], H, lines_per_thread);
    
    // Imprime un resumen de la tabla
    print_num_flights_summary(num_flights, airports);

    // Libera espacio
    free_matrix((void **) airports, NUM_AIRPORTS);
    free_matrix((void **) num_flights, NUM_AIRPORTS);

    // Tiempo cronologico 
    gettimeofday(&tv2, NULL);

    // Tiempo para la creacion del arbol 
    printf("Tiempo para procesar el fichero: %f segundos\n",
      (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
      (double) (tv2.tv_sec - tv1.tv_sec));

    printf("Hilos : %d N Lineas :  %d  \n\n", H, lines_per_thread);

    return 0;
}
