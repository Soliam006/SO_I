#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#define MAXCHAR 500
#define LEN_CODE_AIRPORT 3
#define STR_CODE_AIRPORT (LEN_CODE_AIRPORT + 1)
#define NUM_AIRPORTS 303
#define COL_ORIGIN_AIRPORT 17
#define COL_DESTINATION_AIRPORT 18
#define B 5 // Tamaño del búfer (número de bloques)
#define N 100  // Número de líneas por bloque


int extract_fields_airport(char *origin, char *destination, char *line) 
{
  /*Recorre la linea por caracteres*/
  char caracter;
  int i, iterator, coma_count;
  int start, end, len;
  int invalid = 0;
  int found = 0;
  char eow = '\0';
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
      end = i;
      if (coma_count ==  COL_ORIGIN_AIRPORT || coma_count == COL_DESTINATION_AIRPORT) {
        len = end - start;

        if (len > 1) {

          if (len > STR_CODE_AIRPORT) {
            printf("ERROR len code airport\n");
            exit(1);
          }

          for(iterator = start; iterator < end-1; iterator ++){
            word[iterator-start] = line[iterator];
          }
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
  char line[MAXCHAR], eow = '\0';
  FILE *fp;

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

// Estructura para el buffer de comunicación
struct CommunicationBuffer {
    char ***lines;
    int in;  // Índice de inserción
    int out;  // Índice de extracción
    int count;  // Contador de bloques en el buffer
    int producer_finished;
};

// Variables globales
struct CommunicationBuffer buffer;
pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t bufferFullCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t bufferEmptyCond = PTHREAD_COND_INITIALIZER;

// Estructura para pasar argumentos a los hilos secundarios
struct ThreadArgs {
    char **airports;
    int **num_flights;
    FILE *file;
};

// Función para el consumidor (hilo secundario)
void *process_data_block(void *arg) {
    struct ThreadArgs *thread_args = (struct ThreadArgs *)arg;

    // ... Variables locales ...
    char origin[STR_CODE_AIRPORT], destination[STR_CODE_AIRPORT];
    int invalid, index_origin, index_destination;

    while (!buffer.producer_finished) {
        //Si no hay bloques que procesar, el hilo se duerme en la
        //condición de Buffer Vacío
        while (buffer.count == 0) {
            //El hilo se duerme y ya no continúa
            pthread_cond_wait(&bufferEmptyCond, &bufferMutex);

            //Si recibimos la señal, es porque el productor ha añadido un bloque,
            //volvemos al while, para así asegurarnos de que otro hilo no haya leído
            //ya el bloque el productor dijo. (Por ese motivo es un WHILE, no un IF )
        }

        // Verificar si el productor ha terminado y no hay más bloques para procesar
        if (buffer.count == 0 && buffer.producer_finished) {
            break;  // Salir del bucle
        }

        pthread_mutex_lock(&bufferMutex);
        // Tomar un bloque del búfer de comunicación
        int currentBlockIndex = buffer.out;
        pthread_mutex_unlock(&bufferMutex);

        // Procesar las N líneas del bloque
        for (int i = 0; i < N; i++) {
          pthread_mutex_lock(&bufferMutex);
          char* currentLine = buffer.lines[currentBlockIndex][i];
          pthread_mutex_unlock(&bufferMutex);
          //Ya no estamos en el buffer, soltamos la llave

            //Procesar la línea obtenida del BUFFER
            invalid = extract_fields_airport(origin, destination, currentLine);

            if (!invalid) {
                index_origin = get_index_airport(origin, thread_args->airports);
                index_destination = get_index_airport(destination, thread_args->airports);

                if ((index_origin >= 0) && (index_destination >= 0)) {
                    pthread_mutex_lock(&bufferMutex);
                    thread_args->num_flights[index_origin][index_destination]++;
                    pthread_mutex_unlock(&bufferMutex);
                }
            }
        }

        pthread_mutex_lock(&bufferMutex);
        buffer.out = (buffer.out + 1) % B;
        buffer.count--; //Reducir la variable global de block counts

        //Envíamos al productor una señal de que ya puede despertarse
        //porque se ha leído un bloque del buffer y ya puede llenarlo de nuevo
        pthread_cond_signal(&bufferFullCond);
        pthread_mutex_unlock(&bufferMutex);
    }

    pthread_exit(NULL);
}

// Función para el productor (hilo principal)
void *read_airports_data(int **num_flights, char **airports, char *fname, int H) {
    // Abre el archivo y lee la cabecera
    char line[MAXCHAR];
    char origin[STR_CODE_AIRPORT], destination[STR_CODE_AIRPORT];
    int invalid, index_origin, index_destination;

    FILE *fp = fopen(fname, "r");
    if (!fp) {
        printf("ERROR: could not open '%s'\n", fname);
        exit(1);
    }

    /* Leemos la cabecera del fichero */
    fgets(line, MAXCHAR, fp);

    pthread_t threads[H];
    struct ThreadArgs thread_args[H];

    // Crea los hilos
    for (int i = 0; i < H; i++) {
        thread_args[i].airports = airports;
        thread_args[i].num_flights = num_flights;
        thread_args[i].file = fp;
        // Crear hilos secundarios (consumidores)
        pthread_create(&threads[i], NULL, process_data_block, (void *)&thread_args[i]);
    }

    /* Leemos la cabecera del fichero */
    fgets(line, MAXCHAR, fp);

    while (fgets(line, MAXCHAR, fp) && !buffer.producer_finished) {
        while (buffer.count == B) {
            pthread_cond_wait(&bufferFullCond, &bufferMutex);
        }

        // Leer N líneas y colocarlas en el buffer
        for (int i = 0; i < N && line; i++) {
            // Colocar la línea en el buffer    
            pthread_mutex_lock(&bufferMutex);
            strcpy(buffer.lines[buffer.in][i], line);  // O utiliza strncpy según tus necesidades
            pthread_mutex_unlock(&bufferMutex);


            // Leer la siguiente línea
            if (fgets(line, MAXCHAR, fp) == NULL) {
                // Manejar el final del archivo si se alcanza antes de leer N líneas completas
                buffer.producer_finished = 1;
                break;
            }
        }

        buffer.in = (buffer.in + 1) % B;
        buffer.count++; //Incrementar la variable global de block counts

        pthread_cond_signal(&bufferEmptyCond);  // Signal para el consumidor
    }

    // Espera a que los hilos terminen
    for (int i = 0; i < H; i++) {
        pthread_join(threads[i], NULL);
    }

    fclose(fp);
}



int main(int argc, char **argv) {

    if (argc != 4) {
        printf("%s <airport.csv> <flights.csv> <N_Hilos>\n", argv[0]);
        exit(1);
    }

    char *fname = argv[2], **airports;
    int **num_flights;
    int H = 2;//atoi(argv[3]);  // Número de hilos

        // Inicialización del buffer
    buffer.lines = (char***)malloc(B * sizeof(char**));
    for (int i = 0; i < B; ++i) {
        buffer.lines[i] = (char**)malloc(N * sizeof(char*));
        for (int j = 0; j < N; ++j) {
            buffer.lines[i][j] = (char*)malloc(MAXCHAR * sizeof(char));
        }
    }


    FILE *fp;
    fp = fopen(fname, "r");
    if (!fp) {
        printf("ERROR: could not open '%s'\n", fname);
        exit(1);
    }

    airports = (char **)malloc_matrix(NUM_AIRPORTS, STR_CODE_AIRPORT, sizeof(char));
    num_flights = (int **)malloc_matrix(NUM_AIRPORTS, NUM_AIRPORTS, sizeof(int));

    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);

    // Lee los codigos de los aeropuertos 
    read_airports(airports, argv[1]);


    pthread_t product, threads[H];
    struct ThreadArgs thread_args[H];


    pthread_cond_init(&bufferEmptyCond, NULL);
    pthread_cond_init(&bufferFullCond, NULL);

    // Lee los datos de los vuelos
    read_airports_data(num_flights, airports, argv[2], H);

    // Imprime un resumen de la tabla
    print_num_flights_summary(num_flights, airports);

    // Libera espacio
    free_matrix((void **) airports, NUM_AIRPORTS);
    free_matrix((void **) num_flights, NUM_AIRPORTS);


    // Liberar recursos
    pthread_mutex_destroy(&bufferMutex);
    pthread_cond_destroy(&bufferFullCond);
    pthread_cond_destroy(&bufferEmptyCond);
    
    // Liberar memoria al final del programa
    for (int i = 0; i < B; ++i) {
        for (int j = 0; j < N; ++j) {
            free(buffer.lines[i][j]);
        }
        free(buffer.lines[i]);
    }
    free(buffer.lines);


    // Tiempo cronologico 
    gettimeofday(&tv2, NULL);

    // Tiempo para la creacion del arbol 
    printf("Tiempo para procesar el fichero: %f segundos\n",
      (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
      (double) (tv2.tv_sec - tv1.tv_sec));
      

    return 0;
}
