#include <stdio.h>
#include <pthread.h>

#define MAX 50 // Tamaño máximo del buffer

int buffer[MAX]; // Buffer compartido
int fill = 0;    // Índice para producir
int use = 0;     // Índice para consumir
int count = 0;   // Elementos en el buffer

// Variables de sincronización
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

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
  int loops = (int) arg; // Número de iteraciones
  for (i = 0; i < loops; i++) {
    pthread_mutex_lock(&lock);
    // Espera si el buffer está lleno
    if (count >= MAX)
      pthread_cond_wait(&cond, &lock);
    put(i); // Produce un valor
    pthread_cond_signal(&cond); // Notifica al consumidor
    pthread_mutex_unlock(&lock);
  }
  return NULL;
}

// Función del hilo consumidor
void *consumer(void *arg) {
  int i;
  int loops = (int) arg; // Número de iteraciones
  for (i = 0; i < loops; i++) {
    pthread_mutex_lock(&lock);
    // Espera si el buffer está vacío
    if (count == 0)
      pthread_cond_wait(&cond, &lock);
    int tmp = get(); // Consume un valor
    pthread_cond_signal(&cond); // Notifica al productor
    printf("%d\n", tmp); // Imprime el valor consumido
    pthread_mutex_unlock(&lock);
  }
  return NULL;
}

int main(void) {
  pthread_t p0, p1, p2, p3;
  // Crea dos productores y dos consumidores
  pthread_create(&p0, NULL, producer, (void*)(MAX*2));
  pthread_create(&p1, NULL, producer, (void*)MAX);
  pthread_create(&p2, NULL, consumer, (void*)MAX);
  pthread_create(&p3, NULL, consumer, (void*)MAX);

  // Espera a que los hilos terminen
  pthread_join(p0, NULL);
  pthread_join(p1, NULL);
  pthread_join(p2, NULL);
  pthread_join(p3, NULL);
  
  printf("listo!\n");
  return 0;
}