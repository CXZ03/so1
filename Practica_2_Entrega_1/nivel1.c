/* Practica 2: Nivel 1 */
/* Autores: Guillem, Elena, Xiaozhe */

/* Declaración de debugs */
#define DEBUG_1 1
#define _POSIX_C_SOURCE 200112L

/* Declaración de librerias */
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define RESET_FORMATO "\033[0m"
#define GRIS "\x1b[94m"
#define AZUL_T "\x1b[34m"
#define MAGENTA_T "\x1b[35m"

/* Declaración de constantes */

#define COMMAND_LINE_SIZE 1024  // tamaño máximo de linea de commando
#define ARGS_SIZE 64            // tamaño máximo de argumentos
#define PROMPT '$'              // simbolo del prompt

/* Declaración de Funciones */
char *read_line(char *line);
int execute_line(char *line);
int check_internal(char **args);
int internal_cd(char **args);
int internal_export(char **args);
int internal_source(char **args);
int internal_jobs();
int internal_fg(char **args);
int internal_bg(char **args);

/* Declaración de funciones adicionales */
void imprimir_prompt();

/* Declaración de variables */
char line[COMMAND_LINE_SIZE];  // almacena la línea de comando
char *args[ARGS_SIZE];         // almacena los argumentos disponibles

int check_internal(char **args) {
    if (!strcmp(args[0], "cd")) return internal_cd(args);
    if (!strcmp(args[0], "export")) return internal_export(args);
    if (!strcmp(args[0], "source")) return internal_source(args);
    if (!strcmp(args[0], "jobs")) return internal_jobs(args);
    if (!strcmp(args[0], "bg")) return internal_bg(args);
    if (!strcmp(args[0], "fg")) return internal_fg(args);
    if (!strcmp(args[0], "exit")) exit(0);
    return 0;  // no es un comando interno
}

int internal_cd(char **args) {
    printf("[internal_cd()→ Esta función cambiará de directorio]\n");

    return 1;
}

int internal_export(char **args) {
    printf(
        "[internal_export()→Esta función asignará valores a variablescd de "
        "entorno]\n");

    return 1;
}

int internal_source(char **args) {
    printf(
        "[internal_source()→Esta función ejecutará un fichero de líneas de "
        "comandos]\n");
    return 1;
}

int internal_jobs(char **args) {
#if DEBUG_1
    printf(
        "[internal_jobs()→ Esta función mostrará el PID de los procesos que no "
        "estén en foreground]\n");
#endif
    return 1;
}

int internal_fg(char **args) {
#if DEBUG_1
    printf(
        "[internal_fg()→ Esta función enviará un trabajo detenido al "
        "foreground reactivando su ejecución, o uno del background al "
        "foreground ]\n");
#endif
    return 1;
}

int internal_bg(char **args) {
#if DEBUG_1
    printf(
        "[internal_bg()→ Esta función reactivará un proceso detenido para que "
        "siga ejecutándose pero en segundo plano]\n");
#endif
    return 1;
}

/**
 * Función: read_line()
 * --------------------
 * Descripción:
 * Lee la linea de comando que ha introducido el usuario por stdin, y cambia
 * el carácter '\n' por '\0'. También imprime el prompt de la consola.
 *
 * con strchr buscamos la primera ocurriencia del salto de linea en line
 * Argumentos:
 *      - line: puntero de char donde guardaremos la línea leida.
 *
 * Salida:
 *      - char *: puntero de char que apunta a la línea leida (line).
 */

char *read_line(char *line) {
    imprimir_prompt();
    char *ptr = fgets(line, COMMAND_LINE_SIZE, stdin);
    if (ptr) {
        char *pos = strchr(line, 10);
        if (pos != NULL) {
            *pos = '\0';
        }
    } else {
        printf("\r");
        if (feof(stdin)) {
            // Ctrl+D
            fprintf(stderr, "Bye bye\n");
            exit(0);
        }
    }
    return ptr;
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

void imprimir_prompt() {
    /* Vaciado del buffer */
    sleep(1);  // Dormir un segundo
    fflush(stdout);
    printf(MAGENTA_T);
    /* imprimimos el prompt personalizado */
    printf("%s:~%s%s%c ", getenv("USER"), getenv("HOME"), getenv("PWD"),
           PROMPT);

    printf(RESET_FORMATO);
}

int parse_args(char **args, char *line) {
    int i = 0;
    args[i] = strtok(line, " \t\n\r");

#if DEBUG_1
    fprintf(stderr, AZUL_T "[parse_args()→ token %i: %s]\n" RESET_FORMATO, i,
            args[i]);
#endif
    while (args[i] && args[i][0] != '#') {
        i++;
        args[i] = strtok(NULL, " \t\n\r");

#if DEBUG_1
        fprintf(stderr, AZUL_T "[parse_args()→ token %i: %s]\n" RESET_FORMATO,
                i, args[i]);
#endif
    }
    if (args[i]) {
        // en caso de que el ultimo token sea simbolo comentario
        args[i] = NULL;
#if DEBUG_1
        fprintf(stderr,
                AZUL_T "[parse_args()→ token %i corregido: %s]\n" RESET_FORMATO,
                i, args[i]);
#endif
    }
    return i;
}

int execute_line(char *line) {
    char *args[ARGS_SIZE];
    pid_t pid, status;
    char command_line[COMMAND_LINE_SIZE];

    // copiamos comando sin '\n'
    memset(command_line, '\0', sizeof(command_line));
    strcpy(command_line, line);

    if (parse_args(args, line) > 0) {
        if (check_internal(args)) {
            return 1;
        }
#if DEBUGN_1
        fprintf(stderr, GRIS "[execute_line()→ PID padre: %d]\n" RESET_FORMATO,
                getpid());
#endif
    }
    return 0;
}
int main(int argC, char *argV[]) {
    // Guardamos nombre programa
    char line[COMMAND_LINE_SIZE];
    memset(line, 0, COMMAND_LINE_SIZE);
    while (1) {
        if (read_line(line)) {  // !=NULL
            execute_line(line);
        }
    }
    return 0;
}