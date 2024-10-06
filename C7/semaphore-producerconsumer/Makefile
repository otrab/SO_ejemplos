# Objetivo predeterminado: compila el ejecutable 'main'
all: main

# Variable que define el compilador a usar
CC = clang

# Bandera de compilación que incluye la información de depuración,
# desactiva todas las advertencias, añade soporte para hilos POSIX,
# y enlaza la biblioteca matemática.
override CFLAGS += -g -Wno-everything -pthread -lm

# Encuentra todos los archivos .c en el directorio actual y subdirectorios,
# excluyendo los directorios .ccls-cache, y los asigna a SRCS.
SRCS = $(shell find . -name '.ccls-cache' -type d -prune -o -type f -name '*.c' -print)

# Regla para compilar el ejecutable 'main' usando los archivos fuente en SRCS.
main: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o "$@"

# Regla para compilar el ejecutable 'main-debug' con optimización desactivada (-O0),
# útil para depuración.
main-debug: $(SRCS)
	$(CC) $(CFLAGS) -O0 $(SRCS) -o "$@"

# Objetivo 'clean' para eliminar los ejecutables generados 'main' y 'main-debug'.
clean:
	rm -f main main-debug
