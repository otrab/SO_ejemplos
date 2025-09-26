#include <stdio.h>
#include <pthread.h>

#define MAX 50 // Tamaño máximo del buffer

int buffer[MAX]; // Buffer circular compartido entre productores y consumidores
int fill = 0;    // Índice donde el productor coloca el siguiente elemento
int use = 0;     // Índice donde el consumidor toma el siguiente elemento
int count = 0;   // Número de elementos actualmente en el buffer

// Mutex para proteger el acceso concurrente al buffer
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
// Variable de condición para notificar cuando hay datos disponibles (consumidores esperan aquí)
pthread_cond_t data = PTHREAD_COND_INITIALIZER;
// Variable de condición para notificar cuando hay espacio disponible (productores esperan aquí)
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

// Inserta un valor en el buffer (solo llamado por el productor)
void put(int value) {
  buffer[fill] = value;
  fill = (fill + 1) % MAX; // Avanza el índice circularmente
  count++;                 // Incrementa el contador de elementos
}

// Obtiene un valor del buffer (solo llamado por el consumidor)
int get() {
  int tmp = buffer[use];
  use = (use + 1) % MAX; // Avanza el índice circularmente
  count--;               // Decrementa el contador de elementos
  return tmp;
}

// Función ejecutada por el hilo productor
void *producer(void *arg) {
  int i;
  int loops = (int) arg; // Número de elementos a producir
  for (i = 0; i < loops; i++) {
    pthread_mutex_lock(&lock); // Bloquea el acceso al buffer compartido
    // Si el buffer está lleno, espera hasta que haya espacio disponible
    while (count >= MAX)
      pthread_cond_wait(&empty, &lock);
    put(i); // Inserta el elemento en el buffer
    // Señala a los consumidores que hay datos disponibles
    pthread_cond_signal(&data);
    pthread_mutex_unlock(&lock); // Libera el acceso al buffer
  }
  return NULL;
}

// Función ejecutada por el hilo consumidor
void *consumer(void *arg) {
  int i;
  int loops = (int) arg; // Número de elementos a consumir
  for (i = 0; i < loops; i++) {
    pthread_mutex_lock(&lock); // Bloquea el acceso al buffer compartido
    // Si el buffer está vacío, espera hasta que haya datos disponibles
    while (count == 0)
      pthread_cond_wait(&data, &lock);
    int tmp = get(); // Obtiene el elemento del buffer
    // Señala a los productores que hay espacio disponible
    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&lock); // Libera el acceso al buffer
    printf("%d\n", tmp); // Imprime el elemento consumido
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

/*
Mejora anti-deadlock:
- Se usan **dos variables de condición**: una para productores (`empty`) y otra para consumidores (`data`).
- Esto permite que cada tipo de hilo espere solo por la condición relevante (productores esperan espacio, consumidores esperan datos).
- Al señalizar la condición correcta, se evita que los hilos despierten innecesariamente y se reduce el riesgo de deadlock.
- El mutex garantiza que solo un hilo acceda al buffer a la vez, evitando condiciones de carrera.
*/