#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Función envoltorio para pthread_create: maneja errores y simplifica el código principal
void Pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                    void *(*start_routine) (void *), void *arg) {
    int rc = pthread_create(thread, attr, start_routine, arg);
    if (rc != 0) {
        fprintf(stderr, "pthread_create falló\n");
        exit(EXIT_FAILURE);
    }
}

// Declaración global del semáforo, compartido entre el hilo padre e hijo
sem_t s;

// Función que ejecuta el hilo hijo
void *child(void *arg) {
    printf("child\n");      // Mensaje del hijo
    sem_post(&s);           // Señaliza al padre que terminó
    return NULL;
}

int main(int argc, char *argv[]) {
    // Inicializa el semáforo en 0: el padre se bloqueará en sem_wait hasta que el hijo haga sem_post
    sem_init(&s, 0, 0); 
    
    printf("parent: begin\n"); // Mensaje inicial del padre
    
    pthread_t c;
    // Crea el hilo hijo usando la función envoltorio
    Pthread_create(&c, NULL, child, NULL);
    
    // El padre espera aquí hasta que el hijo termine y haga sem_post
    sem_wait(&s); 
    
    // Espera explícitamente a que el hilo hijo finalice
    pthread_join(c, NULL); 

    printf("parent: end\n"); // Mensaje final del padre
    
    // Libera recursos del semáforo
    sem_destroy(&s);
    
    return 0;
}