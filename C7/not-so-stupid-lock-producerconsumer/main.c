#include <stdio.h>
#include <pthread.h>

#define MAX 50

int buffer[MAX];
int fill = 0;
int use = 0;
int count = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

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
    pthread_mutex_lock(&lock);
    while (count >= MAX)
      pthread_cond_wait(&cond, &lock);
    put(i);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);
  }
  return NULL;
}

void *consumer(void *arg) {
  int i;
  int loops = (int) arg;
  for (i = 0; i < loops; i++) {
    pthread_mutex_lock(&lock);
    while (count == 0)
      pthread_cond_wait(&cond, &lock);
    int tmp = get();
    pthread_cond_signal(&cond);
    printf("%d\n", tmp);
    pthread_mutex_unlock(&lock);
  }
  return NULL;
}

int main(void) {
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