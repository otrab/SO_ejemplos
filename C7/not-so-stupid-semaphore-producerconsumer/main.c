#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX 50

int buffer[MAX];
int fill = 0;
int use = 0;
int count = 0;

sem_t data, empty, mutex;

void put(int value) {
  buffer[fill] = value;
  fill = (fill + 1) % MAX;
  count++;
}

int get() {
  int tmp = buffer[use];
  use = (use + 1) % MAX;
  count--;
  return tmp;
}

void *producer(void *arg) {
  int i;
  int loops = (int) arg;
  for (i = 0; i < loops; i++) {
    sem_wait(&mutex);
    sem_wait(&empty);
    put(i);
    sem_post(&data);
    sem_post(&mutex);
  }
  return NULL;
}

void *consumer(void *arg) {
  int i;
  int loops = (int) arg;
  for (i = 0; i < loops; i++) {
    sem_wait(&mutex);
    sem_wait(&data);
    int tmp = get();
    printf("%d\n", tmp);
    sem_post(&empty);
    sem_post(&mutex);
  }
  return NULL;
}

int main(void) {
  sem_init(&data, 0, 0);
  sem_init(&empty, 0, MAX);
  sem_init(&mutex, 0, 1);

  pthread_t p0, p1, p2, p3;
  pthread_create(&p0, NULL, producer, (void*)(MAX*2));
  pthread_create(&p1, NULL, producer, (void*)MAX);
  pthread_create(&p2, NULL, consumer, (void*)MAX);
  pthread_create(&p3, NULL, consumer, (void*)MAX);

  pthread_join(p0, NULL);
  pthread_join(p1, NULL);
  pthread_join(p2, NULL);
  pthread_join(p3, NULL);
  
  printf("listo!\n");

  return 0;
}