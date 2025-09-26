#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

sem_t s;

void* child(void* arg) {
  // Aseguramos que el mensaje del hijo se imprima primero
  printf("child\n");
  
  // El hijo envía la señal (abre la puerta)
  sem_post(&s); 
  return NULL;
}

int main(void) {
  // Clave de la ordenación: Iniciar a 0 (puerta cerrada)
  sem_init(&s, 0, 0); 
  
  printf("parent: begin\n");
  pthread_t c;
  pthread_create(&c, NULL, child, NULL);
  
  // El padre espera la señal del hijo (espera a que abran la puerta)
  sem_wait(&s); 
  
  // Esta línea solo se ejecutará después de que el hijo llame a sem_post()
  printf("parent: end\n"); 
  
  // Opcional: limpiar la hebra hija
  pthread_join(c, NULL); 
  
  // Limpiar el semáforo
  sem_destroy(&s);
  return 0;
}