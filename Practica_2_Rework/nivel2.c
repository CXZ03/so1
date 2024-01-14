/**
 * Practica 2: Semana 2
 * Autores: Guillem, Elena, Xiaozhe
 * Equipo: AguacateLovers
 * Grupo: 2, 202
 */

/* Librerias */
#define _POSIX_C_SOURCE 200112L  // Version del estándar C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Constantes */
#define COMMAND_LINE_SIZE 1024  // Tamaño máximo de linea de commando
#define ARGS_SIZE 64            // Tamaño máximo de argumentos
#define PROMPT '$'              // Simbolo del prompt

#define RESET "\033[0m"
#define VERDE_T "\x1b[32m"
#define CYAN_T "\x1b[36m"
#define BLANCO_T "\x1b[97m"
#define GRIS_T "\x1b[94m"
#define ROJO_T "\x1b[31m"

#define DEBUG_1 0
#define DEBUG_2 1

/* Variables */
char line[COMMAND_LINE_SIZE];

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
int internal_cd_avanzado(char **args);

/**
 * Función: int main(int argC, char *argV[])
 * -----------------------------------------
 * Descrpción:
 *   Bucle principal del programa donde vamos estar contantemente leyendo y
 * ejecutando el contenido del comando.
 *
 * Salida:
 *   - int: 0 si salida exitosa.
 */
int main() {
    while (1) {
        if (read_line(line)) {
            if (execute_line(line) == -1) {
                fprintf(stderr, ROJO_T "Error main(): execute_line() \n" RESET);
            }
        } else {
            fprintf(stderr, ROJO_T "Error main(): read_line() \n" RESET);
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
        fprintf(stderr, ROJO_T "Error read_line(): imprimir_prompt() \n" RESET);
        return NULL;
    }
    line = fgets(line, COMMAND_LINE_SIZE, stdin);
    if (line == NULL) {
        // Caso ctrl + D
        if (feof(stdin)) {
            printf("\r");
            printf("Bye Bye\n");
            exit(0);
        }
        fprintf(stderr, ROJO_T "Error read_line(): fgets() \n" RESET);
        return NULL;
    }
    char *punteroLineFeed = strchr(line, '\n');
    if (punteroLineFeed != NULL) {
        *punteroLineFeed = '\0';
    }
    return line;
}

/**
 * Función: int execute_line(char *line)
 * -------------------------------------
 * Descrpción:
 *   Ejecuta la linea de comando line, separandolos en tokens y mirar si es un
 * comando interno.
 *
 * Argumentos:
 *   - line: puntero donde se guarda la linea de comandos.
 *
 * Salida:
 *   - int: 0 salida exitosa, -1 si hay error.
 */
int execute_line(char *line) {
    char *args[ARGS_SIZE];
    int cantidadDeToken = parse_args(args, line);
    if (cantidadDeToken == -1) {
        fprintf(stderr, ROJO_T "Error execute_line(): parse_args() \n" RESET);
        return -1;
    }
    if (cantidadDeToken > 0) {
        if (check_internal(args) == -1) {
            fprintf(stderr,
                    ROJO_T "Error execute_line(): check_internal() \n" RESET);
            return -1;
        }
    }
    return 0;
}

/**
 * Función: int parse_args(char **args, char *line)
 * ---------------------------------------
 * Descrpción:
 *   Convierte la linea de comandos line en tokens de comandos que se guarda en
 * args.
 *
 * Argumentos:
 *   - line: puntero donde se guarda la linea de comandos.
 *   - args: puntero donde se guarda los tokens de comandos.
 *
 * Salida:
 *   - int: cantidad de tokens que hay sin contar null.
 */
int parse_args(char **args, char *line) {
    int contadorTokens = 0;
    args[contadorTokens] = strtok(line, " \t\n\r");
#if DEBUG_1
    fprintf(stderr, GRIS_T "[parse_args()→ token %d: %s]\n" RESET,
            contadorTokens, args[contadorTokens]);
#endif
    while (args[contadorTokens] != NULL && *args[contadorTokens] != '#') {
        args[++contadorTokens] = strtok(NULL, " \t\n\r");
#if DEBUG_1
        fprintf(stderr, GRIS_T "[parse_args()→ token %d: %s]\n" RESET,
                contadorTokens, args[contadorTokens]);
#endif
    }
    if (args[contadorTokens]) {
        args[contadorTokens] = NULL;
#if DEBUG_1
        fprintf(stderr, GRIS_T "[parse_args()→ token %d corregido: %s]\n" RESET,
                contadorTokens, args[contadorTokens]);
#endif
    }
    return contadorTokens;
}

/**
 * Función: int check_internal(char **args)
 * ---------------------------------------
 * Descrpción:
 *   Es una función booleana que averigua si args[0] se trata de un comando
 * interno, en el caso de que lo sea llama a la función correspondiente para
 * tratarlo.
 *
 * Argumentos:
 *   - args: puntero donde se guarda los tokens de comandos.
 *
 * Salida:
 *   - int: 1 en el caso de que sea una función interna, 0 en el caso
 * contrario.
 */
int check_internal(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        if (internal_cd(args) == -1) {
            fprintf(stderr,
                    ROJO_T "Error check_internal(): internal_cd() \n" RESET);
            return -1;
        }
        return 1;
    } else if (strcmp(args[0], "export") == 0) {
        if (internal_export(args) == -1) {
            fprintf(stderr, ROJO_T
                    "Error check_internal(): internal_export() \n" RESET);
            return -1;
        }
        return 1;
    } else if (strcmp(args[0], "source") == 0) {
        if (internal_source(args) == -1) {
            fprintf(stderr, ROJO_T
                    "Error check_internal(): internal_source() \n" RESET);
            return -1;
        }
        return 1;
    } else if (strcmp(args[0], "jobs") == 0) {
        if (internal_jobs() == -1) {
            fprintf(stderr,
                    ROJO_T "Error check_internal(): internal_jobs() \n" RESET);
            return -1;
        }
        return 1;
    } else if (strcmp(args[0], "bg") == 0) {
        if (internal_bg(args) == -1) {
            fprintf(stderr,
                    ROJO_T "Error check_internal(): internal_bg() \n" RESET);
            return -1;
        }
        return 1;
    } else if (strcmp(args[0], "fg") == 0) {
        if (internal_fg(args) == -1) {
            fprintf(stderr,
                    ROJO_T "Error check_internal(): internal_fg() \n" RESET);
            return -1;
        }
        return 1;
    } else if (strcmp(args[0], "exit") == 0) {
        printf("Bye Bye\n");
        exit(0);
    }
    return 0;
}

/**
 * Función: int internal_cd(char **args)
 * ---------------------------------------
 * Descrpción:
 *
 * Argumentos:
 *   - args:
 *
 * Salida:
 *   - int:
 */
int internal_cd(char **args) {
#if DEBUG_1
    fprintf(stderr, GRIS_T
            "[internal_cd()→ Esta función cambiará de directorio]\n" RESET);
#endif
    if (args == NULL) {
        fprintf(stderr, ROJO_T "Error internal_cd(): args == NULL\n" RESET);
    }
    // int longitudArgs = sizeof(args) / sizeof(*args);
    // printf("longitudArgs: %d", longitudArgs);
    if (args[1] == NULL) {
        if (chdir(getenv("HOME"))) {
            fprintf(stderr, ROJO_T "Error internal_cd(): chdir() \n" RESET);
            return -1;
        }
    } else if (args[2] == NULL) {
        if (chdir(args[1])) {
            fprintf(stderr, ROJO_T "Error internal_cd(): chdir() \n" RESET);
            return -1;
        }
    } else {
        if (internal_cd_avanzado(args) == -1) {
            fprintf(stderr, ROJO_T "Error internal_cd(): chdir() \n" RESET);
            return -1;
        }
    }
    return 0;
}

/**
 * Función: int internal_export(char **args)
 * ---------------------------------------
 * Descrpción:
 *
 * Argumentos:
 *   - args:
 *
 * Salida:
 *   - int:
 */
int internal_export(char **args) {
#if DEBUG_1
    fprintf(stderr, GRIS_T
            "[internal_cd()→ Esta función asignará valores a variablescd de "
            "entorno]\n" RESET);
#endif
    const char *IGUAL = "=";
    char *nombre, *valor;
    if (args[1] == NULL) {
        fprintf(stderr, "Error internal_export(): Error de sintaxis\n");
    }
    nombre = strtok(args[1], IGUAL);
    valor = strtok(NULL, IGUAL);
    if (nombre == NULL || valor == NULL) {
        fprintf(stderr, "Error internal_export(): Error de sintaxis\n");
    }
#if DEBUG_2
    printf("[internal_export() → nombre: %s]\n", nombre);
    printf("[internal_export() → valor: %s]\n", valor);
    printf("[internal_export() → antiguo valor para %s: %s]\n", nombre,
           getenv(nombre));
#endif
    setenv(nombre, valor, 1);
#if DEBUG_2
    printf("[internal_export() → nuevo valor para %s: %s]\n", nombre,
           getenv(nombre));
#endif

    return 0;
}

/**
 * Función: int internal_source(char **args)
 * ---------------------------------------
 * Descrpción:
 *
 * Argumentos:
 *   - args:
 *
 * Salida:
 *   - int:
 */
int internal_source(char **args) {
#if DEBUG_1
    fprintf(stderr, GRIS_T
            "[internal_cd()→ Esta función ejecutará un fichero de líneas de "
            "comandos]\n]" RESET);
#endif
    return 0;
}

/**
 * Función: internal_jobs()
 * ---------------------------------------
 * Descrpción:
 *
 * Salida:
 *   - int:
 */
int internal_jobs() {
#if DEBUG_1
    fprintf(stderr, GRIS_T
            "[internal_cd()→ Esta función mostrará el PID de los procesos que "
            "no estén en foreground]\n" RESET);
#endif
    return 0;
}

/**
 * Función: int internal_fg(char **args)
 * ---------------------------------------
 * Descrpción:
 *
 * Argumentos:
 *   - args:
 *
 * Salida:
 *   - int:
 */
int internal_fg(char **args) {
#if DEBUG_1
    fprintf(stderr, GRIS_T
            "[internal_cd()→ Esta función enviará un trabajo detenido al "
            "foreground reactivando su ejecución, o uno del background al "
            "foreground]\n" RESET);
#endif
    return 0;
}

/**
 * Función: int internal_bg(char **args)
 * ---------------------------------------
 * Descrpción:
 *
 * Argumentos:
 *   - args:
 *
 * Salida:
 *   - int:
 */
int internal_bg(char **args) {
#if DEBUG_1
    fprintf(stderr, GRIS_T
            "[internal_bg()→ Esta función mostrará el background]\n" RESET);
#endif
    return 0;
}

/**
 * Función: int imprimir_prompt()
 * ---------------------------------------
 * Descrpción:
 *   Imprime el prompt del MiniShell con los datos del usuario y el
 * directorio actual.
 *
 * Salida:
 *  - int: 0 salida exitosa.
 */
int imprimir_prompt() {
    // Vaciado del buffer
    sleep(1);
    if (fflush(stdout) != 0) {
        fprintf(stderr, ROJO_T "Error imprimir_prompt(): fflush() \n" RESET);
    }
    char *cwd = malloc(COMMAND_LINE_SIZE * sizeof(*cwd));
    // Imprimimos el prompt personalizado
    printf(VERDE_T "%s" BLANCO_T ":" CYAN_T "%s" BLANCO_T "%c " RESET,
           getenv("USER"), getcwd(cwd, COMMAND_LINE_SIZE), PROMPT);
    free(cwd);
    return 0;
}

int internal_cd_avanzado(char **args) {
    // Crear un charArray vacio para guardar los argumentos
    char *const path = calloc(COMMAND_LINE_SIZE + 1, sizeof(*path));

    int i = 1;  // saltarse el "cd"
    strcat(path, args[i++]);
    for (; args[i] != NULL; i++) {
        strcat(path, " ");
        strcat(path, args[i]);
    }
    printf("path: %s\n", path);

    // Buscar caracter especial
    char *ptrPath;
    ptrPath = strchr(path, '\'');
    if (ptrPath) {
        // Caso comilla simple
    }
    ptrPath = strchr(path, '\"');
    if (ptrPath) {
        // Caso comilla doble
    }
    ptrPath = strchr(path, '\\');
    if (ptrPath) {
        // Caso barra
    }

    return 0;
}
