#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX 50 // Tamaño del buffer

int buffer[MAX]; // Buffer compartido
int fill = 0;    // Índice para el productor
int use = 0;     // Índice para el consumidor
int count = 0;   // Cantidad de elementos en el buffer

sem_t data, empty; // Semáforos para sincronización

// Inserta un valor en el buffer
void put(int value) {
  buffer[fill] = value;
  fill = (fill + 1) % MAX;
  count++;
}

// Obtiene un valor del buffer
int get() {
  int tmp = buffer[use];
  use = (use + 1) % MAX;
  count--;
  return tmp;
}

// Función del hilo productor
void *producer(void *arg) {
  int i;
  int loops = (int) arg; // Número de elementos a producir
  for (i = 0; i < loops; i++) {
    sem_wait(&empty); // Espera si el buffer está lleno
    put(i);           // Inserta elemento en el buffer
    sem_post(&data);  // Señala que hay datos disponibles
  }
  return NULL;
}

// Función del hilo consumidor
void *consumer(void *arg) {
  int i;
  int loops = (int) arg; // Número de elementos a consumir
  for (i = 0; i < loops; i++) {
    sem_wait(&data);     // Espera si no hay datos disponibles
    int tmp = get();     // Obtiene elemento del buffer
    printf("%d\n", tmp); // Imprime el valor consumido
    sem_post(&empty);    // Señala que hay espacio disponible
  }
  return NULL;
}

int main(void) {
  // Inicializa los semáforos
  sem_init(&data, 0, 0);    // Semáforo de datos disponibles (inicia en 0)
  sem_init(&empty, 0, MAX); // Semáforo de espacios vacíos (inicia en MAX)

  pthread_t p0, p1, p2, p3;
  // Crea hilos productores y consumidores
  pthread_create(&p0, NULL, producer, (void*)(MAX*2)); // Produce 100 elementos
  pthread_create(&p1, NULL, producer, (void*)MAX);     // Produce 50 elementos
  pthread_create(&p2, NULL, consumer, (void*)MAX);     // Consume 50 elementos
  pthread_create(&p3, NULL, consumer, (void*)MAX);     // Consume 50 elementos

  // Espera a que los hilos terminen
  pthread_join(p0, NULL);
  pthread_join(p1, NULL);
  pthread_join(p2, NULL);
  pthread_join(p3, NULL);
  
  printf("listo!\n"); // Mensaje final

  return 0;
}