#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX 50 // Tamaño máximo del buffer

int buffer[MAX]; // Buffer compartido
int fill = 0;    // Índice para el productor
int use = 0;     // Índice para el consumidor
int count = 0;   // Cantidad de elementos en el buffer

sem_t data, empty, mutex; // Semáforos para sincronización

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
    sem_wait(&mutex); // Entra a la sección crítica
    sem_wait(&empty); // Espera espacio disponible en el buffer
    put(i);           // Inserta elemento
    sem_post(&data);  // Señala que hay datos disponibles
    sem_post(&mutex); // Sale de la sección crítica
  }
  return NULL;
}

// Función del hilo consumidor
void *consumer(void *arg) {
  int i;
  int loops = (int) arg; // Número de elementos a consumir
  for (i = 0; i < loops; i++) {
    sem_wait(&mutex); // Entra a la sección crítica
    sem_wait(&data);  // Espera datos disponibles en el buffer
    int tmp = get();  // Obtiene elemento
    printf("%d\n", tmp); // Imprime el elemento consumido
    sem_post(&empty); // Señala que hay espacio disponible
    sem_post(&mutex); // Sale de la sección crítica
  }
  return NULL;
}

int main(void) {
  // Inicializa los semáforos
  sem_init(&data, 0, 0);   // No hay datos disponibles al inicio
  sem_init(&empty, 0, MAX); // El buffer está vacío al inicio
  sem_init(&mutex, 0, 1);   // Mutex para la sección crítica

  pthread_t p0, p1, p2, p3;
  // Crea hilos productores y consumidores
  pthread_create(&p0, NULL, producer, (void*)(MAX*2));
  pthread_create(&p1, NULL, producer, (void*)MAX);
  pthread_create(&p2, NULL, consumer, (void*)MAX);
  pthread_create(&p3, NULL, consumer, (void*)MAX);

  // Espera a que los hilos terminen
  pthread_join(p0, NULL);
  pthread_join(p1, NULL);
  pthread_join(p2, NULL);
  pthread_join(p3, NULL);
  
  printf("listo!\n"); // Mensaje final

  return 0;
}