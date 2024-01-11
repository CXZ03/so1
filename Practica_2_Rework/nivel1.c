/**
 * Practica 2: Semana 1
 * Autores: Guillem, Elena, Xiaozhe
 * Equipo: AguacateLovers
 * Grupo: 2, 202
 */

/* Librerias */
#define _POSIX_C_SOURCE 200112L  // Version del estándar C

#include <cstddef>

/* Constantes */
#define COMMAND_LINE_SIZE 1024  // tamaño máximo de linea de commando
#define ARGS_SIZE 64            // tamaño máximo de argumentos
#define PROMPT '$'              // simbolo del prompt

#define RESET "\033[0m"
#define AMARILLO_T "\x1b[33m"
#define MAGENTA_T "\x1b[35m"
#define BLANCO_T "\x1b[97m"
#define GRIS_T "\x1b[94m"
#define ROJO_T "\x1b[31m"

#define DEBUG_1 1

/* Variables */
char line[COMMAND_LINE_SIZE];
char *args[ARGS_SIZE];

/* Funciones */
char *read_line(char *line);
int execute_line(char *line);
int parse_args(char **args, char *line);
int check_internal(char **args);
int internal_cd(char **args);
int internal_export(char **args);
int internal_source(char **args);
int internal_jobs();
int internal_fg(char **args);
int internal_bg(char **args);

int imprimir_prompt();

int main(int argC, char *argV[]) {
    while (1) {
        if (read_line(line)) {
            execute_line(line);
        }
    }
    return 0;
}

/**
 * Función: char *read_line(char *line)
 * ------------------------------------
 * Descrpción:
 *   Lee la linea de comandos y cambia el "\n" de la última posición por "\0",
 * guardándolo en line.
 * 
 * Argumentos:
 *   - line: puntero donde se guarda la linea leida y modificada.
 * 
 * Salida:
 *   - char *: mismo que line, NULL en el caso de error.
 */
char *read_line(char *line) {
    if (imprimir_prompt() != 0) {
        fprintf(stderr, ROJO_T "Error imprimir_prompt(): " RESET);
    }
    line = fgets(line, COMMAND_LINE_SIZE, stdin);
    if (line == NULL) {
        // caso ctrl + D
        if (feof(stdin)) {
            printf("\r");
            fprintf(stderr, "Bye bye\n");
            exit(0);
        }
        fprintf(stderr, ROJO_T "Error fgets(): " RESET);
    }
    char *punturoLineFeed = strchr(line, '\n');
    if (punteroLineFeed != NULL) {
        *punteroLineFeed = '\0';
    }
    return line;
}
int execute_line(char *line) {
    if(parse_args);
}
int parse_args(char **args, char *line) {}
int check_internal(char **args) {}
int internal_cd(char **args) {}
int internal_export(char **args) {}
int internal_source(char **args) {}
int internal_jobs() {}
int internal_fg(char **args) {}
int internal_bg(char **args) {}

int imprimir_prompt() {
    // Vaciado del buffer
    sleep(1);
    if (fflush(stdout) != 0) {
        fprintf(stderr, ROJO_T "Error fflush(): " RESET);
    }
    // imprimimos el prompt personalizado
    printf(AMARILLO_T "%s" BLANCO_T ":" MAGENTA_T "~%s%s" BLANCO_T "%c " RESET,
           getenv("USER"), getenv("HOME"), getenv("PWD"), PROMPT);
    return 0;
}