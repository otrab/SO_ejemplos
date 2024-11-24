#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "sock.h"

// Función para reportar errores
void report(const char* msg, int terminate) {
  perror(msg); // Imprime el mensaje de error
  if (terminate) exit(-1); /* falla y termina el programa */
}

int main() {
  // Crear un socket
  int fd = socket(AF_INET,     /* red versus AF_LOCAL */
		  SOCK_STREAM, /* confiable, bidireccional: TCP */
		  0);          /* el sistema elige el protocolo subyacente */
  if (fd < 0) report("socket", 1); /* termina si hay error */
  	
  /* Vincular la dirección local del servidor en memoria */
  struct sockaddr_in saddr;
  memset(&saddr, 0, sizeof(saddr));          /* limpiar los bytes */
  saddr.sin_family = AF_INET;                /* versus AF_LOCAL */
  saddr.sin_addr.s_addr = htonl(INADDR_ANY); /* convertir de host a red */
  saddr.sin_port = htons(PortNumber);        /* para escuchar */
  
  if (bind(fd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0)
    report("bind", 1); /* termina si hay error */
	
  /* Escuchar en el socket */
  if (listen(fd, MaxConnects) < 0) /* escuchar clientes, hasta MaxConnects */
    report("listen", 1); /* termina si hay error */

  fprintf(stderr, "Escuchando en el puerto %i para clientes...\n", PortNumber);
  /* un servidor tradicionalmente escucha indefinidamente */
  while (1) {
    struct sockaddr_in caddr; /* dirección del cliente */
    socklen_t len = sizeof(caddr);
    
    int client_fd = accept(fd, (struct sockaddr*) &caddr, &len);  /* aceptar bloquea */
    if (client_fd < 0) {
      report("accept", 0); /* no termina, aunque haya un problema */
      continue;
    }

    /* leer del cliente */
    int i;
    for (i = 0; i < ConversationLen; i++) {
      char buffer[BuffSize + 1];
      memset(buffer, '\0', sizeof(buffer)); 
      int count = read(client_fd, buffer, sizeof(buffer));
      if (count > 0) {
	puts(buffer); // imprimir el mensaje recibido
	write(client_fd, buffer, sizeof(buffer)); /* eco como confirmación */
      }
    }
    close(client_fd); /* cerrar conexión */
  }  /* while(1) */
  return 0;
}
