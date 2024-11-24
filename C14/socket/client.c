#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include "sock.h"

// Lista de libros
const char* books[] = {"War and Peace",
		       "Pride and Prejudice",
		       "The Sound and the Fury"};

// Función para reportar errores
void report(const char* msg, int terminate) {
  perror(msg); // Imprime el mensaje de error
  if (terminate) exit(-1); /* falla y termina el programa */
}

int main() {
  /* fd para el socket */
  int sockfd = socket(AF_INET,      /* versus AF_LOCAL */
		      SOCK_STREAM,  /* confiable, bidireccional */
		      0);           /* el sistema elige el protocolo (TCP) */
  if (sockfd < 0) report("socket", 1); /* termina si hay error */

  /* obtener la dirección del host */
  struct hostent* hptr = gethostbyname(Host); /* localhost: 127.0.0.1 */ 
  if (!hptr) report("gethostbyname", 1); /* ¿es hptr NULL? */
  if (hptr->h_addrtype != AF_INET)       /* versus AF_LOCAL */
    report("bad address family", 1);
  
  /* conectar al servidor: configurar la dirección del servidor primero */
  struct sockaddr_in saddr;
  memset(&saddr, 0, sizeof(saddr)); // Limpiar la estructura
  saddr.sin_family = AF_INET; // Familia de direcciones
  saddr.sin_addr.s_addr = 
     ((struct in_addr*) hptr->h_addr_list[0])->s_addr; // Dirección IP del host
  saddr.sin_port = htons(PortNumber); /* número de puerto en big-endian */
  
  if (connect(sockfd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0)
    report("connect", 1); // Termina si hay error
  
  /* Escribir algunos datos y leer los ecos. */
  puts("Conectado al servidor, a punto de escribir algunos datos...");
  int i;
  for (i = 0; i < ConversationLen; i++) {
    if (write(sockfd, books[i], strlen(books[i])) > 0) {
      /* obtener confirmación del eco del servidor y imprimir */
      char buffer[BuffSize + 1];
      memset(buffer, '\0', sizeof(buffer)); // Limpiar el buffer
      if (read(sockfd, buffer, sizeof(buffer)) > 0)
	puts(buffer); // Imprimir el eco recibido
    }
  }
  puts("Cliente terminado, a punto de salir...");
  close(sockfd); /* cerrar la conexión */
  return 0;
}
