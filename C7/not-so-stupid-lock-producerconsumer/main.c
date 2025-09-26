#include <stdio.h>
#include <pthread.h>

#define MAX 50 // Tamaño máximo del buffer

int buffer[MAX];      // Buffer circular compartido
int fill = 0;         // Índice donde el productor coloca el siguiente elemento
int use = 0;          // Índice donde el consumidor toma el siguiente elemento
int count = 0;        // Número de elementos actualmente en el buffer
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // Mutex para sincronización
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;   // Variable de condición para sincronización

// Función para colocar un valor en el buffer
void put(int value) {
  buffer[fill] = value;
  fill = (fill + 1) % MAX; // Avanza el índice circularmente
  count++;                 // Incrementa el contador de elementos
}

// Función para obtener un valor del buffer
int get() {
  int tmp = buffer[use];
  use = (use + 1) % MAX; // Avanza el índice circularmente
  count--;               // Decrementa el contador de elementos
  return tmp;
}

// Función del hilo productor
void *producer(void *arg) {
  int i;
  int loops = (int) arg; // Número de elementos a producir
  for (i = 0; i < loops; i++) {
    pthread_mutex_lock(&lock); // Bloquea el mutex
    while (count >= MAX)       // Espera si el buffer está lleno
      pthread_cond_wait(&cond, &lock);
    put(i);                    // Coloca el elemento en el buffer
    pthread_cond_signal(&cond); // Señala a los consumidores
    pthread_mutex_unlock(&lock); // Libera el mutex
  }
  return NULL;
}

// Función del hilo consumidor
void *consumer(void *arg) {
  int i;
  int loops = (int) arg; // Número de elementos a consumir
  for (i = 0; i < loops; i++) {
    pthread_mutex_lock(&lock); // Bloquea el mutex
    while (count == 0)         // Espera si el buffer está vacío
      pthread_cond_wait(&cond, &lock);
    int tmp = get();           // Obtiene el elemento del buffer
    pthread_cond_signal(&cond); // Señala a los productores
    printf("%d\n", tmp);       // Imprime el valor consumido
    pthread_mutex_unlock(&lock); // Libera el mutex
  }
  return NULL;
}

int main(void) {
  pthread_t p0, p1, p2, p3;
  // Crea dos productores y dos consumidores
  pthread_create(&p0, NULL, producer, (void*)(MAX*2)); // Productor produce MAX*2 elementos
  pthread_create(&p1, NULL, producer, (void*)MAX);     // Productor produce MAX elementos
  pthread_create(&p2, NULL, consumer, (void*)MAX);     // Consumidor consume MAX elementos
  pthread_create(&p3, NULL, consumer, (void*)MAX);     // Consumidor consume MAX elementos

  // Espera a que los hilos terminen
  pthread_join(p0, NULL);
  pthread_join(p1, NULL);
  pthread_join(p2, NULL);
  pthread_join(p3, NULL);
  
  printf("listo!\n"); // Mensaje final
  return 0;
}