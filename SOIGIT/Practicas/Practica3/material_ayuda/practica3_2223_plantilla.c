#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Operaciones con Procesos
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define MAX_LEN 100

typedef struct {
  int id;
  char nom[MAX_LEN];
} opcio;

typedef struct {
  int id1;
  int id2;
  char descripcion[MAX_LEN];
} reglas;

int leer_opciones(const char *filename, opcio *array, int max_size);
int leer_reglas(const char *filename, reglas *array, int max_size);



//Valor que sirve para saber cuando de ha llamado al SIGNUSER1
int sigusr1 = 0;
int hijo1, hijo2;


void siguser1(){
      sigusr1 =1;
}

void logicaProductor(int N, int * fd, int maxSeleccio){

    /* Random seed */
    //srand(time(NULL));
    srand(getpid());

    int turno=1;
    //Empezamos los turnos de cada productor
    while(turno != (N+1) ){

      while (!sigusr1) {}; //Esperando al Consumidor
      sigusr1 = 0;

      int value =  (rand() % maxSeleccio); //Generamos una opción para el jugador
      write(fd[1], &value, sizeof(value)); //La escribimos en la tubería

      kill(getppid(),SIGUSR1); //Envia la señal al consumidor para continuar...

      turno++; //Siguiente turno
  }
}

  void logicaConsumidor(int N, opcio * o_array, reglas *r_array, int * fd){

    int turno=1, wins1=0, wins2=0, empate=0;

    while(turno != (N+1)){  //Revisamos todos los turnos

      
        int seleccionJ1=0, seleccionJ2=0; //Respuestas de los productores

        kill(hijo1, SIGUSR1); //Avisamos al primer productor que debe iniciar su elección

        while(!sigusr1){}; //Esperamos su opción...
        sigusr1=0;

        /* Leer el resultado */
        read(fd[0], &seleccionJ1, sizeof(seleccionJ1));
        printf("El Jugador 1 ha elegido %s\n", o_array[seleccionJ1].nom);

        //Después de leer el resultado del primer productor enviamos la señal al segundo productor
        kill(hijo2, SIGUSR1);

        while(!sigusr1){};//Esperamos la respuesta...
        sigusr1=0;

        /* Leemos la elección del segundo productor*/
        read(fd[0], &seleccionJ2, sizeof(seleccionJ2));
        printf("El Jugador 2 ha elegido %s\n", o_array[seleccionJ2].nom);

        int gana1 =0; //Con este valor podremos saber si gana el jugador 1 o el jugador 2

        //Si no es un empate...
        if(seleccionJ1!=seleccionJ2){

          for (int i=0; i < 3; i++){
            //Casos en que el primer jugador haya ganado
              if(r_array[i].id1 == seleccionJ1 && r_array[i].id2 == seleccionJ2){
                  wins1+=1; gana1=1;
              }
              //Casos en el que el segundo jugador haya ganado
              if(r_array[i].id2 == seleccionJ1 && r_array[i].id1 == seleccionJ2){
                  wins2+=1;
              }
          } 

            if(gana1==1){
                printf("Este turno lo ha ganado el Jugador 1!...\n");
                printf("Solución del Encuentro: %s\n\n", r_array[seleccionJ1].descripcion);
            }else{
                printf("Este turno lo ha ganado el Jugador 2!...\n");
                printf("Solución del Encuentro: %s\n\n", r_array[seleccionJ2].descripcion);
            }
        }else{
          empate++;
          printf("Solución del Encuentro: Ha sido un empate\n\n");
        }
          turno++;
    }

    //Al final de los turnos sólo queda comprobar quién ha ganado...

    printf("Wins P1: %d, Wins P2: %d, Empates: %d\n", wins1, wins2, empate);
    if(wins1> wins2)
      printf("El ganador del Juego es el Jugador 1 !! \n");
    else if( wins2 > wins1)
      printf("El ganador del Juego es el Jugador 2 !! \n");
    else
      printf("Suerte para la próxima empatados... \n");

}


int main(int argc, char *argv[])
{
  char *fichero_opciones, *fichero_reglas;
  int N, num_opciones;//, num_reglas;
  opcio o_array[100];
  reglas r_array[100];

      // Leer parametros
  if (argc != 4) {
    printf("%s <fichero_opciones> <fichero_reglas> <N>\n", argv[0]);
    exit(1);
  }

  fichero_opciones = argv[1];
  fichero_reglas = argv[2];
  N = atoi(argv[3]);

  if (N < 1) {
    printf("N no puede ser cero o negativo!\n");
    exit(1);
  }

  // Leer fichero de opciones
  num_opciones = leer_opciones(fichero_opciones, o_array, 100);
  if(num_opciones == -1){
    printf("Error leyendo el fichero de opciones!\n");
    exit(1);
  }
      
  // Leer fichero de reglas
  if(leer_reglas(fichero_reglas, r_array, 100) == -1){
    printf("Error leyendo el fichero de reglas!\n");
    exit(1);
  }
  int fd[2];  
  
  pipe(fd); //Creamos la tubería 

  hijo1 = fork(); //Creamos al primer consumidor

  
  if (hijo1 != 0) //Si somos el padre creamos al segundo consumidor
      hijo2 = fork();


  signal(SIGUSR1, siguser1); //Declaramos el método para la señal SIGUSR1
      
  if(hijo1==0){ //Cas Fills
        logicaProductor(N, fd, num_opciones);   
  }else if( hijo2 == 0){
        logicaProductor(N, fd, num_opciones);
  }else{ //Cas de Pare
      sleep(0.5);
      logicaConsumidor(N, o_array,r_array, fd); 
  }
    
  return 0;
}




int leer_opciones(const char *filename, opcio *array, int max_size) {
    FILE *fp;
    char line[MAX_LEN];
    char *token;
    int count = 0;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        return -1;
    }

    while (fgets(line, MAX_LEN, fp) != NULL && count < max_size) {
      token = strtok(line, ",");
      array[count].id = atoi(token);
      token = strtok(NULL, ",");
      token[strcspn(token, "\r\n")] = 0;  // remove newline character
      strncpy(array[count].nom, token, MAX_LEN - 1);
      array[count].nom[MAX_LEN - 1] = '\0'; // Ensure null-termination
      count++;
    }

    fclose(fp);
    return count;
}

int leer_reglas(const char *filename, reglas *array, int max_size)
{
    FILE *fp;
    char line[MAX_LEN];
    char *token;
    int count = 0;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        return -1;
    }

    while (fgets(line, MAX_LEN, fp) != NULL && count < max_size) {
        token = strtok(line, ",");
        array[count].id1 = atoi(token);
        token = strtok(NULL, ",");
        array[count].id2 = atoi(token);
        token = strtok(NULL, ",");
        token[strcspn(token, "\r\n")] = 0;  // remove newline character
        strncpy(array[count].descripcion, token, MAX_LEN - 1);
        array[count].descripcion[MAX_LEN - 1] = '\0'; // Ensure null-termination
        count++;
    }

    fclose(fp);
    return count;
}

