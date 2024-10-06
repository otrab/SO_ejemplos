#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

sem_t s;

void* child(void* arg) {
  printf("child\n");
  sem_post(&s); // signal here: child is done
  return NULL;
}

int main(void) {
  sem_init(&s, 0, 1);
  printf("parent: begin\n");
  pthread_t c;
  pthread_create(&c, NULL, child, NULL);
  sem_wait(&s); // wait here for child
  printf("parent: end\n");
  return 0;
}