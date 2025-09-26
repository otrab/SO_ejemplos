#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX 50 // Tamaño máximo del buffer

int buffer[MAX]; // Buffer compartido
int fill = 0;    // Índice para el productor
int use = 0;     // Índice para el consumidor
int count = 0;   // Número de elementos en el buffer

sem_t data, empty, mutex; // Semáforos para sincronización

// Función para agregar un valor al buffer
void put(int value) {
  buffer[fill] = value;
  fill = (fill + 1) % MAX;
  count++;
}

// Función para obtener un valor del buffer
int get() {
  int tmp = buffer[use];
  use = (use + 1) % MAX;
  count--;
  return tmp;
}

// Función del hilo productor
void *producer(void *arg) {
  int i;
  int loops = (int) arg; // Número de iteraciones
  for (i = 0; i < loops; i++) {
    sem_wait(&empty); // Espera si el buffer está lleno
    sem_wait(&mutex); // Entra en sección crítica
    put(i);           // Produce un elemento
    sem_post(&data);  // Señala que hay datos disponibles
    sem_post(&mutex); // Sale de sección crítica
  }
  return NULL;
}

// Función del hilo consumidor
void *consumer(void *arg) {
  int i;
  int loops = (int) arg; // Número de iteraciones
  for (i = 0; i < loops; i++) {
    sem_wait(&data);  // Espera si no hay datos
    sem_wait(&mutex); // Entra en sección crítica
    int tmp = get();  // Consume un elemento
    printf("%d\n", tmp); // Imprime el valor consumido
    sem_post(&empty); // Señala que hay espacio disponible
    sem_post(&mutex); // Sale de sección crítica
  }
  return NULL;
}

int main(void) {
  // Inicialización de semáforos
  sem_init(&data, 0, 0);   // No hay datos al inicio
  sem_init(&empty, 0, MAX); // Buffer vacío al inicio
  sem_init(&mutex, 0, 1);   // Mutex para sección crítica

  pthread_t p1, p2;
  // Crea hilo productor y consumidor
  pthread_create(&p1, NULL, producer, (void*)(MAX*2));
  pthread_create(&p2, NULL, consumer, (void*)MAX);

  // Espera a que los hilos terminen
  pthread_join(p1, NULL);
  pthread_join(p2, NULL);
  
  printf("listo!\n"); // Mensaje de finalización

  return 0;
}