/* Practica 2: Nivel 1 */
/* Autores: Guillem, Elena, Xiaozhe */

/* Declaración de debugs */
#define DEBUG_1 1

/* Declaración de librerias */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/* Declaración de constantes */
#define _POSIX_C_SOURCE 200112L
#define COMMAND_LINE_SIZE 1024  // tamaño máximo de linea de commando
#define ARGS_SIZE 64            // tamaño máximo de argumentos
#define PROMPT '$'              // simbolo del prompt

/* Declaración de Funciones */
char *read_line(char *line);
// int execute_line(char *line);
// int parse_args(char **args, char *line);
// int check_internal(char **args);
// int internal_cd(char **args);
// int internal_export(char **args);
// int internal_source(char **args);
// int internal_jobs();
// int internal_fg(char **args);
// int internal_bg(char **args);

/* Declaración de funciones adicionales */
int imprimir_prompt();

/* Declaración de variables */
char line[COMMAND_LINE_SIZE];  // almacena la línea de comando
char *args[ARGS_SIZE];         // almacena los argumentos disponibles

int main() {
    while (true) {
        if (read_line(line)) {
            // execute_line(line);
        }
    }
}

/**
 * Función: read_line()
 * --------------------
 * Descripción:
 * Lee la linea de comando que ha introducido el usuario por stdin, y cambia
 * el carácter '\n' por '\0'. También imprime el prompt de la consola.
 *
 * Argumentos:
 *      - line: puntero de char donde guardaremos la línea leida.
 *
 * Salida:
 *      - char *: puntero de char que apunta a la línea leida (line).
 */
char *read_line(char *line) {
    if (imprimir_prompt() == -1) {
        fprintf(stderr, "Error al imprimir el prompt\n");
    }
#if DEBUG_1
    printf("llamada a la función: fgets()\n");
#endif
    if (fgets(line, sizeof(line) - 1, stdin) != NULL) {  // leemos la linea
/* Sustituimos el carácter final '\n' por '\0' */
#if DEBUG_1
        printf("llamada a la función: strchr()\n");
#endif

        // /* intento 1 */
        // line[sizeof(line) - 1] = '\0';
        // char *pointerAux = strchr(line, (int)'\n');
        // *pointerAux = '\0';

        /* intento 2 */
        int i = 0;
        while (line[i] != '\n') {
            i++;
        }
        line[i] = '\0';

    } else {
        /* Caso que la linea leida sea NULL */
        if (feof(stdin)) {
            printf("\rGracias por usar el MiniShell, hasta la próxima. ;D\n");
            exit(0);
        } else {
            fprintf(stderr, "Error al leer la línea.\n");
        }
    }

    return line;
}

/**
 * Función: imprimir_prompt()
 * --------------------------
 * Imprime un String a la consola que contiene los siguientes variables de
 * entorno: USER, HOME y PWD.
 *
 * Salida:
 *      - int: si es 0 significa que ha hecho la función correctamente.
 */
int imprimir_prompt() {
#if DEBUG_1
    printf("llamada a la función: imprimir_prompt()\n");
#endif
    /* Vaciado del buffer */
    sleep(1);  // Dormir un segundo
    fflush(stdout);

    /* imprimimos el prompt personalizado */
    printf("%s:~%s%s%c ", getenv("USER"), getenv("HOME"), getenv("PWD"), PROMPT);

    return 0;
}
