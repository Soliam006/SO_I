CC = gcc

all: analisis analisis_hilos
	$(CC) -o analisis analisis.c
	$(CC) -lpthread -o analisis_hilos analisis_hilos.c
analisis: analisis.c
	$(CC) -o analisis analisis.c

analisis_hilos: analisis_hilos.c
	$(CC) -lpthread -o analisis_hilos analisis_hilos.c

clean:
	rm -f analisis analisis_hilos