/**
 * Practica 2:      my_shell
 * Autores:         Guillem, Elena, Xiaozhe
 * Equipo:          AguacateLovers
 * Grupo grande:    2
 * Grupo mediano:   202
 */

/* Librerias */
#define _POSIX_C_SOURCE 200112L  // Version del estándar C

#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Constantes */
#define COMMAND_LINE_SIZE 1024  // Tamaño máximo de linea de commando
#define ARGS_SIZE 64            // Tamaño máximo de argumentos
#define PROMPT '$'              // Simbolo del prompt
#define N_JOBS 64

#define RESET "\033[0m"
#define VERDE_T "\x1b[32m"
#define CYAN_T "\x1b[36m"
#define BLANCO_T "\x1b[97m"
#define GRIS_T "\x1b[94m"
#define ROJO_T "\x1b[31m"

#define ERASE_LINE "\33[2K"

#define DEBUG_1 0
#define DEBUG_2 0
#define DEBUG_3 0
#define DEBUG_4 0
#define DEBUG_5 0
#define DEBUG_6 0

/* Estructuras */
struct info_job {
    pid_t pid;
    char estado;  // ‘N’, ’E’, ‘D’, ‘F’ (‘N’: Ninguno, ‘E’: Ejecutándose y ‘D’:
                  // Detenido, ‘F’: Finalizado)
    char cmd[COMMAND_LINE_SIZE];  // línea de comando asociada
};

/* Variables */
char line[COMMAND_LINE_SIZE];
static struct info_job jobs_list[N_JOBS];
static char mi_shell[COMMAND_LINE_SIZE];
int n_job = 0;

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
void reaper(int signum);
void ctrlc(int signum);
void ctrlz(int signum);
int is_background(char **args);
int jobs_list_add(pid_t pid, char estado, char *cmd);
int jobs_list_find(pid_t pid);
int jobs_list_remove(int pos);
int is_output_redirection(char **args);

int imprimir_prompt();
int internal_cd_avanzado(char **args);

/**
 * Función: int main(int argc, char *argv[])
 * -----------------------------------------------------------------------------
 * Descrpción:
 *   Bucle principal del programa donde vamos estar contantemente leyendo y
 * ejecutando el contenido del comando.
 *
 * Salida:
 *   - int: 0 si salida exitosa.
 */
int main(int argc, char *argv[]) {
    // Inicializamos jobs_list[0] (Foreground)
    jobs_list[0].pid = 0;
    jobs_list[0].estado = 'N';
    strcpy(jobs_list[0].cmd, "");
    // Guardamos la ejecucion del minishell
    strcpy(mi_shell, argv[0]);
    // Inicializamos las señales
    signal(SIGCHLD, reaper);
    signal(SIGINT, ctrlc);
    signal(SIGTSTP, ctrlz);
    while (1) {
        if (read_line(line)) {
            if (execute_line(line) == -1) {
                fprintf(stderr, ROJO_T "Error main(): execute_line() \n" RESET);
            }
        }
        // else {
        //      fprintf(stderr, ROJO_T "Error main(): read_line() \n" RESET);
        // }
    }
    return 0;
}

/**
 * Función: char *read_line(char *line)
 * -----------------------------------------------------------------------------
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
#if DEBUG_1
            printf("Bye Bye\n");
#endif
            exit(0);
        }
        // fprintf(stderr, ROJO_T "Error read_line(): fgets() \n" RESET);
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
 * -----------------------------------------------------------------------------
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
        switch (check_internal(args)) {
            case 0:
                // Caso comando no interno
                pid_t pid;
                bool cmdIsBackground = is_background(args);
                pid = fork();
                if (pid == 0) {
                    // Proceso hijo
                    signal(SIGCHLD, SIG_DFL);
                    signal(SIGINT, SIG_IGN);   // Ignorar el ctrl + C
                    signal(SIGTSTP, SIG_IGN);  // Ignorar el ctrl + Z
                    is_output_redirection(args);
                    execvp(args[0], args);  // Ejecutar comando
                    // #if DEBUG_3
                    fprintf(stderr,
                            ROJO_T "%s: no se encontró la orden\n" RESET, line);
                    // #endif
                    exit(1);

                } else if (pid > 0) {
                    // Proceso padre
#if DEBUG_3
                    fprintf(stderr,
                            GRIS_T
                            "[execute_line()→ PID padre: %d (%s)]\n" RESET,
                            getpid(), mi_shell);
                    fprintf(stderr,
                            GRIS_T
                            "[execute_line()→ PID hijo: %d (%s)]\n" RESET,
                            pid, line);
#endif
                    if (!cmdIsBackground) {
                        jobs_list[0].pid = pid;
                        jobs_list[0].estado = 'E';
                        strcpy(jobs_list[0].cmd, line);
                    } else {
                        jobs_list_add(pid, 'E', line);
#if DEBUG_5
                        fprintf(stderr, "[%d] %d\t%c\t%s\n", n_job,
                                jobs_list[n_job].pid, jobs_list[n_job].estado,
                                jobs_list[n_job].cmd);
#endif
                    }
                } else {
                    fprintf(stderr,
                            ROJO_T "Error execute_line(): fork()\n" RESET);
                }
                // Esperar señal de los procesos hijos
                while (jobs_list[0].pid > 0) {
                    pause();
                }
                break;
            case -1:
                // Caso error
                fprintf(stderr, ROJO_T
                        "Error execute_line(): check_internal() \n" RESET);
                return -1;
        }
    }
    return 0;
}

/**
 * Función: int parse_args(char **args, char *line)
 * -----------------------------------------------------------------------------
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
    char lineCopia[COMMAND_LINE_SIZE];
    strcpy(lineCopia, line);
    int contadorTokens = 0;
    args[contadorTokens] = strtok(lineCopia, " \t\n\r");
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
 * -----------------------------------------------------------------------------
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
#if DEBUG_1
        printf("Bye Bye\n");
#endif
        exit(0);
    }
    return 0;
}

/**
 * Función: int internal_cd(char **args)
 * -----------------------------------------------------------------------------
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
    if (args[1] == NULL) {
        if (chdir(getenv("HOME"))) {
            fprintf(stderr, ROJO_T
                    "Error internal_cd(): chdir(): No such file or "
                    "directory\n" RESET);
            return -1;
        }
    } else if (args[2] == NULL) {
        if (chdir(args[1])) {
            fprintf(stderr, ROJO_T
                    "Error internal_cd(): chdir(): No such file or "
                    "directory\n" RESET);
            return -1;
        }
    } else {
        if (internal_cd_avanzado(args) == -1) {
            fprintf(stderr, ROJO_T
                    "Error internal_cd(): internal_cd_avanzado()\n" RESET);
            return -1;
        }
    }
#if DEBUG_2
    char *const cwd = malloc((COMMAND_LINE_SIZE + 1) * sizeof(*cwd));
    if (getcwd(cwd, COMMAND_LINE_SIZE) == NULL) {
        fprintf(stderr, ROJO_T "Error internal_cd: getcwd(): \n" RESET);
        return -1;
    }
    fprintf(stderr, GRIS_T "[internal_cd() → %s]\n" RESET, cwd);
    free(cwd);
#endif
    return 0;
}

/**
 * Función: int internal_export(char **args)
 * -----------------------------------------------------------------------------
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
        fprintf(stderr,
                ROJO_T "Error internal_export(): Error de sintaxis\n" RESET);
        return -1;
    }
    nombre = strtok(args[1], IGUAL);
    valor = strtok(NULL, IGUAL);
#if DEBUG_2
    printf(GRIS_T "[internal_export() → nombre: %s]\n" RESET, nombre);
    printf(GRIS_T "[internal_export() → valor: %s]\n" RESET, valor);
#endif
    if (nombre == NULL || valor == NULL) {
        fprintf(stderr,
                ROJO_T "Error internal_export(): Error de sintaxis\n" RESET);
        return -1;
    }
#if DEBUG_2
    printf(GRIS_T "[internal_export() → antiguo valor para %s: %s]\n" RESET,
           nombre, getenv(nombre));
#endif
    setenv(nombre, valor, 1);
#if DEBUG_2
    printf(GRIS_T "[internal_export() → nuevo valor para %s: %s]\n" RESET,
           nombre, getenv(nombre));
#endif

    return 0;
}

/**
 * Función: int internal_source(char **args)
 * -----------------------------------------------------------------------------
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
    FILE *fich = fopen(args[1], "r");
    if (fich == NULL) {
#if DEBUG_3
        fprintf(stderr,
                ROJO_T "Error de sintaxis. Uso: source <nombre_fichero>\n");
#endif
        fprintf(stderr, ROJO_T "Error internal_source(): fopen()\n");
        return -1;
    }
    char *linea = malloc((COMMAND_LINE_SIZE + 1) * sizeof(*linea));
    linea = fgets(linea, COMMAND_LINE_SIZE, fich);
    while (linea != NULL) {
        // pasamos las lineas del fichero a execute_line
#if DEBUG_3
        char *ptrLineFeed = strchr(linea, '\n');
        *ptrLineFeed = '\0';
        fprintf(stderr, GRIS_T "[internal_source()→ LINE: %s]\n", linea);
#endif
        execute_line(linea);
        linea = fgets(linea, COMMAND_LINE_SIZE, fich);
        fflush(fich);
    }
    free(linea);
    fclose(fich);
    return 0;
}

/**
 * Función: internal_jobs()
 * -----------------------------------------------------------------------------
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
    // Imprimimos jobs_list
    for (int i = 1; i <= n_job; i++) {
        printf("[%d] %d\t%c\t%s\n", i, jobs_list[i].pid, jobs_list[i].estado,
               jobs_list[i].cmd);
    }
    return 0;
}

/**
 * Función: int internal_fg(char **args)
 * -----------------------------------------------------------------------------
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
    if (args[1] == NULL) {
        fprintf(stderr, ROJO_T "Error internal_fg(): falta argumento\n" RESET);
        return -1;
    }
    int posJob = atoi(args[1]);
    if (posJob <= 0 || posJob > n_job) {
        fprintf(stderr,
                ROJO_T "Error internal_fg(): el trabajo %d no existe\n" RESET,
                posJob);
        return -1;
    }
    if (jobs_list[posJob].estado == 'D') {
        jobs_list[posJob].estado = 'E';
        kill(jobs_list[posJob].pid, SIGCONT);
#if DEBUG_6
        fprintf(stderr,
                GRIS_T
                "[internal_fg()→ Señal %d (SIGCONT) enviada a %d (%s)]\n" RESET,
                SIGCONT, jobs_list[posJob].pid, jobs_list[posJob].cmd);
#endif
    }
    char *ptrAmpersand = strchr(jobs_list[posJob].cmd, '&');
    if (ptrAmpersand) {
        *ptrAmpersand = '\0';
    }
    // Mover el trabajo a fg
    jobs_list[0].pid = jobs_list[posJob].pid;
    jobs_list[0].estado = jobs_list[posJob].estado;
    strcpy(jobs_list[0].cmd, jobs_list[posJob].cmd);
    jobs_list_remove(posJob);
    // #if DEBUG_6
    fprintf(stderr, "%s\n", jobs_list[0].cmd);
    fflush(stderr);
    // #endif
    while (jobs_list[0].pid) {
        pause();
    }
    return 0;
}

/**
 * Función: int internal_bg(char **args)
 * -----------------------------------------------------------------------------
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
    if (args[1] == NULL) {
        fprintf(stderr, ROJO_T "Error internal_bg(): falta argumento" RESET);
        return -1;
    }
    int posJob = atoi(args[1]);
    if (posJob <= 0 || posJob >= N_JOBS) {
        fprintf(stderr,
                ROJO_T "Error internal_bg(): el trabajo %d no existe\n" RESET,
                posJob);
        return -1;
    }
    if (jobs_list[posJob].estado == 'D') {
        jobs_list[posJob].estado = 'E';
        strcat(jobs_list[posJob].cmd, " &");
        kill(jobs_list[posJob].pid, SIGCONT);
#if DEBUG_6
        fprintf(stderr,
                GRIS_T "[internal_bg() -> Señal %d enviada a %d (%s)\n" RESET,
                SIGCONT, jobs_list[posJob].pid, jobs_list[posJob].cmd);
#endif
    } else if (jobs_list[posJob].estado == 'E') {
        fprintf(stderr,
                ROJO_T "El trabajo %d, ya está en segundo plano\n" RESET,
                posJob);
        return 0;
    }
    // #if DEBUG_6
    fprintf(stderr, "[%d] %d\t%c\t%s\n", n_job, jobs_list[posJob].pid,
            jobs_list[posJob].estado, jobs_list[posJob].cmd);
    // #endif
    while (jobs_list[0].pid) {
        pause();
    }
    return 0;
}

/**
 * Función: void reaper(int signum)
 * -----------------------------------------------------------------------------
 * Descrpción:
 *   Manejador propia para la señal SIGCHLD.
 *
 * Argumentos:
 *   - signum: número de la señal recibida
 *
 * Salida:
 *   - void
 */
void reaper(int signum) {
    signal(SIGCHLD, reaper);
#if DEBUG_5
    fprintf(stderr,
            GRIS_T ERASE_LINE
            "\r[reaper()→ recibida señal %d (SIGINT)]\n" RESET,
            SIGCHLD);
#endif
    pid_t endedPid;
    int status;
    while ((endedPid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (endedPid == jobs_list[0].pid) {
            // Se ha acabado un hijo fg, restaura los valores del fg
            if (WIFEXITED(status)) {
#if DEBUG_4
                printf(GRIS_T ERASE_LINE
                       "\r[reaper()→ Proceso hijo %d (%s) finalizado con exit "
                       "code "
                       "%d]\n" RESET,
                       endedPid, jobs_list[0].cmd, (status));
#endif
            } else if (WIFSIGNALED(status)) {
#if DEBUG_4
                printf(GRIS_T ERASE_LINE
                       "\r[reaper()→ Proceso hijo %d (%s) finalizado con exit "
                       "code "
                       "%d]\n" RESET,
                       endedPid, jobs_list[0].cmd, WTERMSIG(status));
#endif
            }
            jobs_list[0].pid = 0;
            jobs_list[0].estado = 'F';
            strcpy(jobs_list[0].cmd, "");
        } else {
            // Se ha acabado un hijo bg
            int posPid = jobs_list_find(endedPid);
            if (WIFEXITED(status)) {
#if DEBUG_4
                printf(GRIS_T ERASE_LINE
                       "\r[reaper()→ Proceso hijo %d (%s) finalizado con exit "
                       "code "
                       "%d]\n" RESET,
                       endedPid, jobs_list[jobs_list_find(endedPid)].cmd,
                       WEXITSTATUS(status));
#endif
                // #if DEBUG_6
                fprintf(
                    stderr,
                    ERASE_LINE
                    "\rTerminado PID %d (%s) en jobs_list[%d] con status %d\n",
                    jobs_list[posPid].pid, jobs_list[posPid].cmd, posPid,
                    WEXITSTATUS(status));
                // #endif
            } else if (WIFSIGNALED(status)) {
#if DEBUG_4
                printf(GRIS_T ERASE_LINE
                       "\r[reaper()→ Proceso hijo %d (%s) finalizado con exit "
                       "code "
                       "%d]\n" RESET,
                       endedPid, jobs_list[jobs_list_find(endedPid)].cmd,
                       WTERMSIG(status));
#endif
                // #if DEBUG_6
                fprintf(
                    stderr,
                    ERASE_LINE
                    "\rTerminado PID %d (%s) en jobs_list[%d] con status %d\n",
                    jobs_list[posPid].pid, jobs_list[posPid].cmd, posPid,
                    WTERMSIG(status));
                // #endif
            }
            jobs_list_remove(posPid);
        }
    }
}

/**
 * Función: void ctrlc(int signum)
 * -----------------------------------------------------------------------------
 * Descrpción:
 *   Manejador propia para la señal SIGINT (ctrl + c).
 *
 * Argumentos:
 *   - signum: número de la señal recibida
 *
 * Salida:
 *   - void
 */
void ctrlc(int signum) {
    signal(SIGINT, ctrlc);
    // Alinear el prompt
    fprintf(stderr, "\n");
    fflush(stdout);
    if (jobs_list[0].pid > 0) {
        // Hay un proceso en el fg
        if (strcmp(jobs_list[0].cmd, mi_shell)) {
            // El proceso no es el del minishell
#if DEBUG_4
            fprintf(stderr,
                    GRIS_T
                    "[ctrlc()→ Soy el proceso con PID %d (%s), el "
                    "proceso en foreground es %d (%s)]\n" RESET,
                    getpid(), mi_shell, jobs_list[0].pid, jobs_list[0].cmd);
            fprintf(stderr,
                    GRIS_T "[ctrlc()→ recibida señal %d (SIGINT)]\n" RESET,
                    SIGINT);
            fprintf(stderr,
                    GRIS_T
                    "[ctrlc()→ Señal %d (SIGTERM) enviada a %d (%s) por %d "
                    "(%s)]\n" RESET,
                    SIGTERM, jobs_list[0].pid, jobs_list[0].cmd, getpid(),
                    mi_shell);
#endif
            kill(jobs_list[0].pid, SIGTERM);
        } else {
#if DEBUG_4
            fprintf(stderr,
                    GRIS_T
                    "[ctrlc()→ Señal %d (SIGTERM) no enviada por %d (%s) "
                    "debido a que el "
                    "proceso actual es un mini_shell]\n" RESET,
                    SIGTERM, getpid(), mi_shell);
#endif
        }
    } else {
#if DEBUG_4
        fprintf(stderr,
                GRIS_T
                "[ctrlc()→ Soy el proceso con PID %d (%s), el proceso en "
                "foreground es %d (%s)]\n" RESET,
                getpid(), mi_shell, jobs_list[0].pid, jobs_list[0].cmd);
        fprintf(stderr, GRIS_T "[ctrlc()→ recibida señal %d (SIGINT)]\n" RESET,
                SIGINT);
        fprintf(stderr,
                GRIS_T
                "[ctrlc()→ Señal %d (SIGTERM) no enviada por %d (%s) debido a "
                "que no hay "
                "proceso en foreground]\n" RESET,
                SIGTERM, getpid(), mi_shell);
#endif
    }
}

/**
 * Función: void ctrlc(int signum)
 * -----------------------------------------------------------------------------
 * Descrpción:
 *   Manejador propia para la señal SIGINT (ctrl + c).
 *
 * Argumentos:
 *   - signum: número de la señal recibida
 *
 * Salida:
 *   - void
 */
void ctrlz(int signum) {
    signal(SIGTSTP, ctrlz);
    // Alinear el prompt
    fprintf(stderr, "\n");
    fflush(stdout);
    if (jobs_list[0].pid > 0) {
        // Hay un proceso en el fg
        if (strcmp(jobs_list[0].cmd, mi_shell)) {
            // El proceso no es el del minishell
#if DEBUG_5
            fprintf(stderr,
                    GRIS_T
                    "[ctrlz()→ Soy el proceso con PID %d (%s), el "
                    "proceso en foreground es %d (%s)]\n" RESET,
                    getpid(), mi_shell, jobs_list[0].pid, jobs_list[0].cmd);
            fprintf(stderr,
                    GRIS_T "[ctrlz()→ recibida señal %d (SIGTSTP)]\n" RESET,
                    SIGTSTP);
            fprintf(stderr,
                    GRIS_T
                    "[ctrlz()→ Señal %d (SIGSTOP) enviada a %d (%s) por %d "
                    "(%s)]\n" RESET,
                    SIGSTOP, jobs_list[0].pid, jobs_list[0].cmd, getpid(),
                    mi_shell);
#endif
            // Paramos el proceso fg
            kill(jobs_list[0].pid, SIGSTOP);
            // Ponemos el proceso el jobs_list
            jobs_list[0].estado = 'D';
            jobs_list_add(jobs_list[0].pid, jobs_list[0].estado,
                          jobs_list[0].cmd);
            // Reseteamos foreground
            jobs_list[0].pid = 0;
            jobs_list[0].estado = 'N';
            strcpy(jobs_list[0].cmd, "");
        } else {
// El proceso es el del minishell
#if DEBUG_5
            fprintf(stderr,
                    GRIS_T
                    "[ctrlz()→ Señal %d (SIGSTOP) no enviada por %d (%s) "
                    "debido a que el "
                    "proceso actual es un mini_shell]\n" RESET,
                    SIGSTOP, getpid(), mi_shell);
#endif
        }
    } else {
// No hay un proceso en el fg
#if DEBUG_5
        fprintf(stderr,
                GRIS_T
                "[ctrlz()→ Soy el proceso con PID %d (%s), el proceso en "
                "foreground es %d (%s)]\n" RESET,
                getpid(), mi_shell, jobs_list[0].pid, jobs_list[0].cmd);
        fprintf(stderr, GRIS_T "[ctrlz()→ recibida señal %d (SIGTSTP)]\n" RESET,
                SIGTSTP);
        fprintf(stderr,
                GRIS_T
                "[ctrlz()→ Señal %d no enviada por %d (%s) debido a que no hay "
                "proceso en foreground]\n" RESET,
                SIGSTOP, getpid(), mi_shell);
#endif
    }
}

/**
 * Función: int is_background(char **args)
 * -----------------------------------------------------------------------------
 * Descrpción:
 *   Localiza '&' para ver si es un comando background o no.
 *
 * Argumentos:
 *   - args: linea de comandos en tokens que checkear.
 *
 * Salida:
 *   - int: 1 si es background, 0 si no lo es.
 */
int is_background(char **args) {
    for (int i = 0; args[i] != NULL; i++) {
        if (args[i][0] == '&') {
            // caso '&', es un proceso background
            args[i] = NULL;
            return 1;
        }
    }
    return 0;
}

/**
 * Función: int jobs_list_add(pid_t pid, char estado, char *cmd)
 * -----------------------------------------------------------------------------
 * Descrpción:
 *   Añade un proceso en la joblist.
 *
 * Argumentos:
 *   - pid: número pid del proceso.
 *   - estado: estado del proceso.
 *   - cmd: comando del proceso.
 *
 * Salida:
 *   - int: 0 salida exitosa, -1 salida fallida.
 */
int jobs_list_add(pid_t pid, char estado, char *cmd) {
    if (n_job >= N_JOBS) {
        fprintf(stderr, "cantidad máxima de trabajo sobrepasado");
        return -1;
    }
    n_job++;
    jobs_list[n_job].pid = pid;
    jobs_list[n_job].estado = estado;
    strcpy(jobs_list[n_job].cmd, cmd);
    // #if DEBUG_6
    fprintf(stderr, "[%d] %d\t%c\t%s\n", n_job, jobs_list[n_job].pid,
            jobs_list[n_job].estado, jobs_list[n_job].cmd);
    // #endif
    return 0;
}

/**
 * Función: int jobs_list_find(pid_t pid)
 * -----------------------------------------------------------------------------
 * Descrpción:
 *   Buscar la posición de un proceso en la jobs_list con su pid.
 *
 * Argumentos:
 *   - pid: número pid del proceso buscado.
 *
 * Salida:
 *   - int: posición del proceso en jobs_list.
 */
int jobs_list_find(pid_t pid) {
    for (int i = 1; i <= n_job; i++) {
        if (jobs_list[i].pid == pid) {
            return i;
        }
    }
    return -1;
}

/**
 * Función: void ctrlc(int signum)
 * -----------------------------------------------------------------------------
 * Descrpción:
 *   Manejador propia para la señal SIGINT (ctrl + c).
 *
 * Argumentos:
 *   - signum: número de la señal recibida
 *
 * Salida:
 *   - void
 */
int jobs_list_remove(int pos) {
    if (pos <= 0 && pos > n_job) {
        fprintf(stderr,
                ROJO_T "Error jobs_list_remove(): Fuera del rango\n" RESET);
        return -1;
    }
    if (pos == n_job) {
        jobs_list[pos].pid = 0;
        jobs_list[pos].estado = 'F';
        strcpy(jobs_list[pos].cmd, "");
    } else {
        jobs_list[pos].pid = jobs_list[n_job].pid;
        jobs_list[pos].estado = jobs_list[n_job].estado;
        strcpy(jobs_list[pos].cmd, jobs_list[n_job].cmd);
    }
    n_job--;
    return 0;
}

/**
 * Función: int is_output_redirection(char **args)
 * -----------------------------------------------------------------------------
 * Descrpción:
 *   Función que cambia la salida estándard stdout.
 *
 * Argumentos:
 *   - args: argumento del comando.
 *
 * Salida:
 *   - int: 1 redireccionamiento hecho, 0 no redireccionamiento.
 */
int is_output_redirection(char **args) {
    // Buscamos '>'
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0 && args[i + 1] != NULL) {
            args[i] = NULL;
            int fichStdout = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC,
                                  S_IRUSR | S_IWUSR);
            dup2(fichStdout, 1);
            close(fichStdout);
            return 1;
        }
    }
    return 0;
}

/**
 * Función: int imprimir_prompt()
 * -----------------------------------------------------------------------------
 * Descrpción:
 *   Imprime el prompt del MiniShell con los datos del usuario y el
 * directorio actual.
 *
 * Salida:
 *   - int: 0 salida exitosa.
 */
int imprimir_prompt() {
    // Vaciado del buffer
    sleep(1);
    if (fflush(stdout) != 0) {
        fprintf(stderr, ROJO_T "Error imprimir_prompt(): fflush() \n" RESET);
    }
    char *cwd = malloc(COMMAND_LINE_SIZE * sizeof(*cwd));
    // Imprimimos el prompt personalizado
    printf(VERDE_T ERASE_LINE "\r%s" BLANCO_T ":" CYAN_T "%s" BLANCO_T
                              "%c " RESET,
           getenv("USER"), getcwd(cwd, COMMAND_LINE_SIZE), PROMPT);
    free(cwd);
    return 0;
}

/**
 * Función: int internal_cd_avanzado(char **args)
 * -----------------------------------------------------------------------------
 * Descrpción:
 *   Extensión de internal_cd() para tratar casos que se emplea \', \" y \\
 *
 * Argumentos:
 *   - args: argumentos de la consola.
 * Salida:
 *   - int: 0 salida exitosa, -1 salida fallida.
 */
int internal_cd_avanzado(char **args) {
    bool error = false;
    // Crear un charArray vacio para guardar los argumentos
    char *const path = calloc((COMMAND_LINE_SIZE + 1), sizeof(*path));

    int i = 1;  // saltarse el "cd"
    strcat(path, args[i++]);
    for (; args[i] != NULL; i++) {
        strcat(path, " ");
        strcat(path, args[i]);
    }

    // Buscar caracter especial
    char *ptrPath;
    bool pathTratado = false;
    ptrPath = strchr(path, '\'');
    if (ptrPath && !pathTratado) {
        // Caso comilla simple
        ptrPath++;
        strcpy(path, ptrPath);
        ptrPath = strchr(path, '\'');
        if (!ptrPath) {
            fprintf(stderr, ROJO_T
                    "Error internal_cd_avanzado(): falta segunda \\\'\n" RESET);
            error = true;
        }
        *ptrPath = '\0';
        pathTratado = true;
    }
    ptrPath = strchr(path, '\"');
    if (ptrPath && !pathTratado) {
        // Caso comilla doble
        ptrPath++;
        strcpy(path, ptrPath);
        ptrPath = strchr(path, '\"');
        if (!ptrPath) {
            fprintf(stderr, ROJO_T
                    "Error internal_cd_avanzado(): falta segunda \\\"\n" RESET);
            error = true;
        }
        *ptrPath = '\0';
        pathTratado = true;
    }
    ptrPath = strchr(path, '\\');
    if (ptrPath && !pathTratado) {
        // Caso barra
        // strcat(path, "\\");
        // while (ptrPath) {
        //     *ptrPath = '\0';
        //     ptrPath++;
        //     strcat(path, ptrPath);
        //     ptrPath = strchr(ptrPath, '\\');
        // }
        ptrPath = path;
        char *ptrPathAux = path;
        while (*ptrPath != '\0') {
            if (*ptrPath == '\\') {
                ptrPath++;
            } else {
                *ptrPathAux = *ptrPath;
                ptrPath++;
                ptrPathAux++;
            }
        }
        *ptrPathAux = '\0';
        pathTratado = true;
    }
    if (!error) {
        if (chdir(path)) {
            fprintf(stderr, ROJO_T
                    "Error internal_cd_avanzado(): chdir(): No such file or "
                    "directory\n" RESET);
            error = true;
        }
    }
    free(path);
    return error ? -1 : 0;
}
