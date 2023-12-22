/* Practica 2: Nivel 2*/
/* Autores: Guillem, Elena, Xiaozhe */

/* Declaración de debugs */
#define DEBUG_1 1
#define DEBUG_0 0
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
#define RED "\x1b[31m"

/* Declaración de constantes */

#define COMMAND_LINE_SIZE 1024  // tamaño máximo de linea de commando
#define ARGS_SIZE 64            // tamaño máximo de argumentos
#define PROMPT '$'              // simbolo del prompt
#define N_JOBS 64

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

struct info_job {
    pid_t pid;
    char estado;  // ‘N’, ’E’, ‘D’, ‘F’ (‘N’: Ninguno, ‘E’: Ejecutándose y ‘D’:
                  // Detenido, ‘F’: Finalizado)
    char cmd[COMMAND_LINE_SIZE];  // línea de comando asociada
};

/* Declaración de variables */
char line[COMMAND_LINE_SIZE];             // almacena la línea de comando
char *args[ARGS_SIZE];                    // almacena los argumentos disponibles
static char mi_shell[COMMAND_LINE_SIZE];  // almacena comando en ejecucion
static struct info_job jobs_list[N_JOBS];  // almacena la tablaa

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
    char *linea = malloc(sizeof(char) * COMMAND_LINE_SIZE);

    if (linea == NULL) {
        fprintf(stderr, "No hay memoria.\n");
    }

    int NArgs = 0;

    for (int i = 0; args[i]; i++) {
        strcat(linea, " ");
        strcat(linea, args[i]);
        NArgs++;
    }

    // Si es una palabra
    {
        if (NArgs == 1) {
            if (chdir(getenv("HOME"))) {
                perror("Error");
            }
        } else {
            if (chdir(args[1])) {
                perror("Error");
            }
        }
    }

#if DEBUG_1
    char *prompt;

    if ((prompt = malloc((sizeof(char) * COMMAND_LINE_SIZE)))) {
        // Obtiene el directorio de trabajo actual
        getcwd(prompt, COMMAND_LINE_SIZE);
        printf("[internal_cd() → %s]\n", prompt);
    } else {
        perror("Error");
    }
    free(prompt);
#endif
    free(linea);
    return 1;
}

// INTERNAL EXPORT
// DESCOMPONEMOS NOMBRE = VALOR
//
int internal_export(char **args) {
    const char *IGUAL = "=";
    char *nombre, *valor;
    // ERROR POR SI EL PRIMER TOKEN ESTA VACIO
    if (args[1] == NULL) {
        fprintf(stderr, "Error de sintaxis\n");
    }
    // SI NO HAY ERROES ASIGNAMOS VALORES
    else {
        nombre = strtok(args[1], IGUAL);
        valor = strtok(NULL, IGUAL);
    }
    // MOSTRAMOS ERROR PORQUE NO HAY NADA
    if (args[1] == NULL || valor == NULL) {
        fprintf(stderr, "Error de sintaxis\n");
    } else {
#if DEBUG_1
        printf("[internal_export() → nombre: %s]\n", nombre);
        printf("[internal_export() → valor: %s]\n", valor);
        printf("[internal_export() → antiguo valor para %s: %s]\n", nombre,
               getenv(nombre));
#endif
        setenv(nombre, valor, 1);
#if DEBUG_1
        printf("[internal_export() → nuevo valor para %s: %s]\n", nombre,
               getenv(nombre));
#endif
    }

    return 1;
}

int internal_source(char **args) {
    char *linea = (char *)malloc(sizeof(char) * COMMAND_LINE_SIZE);
    // abrimos el archivo
    if (linea) {
        FILE *fich = fopen(args[1], "r");
        if (fich) {
            while (fgets(linea, COMMAND_LINE_SIZE, fich)) {
                // pasamos las lineas del fichero a execute_line
                if (strlen(linea) > 0) {  // Verificar si la línea no está vacía
                    execute_line(linea);
                }
            }
            fclose(fich);
            free(linea);
            return EXIT_SUCCESS;
            // error al abrir fichero
        } else {
            perror("Error");
            free(linea);
        }
    }

#if DEBUG_0
    printf(
        "[internal_source()→Esta función ejecutará un fichero de líneas de "
        "comandos]\n");
#endif

    return EXIT_FAILURE;
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
    // printf("%s:~%s%s%c ",getenv("USER"), getenv("HOME"), getenv("PWD"),
    // PROMPT);

    // actualizacion del prompt ahora se actualiza
    char *prompt;
    if ((prompt = malloc((sizeof(char) * COMMAND_LINE_SIZE))))
        getcwd(prompt, COMMAND_LINE_SIZE);

    printf("%s:~%s%c ", getenv("USER"), prompt, PROMPT);
    printf(RESET_FORMATO);
}

// La visualizacion de los tokens se ha comentado
int parse_args(char **args, char *line) {
    int i = 0;
    args[i] = strtok(line, " \t\n\r");

#if DEBUG_1
    // fprintf(stderr, AZUL_T "[parse_args()→ token %i: %s]\n" RESET_FORMATO, i,
    // args[i]);
#endif
    while (args[i] && args[i][0] != '#') {
        i++;
        args[i] = strtok(NULL, " \t\n\r");

#if DEBUG_1
        // fprintf(stderr,AZUL_T  "[parse_args()→ token %i: %s]\n"
        // RESET_FORMATO, i, args[i]);
#endif
    }
    if (args[i]) {
        // en caso de que el ultimo token sea simbolo comentario
        args[i] = NULL;
#if DEBUG_1
        // fprintf(stderr, AZUL_T "[parse_args()→ token %i corregido: %s]\n"
        // RESET_FORMATO, i, args[i]);
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
            perror("Error");
            return 1;
        } else {
            fprintf(stderr,
                    GRIS "[execute_line()→ PID padre: %d]\n" RESET_FORMATO,
                    getpid());
            pid = fork();
            if (pid == 0) {
                fprintf(stderr,
                        GRIS
                        "[execute_line()→ PID hijo: %d(%s)]\n" RESET_FORMATO,
                        getpid(), args[0]);
                if (strcmp(args[0], "source\n") == 0) {
                    // Manejo especial para el comando "source"
                    internal_source(args);
                } else {
                    execvp(args[0], args);
                    fprintf(stderr, "%s: no se encontró la orden\n", line);
                    exit(1);
                }
            } else if (pid > 0) {
                jobs_list[0].pid = pid;
                jobs_list[0].estado = 'E';
                wait(&status);
                if (WIFEXITED(status)) {
                    fprintf(stderr,
                            "[execute_line()→ Proceso hijo %d (%s) finalizado "
                            "con exit(), estado: %d]\n",
                            pid, command_line, WEXITSTATUS(status));
                            printf("\n");
                } else {
                    if (WIFSIGNALED(status)) {
                        fprintf(stderr,
                                "[execute_line()→ Proceso hijo %d (%s) "
                                "finalizado por señal %d]\n",
                                pid, command_line, WTERMSIG(status));
                    }
                }
            }
        }
        return 0;
    } else {
        return -1;
    }
}

int main(int argC, char *argV[]) {
    // Guardamos nombre programa
    // ponemos a 0 joblist, pid y el estado a N
    jobs_list[0].pid = 0;
    jobs_list[0].estado = 'N';
    strcpy(jobs_list[0].cmd, "");
    // guardamos la ejecucion del minishell
    strcpy(mi_shell, argV[0]);
    char line[COMMAND_LINE_SIZE];
    memset(line, 0, COMMAND_LINE_SIZE);
    while (1) {
        if (read_line(line)) {
            execute_line(line);
        }
    }
    return 0;
}