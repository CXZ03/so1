/* Practica 2: Nivel 2*/
/* Autores: Guillem, Elena, Xiaozhe */

/* Declaración de debugs */
#define DEBUG_1 0
#define DEBUG_0 0
#define DEBUG_4 0
#define DEBUG_5 0
#define DEBUG_6 1
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

#define COMMAND_LINE_SIZE 1024  // tamaño máximo de lin de commando
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
static int NJobs;

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
    char *lin = malloc(sizeof(char) * COMMAND_LINE_SIZE);

    if (lin == NULL) {
        fprintf(stderr, "No hay memoria.\n");
    }

    int NArgs = 0;

    for (int i = 0; args[i]; i++) {
        strcat(lin, " ");
        strcat(lin, args[i]);
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
    free(lin);
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
    char *lin = (char *)malloc(sizeof(char) * COMMAND_LINE_SIZE);
    // abrimos el archivo
    if (lin) {
        FILE *fich = fopen(args[1], "r");
        if (fich) {
            while (fgets(lin, COMMAND_LINE_SIZE, fich)) {
                // pasamos las lineas del fichero a execute_line
                execute_line(lin);
                fflush(fich);
            }
            fclose(fich);
            free(lin);
            return EXIT_SUCCESS;
            // error al abrir fichero
        } else {
            perror("Error");
            fclose(fich);
            free(lin);
        }
        fclose(fich);
        free(lin);
    }
    return EXIT_FAILURE;

#if DEBUG_0
    printf(
        "[internal_source() → Esta función ejecutará un fichero de líneas de "
        "comandos]\n");
#endif

    return 1;
}

/**
 * Función: int internal_jobs()
 * --------------------------------------
 *
 * Descripción:
 * Recorrerá jobs_list[] imprimiendo por pantalla el identificador de trabajo
 * entre corchetes  (a partir del 1), su PID, la línea de comandos y el estado
 * (D de Detenido, E de Ejecutando). Importante formatear bien los datos con
 * tabuladores y en el mismo orden que el job del Bash.
 *
 * Argumentos:
 *
 * Salida:
 *      - int: 0 salida exitosa, -1 salida fracasada
 */
int internal_jobs(char **args) {
#if DEBUG_1
    printf(
        "[internal_jobs()→ Esta función mostrará el PID de los procesos que no "
        "estén en foreground]\n");
#endif
    // Recorremos todo el job_list e imprimimos info de cada trabajo
    for (int i = 1; i <= NJobs; i++) {
        printf("[%d] %d\t%c\t%s\n", i, jobs_list[i].pid, jobs_list[i].estado,
               jobs_list[i].cmd);
    }
    return 0;
}

int internal_fg(char **args) {
#if DEBUG_1
    printf(
        "[internal_fg()→ Esta función enviará un trabajo detenido al "
        "foreground reactivando su ejecución, o uno del background al "
        "foreground ]\n");
#endif
    if (args[1]) {
        // Extraemos el numero del trabajo que esta en args
        int pos = 0;
        for (int i = 0; args[1][i] != '\0'; i++) {
            pos *= 10;
            job_index += (int)args[1][i] - 48;
        }

        if (pos > NJobs && pos < 0) {
#if DEBUG_6
            fprintf(stderr, "Error: El trabajo %d no existe\n", pos);
#endif
        } else {
            // Activamos el trabajo
            if (jobs_list[pos].estado == 'D') {
                kill(jobs_list[pos].pid, SIGCONT);
            }
            char *ptrAux strchr(jobs_list[pos].cmd, '&');
            *ptrAux = '\0';
            // Actualizamos el foreground
            jobs_list[0].pid = jobs_list[pos].pid;
            jobs_list[0].estado = jobs_list[pos].estado;
            strcpy(jobs_list[0].cmd, jobs_list[pos].cmd);
            jobs_list_remove(pos);
#if DEBUG_6
            printf("%s\n", jobs_list[0].cmd);
#endif
            while (jobs_list[0].pid) {
                pause();
            }
            return 0;
        }
    }
#if DEBUG_6
    fprintf(stderr, "Error: fg sin argumentos");
#endif
    return -1;
}

int internal_bg(char **args) {
#if DEBUG_1
    printf(
        "[internal_bg()→ Esta función reactivará un proceso detenido para que "
        "siga ejecutándose pero en segundo plano]\n");
#endif
    return 0;
}

/**
 * Función: int jobs_list_remove(int pos)
 * --------------------------------------
 *
 * Descripción:
 * Recibe como parámetro la posición del array del trabajo que hay que eliminar
 * y mueve el registro del último proceso de la lista a la posición del que
 * eliminamos. Decrementamos la variable global n_job.
 *
 * Argumentos:
 *      - pos: num de pid que va a ser eliminado
 *
 * Salida:
 *      - int: 0 salida exitosa, -1 salida fracasada
 */
int jobs_list_remove(int pos) {
    if (NJobs > 0 && NJobs < N_JOBS) {
        if (pos == NJobs) {
            jobs_list[pos].pid = 0;
            jobs_list[pos].estado = 'F';
            memset(jobs_list[pos].cmd, '\0', COMMAND_LINE_SIZE);
        } else {
            jobs_list[pos].pid = jobs_list[NJobs].pid;
            jobs_list[pos].estado = jobs_list[NJobs].estado;
            strcpy(jobs_list[pos].cmd, jobs_list[NJobs].cmd);
        }
    } else {
        fprintf(stderr, "Error: Fuera del rango");
        return -1;
    }
    return 0;
}

/**
 * Función: int jobs_list_find(pid_t pid)
 * --------------------------------------
 *
 * Descripción:
 * Esta función busca los trabajos en la lista de trabajos jobs_list[]
 *
 * Argumentos:
 *      - pid: num de pid buscado
 *
 * Salida:
 *      - int: la posicion que está el trabajo en el jobs_list[].
 */
int jobs_list_find(pid_t pid) {
    // Buscamos de uno en uno hasta encontrar el trabajo buscado
    for (int i = 1; i <= NJobs; i++) {
        if (jobs_list[i].pid == pid) {
            return i;
        }
    }
    return -1;
}

/**
 * Función: int jobs_list_add(pid_t pid, char status, char *cmd)
 * ----------------------------------------
 *
 * Descripción:
 * Esta función incorpora los trabajos en background en la lista de trabajos
 * jobs_list[] Argumentos:
 *      - pid: num de pid
 *      - status: estado del trabajo
 *      - cmd
 *
 * Salida:
 *      - int: 1 si localiza el token en args, 0 si no lo localiza.
 */
int jobs_list_add(pid_t pid, char estado, char *command_line) {
    if (NJobs > N_JOBS) {
        fprintf(stderr, "cantidad máxima de trabajo sobrepasado");
        return -1;
    } else {
        NJobs++;
        jobs_list[NJobs].pid = pid;
        jobs_list[NJobs].estado = estado;
        strcpy(jobs_list[NJobs].cmd, command_line);
#if DEBUG_5
        printf("jobs_list_add()-> num pid: %d cmd: %s\n", NJobs,
               jobs_list[NJobs].cmd);
#endif
        return 0;
    }
}
/**
 * Función: int is_background(char **args);
 * ----------------------------------------
 *
 * Descripción:
 * Esta función devueve 1(TRUE) si localiza el token & en args[ ] (comando
 * en background) y 0 (FALSE) en caso contrario (comando en foreground).
 * Sustituirá el token & por NULL para después poder pasarle args[ ] sin & a
 * execvp(). Argumentos:
 *      - args: donde esta guardado el token &
 *
 * Salida:
 *      - int: 1 si localiza el token en args, 0 si no lo localiza.
 */
int is_background(char **args) {
    for (int i = 0; args[i] != NULL; i++) {
#if DEBUG_5
        printf("is_backgroud()-> args: %s\n", args[i]);
#endif
        if (args[i][0] == '&') {
            // caso '&', es un proceso background
            args[i] = NULL;
#if DEBUG_5
            printf("is_backgroud()-> Es backgroud\n");
#endif
            return 1;
        }
    }
    // caso de que no es un backgroud
#if DEBUG_5
    printf("is_backgroud()-> No es backgroud\n");
#endif
    return 0;
}

/**
 * Función: read_line()
 * --------------------
 * Descripción:
 * Lee la lin de comando que ha introducido el usuario por stdin, y cambia
 * el carácter '\n' por '\0'. También imprime el prompt de la consola.
 *
 * con strchr buscamos la primera ocurriencia del salto de lin en line
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
    // fprintf(stderr, AZUL_T "[parse_args()→ token %i: %s]\n"
    // RESET_FORMATO, i, args[i]);
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
    pid_t pid /*, stat*/;
    char command_line[COMMAND_LINE_SIZE];

    // copiamos comando sin '\n'
    strcpy(command_line, line);

    if (parse_args(args, line) > 0) {
        if (check_internal(args)) {
            perror("Error");
            exit(EXIT_FAILURE);
        } else {
#if DEBUG_4
            fprintf(stderr,
                    GRIS "[execute_line()→ PID padre: %d(%s)]\n" RESET_FORMATO,
                    getpid(), args[0]);
#endif
            bool isBackground = is_background(args);
            pid = fork();
            if (pid == 0) {
                // Añadido n4
                signal(SIGCHLD, SIG_DFL);
                signal(SIGINT, SIG_IGN);
                signal(SIGTSTP, SIG_IGN);
#if DEBUG_4
                fprintf(stderr,
                        GRIS
                        "[execute_line()→ PID hijo: %d(%s)]\n" RESET_FORMATO,
                        getpid(), args[0]);
#endif
                if (execvp(args[0], args) == -1) {
                    fprintf(stderr, "%s: no se encontró la orden\n", line);
                    exit(-1);
                }
            } else if (pid > 0) {
                if (isBackground) {
                    jobs_list_add(pid, 'E', command_line);
#if DEBUG_5
                    printf("[%d] %d\t%c\t%s\n", NJobs, jobs_list[NJobs].pid,
                           jobs_list[NJobs].estado, jobs_list[NJobs].cmd);
#endif
                } else {
                    jobs_list[0].pid = pid;
                    jobs_list[0].estado = 'E';
                    strcpy(jobs_list[0].cmd, line);
                }
            }
            while (jobs_list[0].pid > 0) {
                pause();
            }
        }
    }

    return EXIT_SUCCESS;
}
// funcion para enterrar a los hijos
void reaper(int signum) {
    signal(SIGCHLD, reaper);
    pid_t end;
    int stat;
    // si se acaba pone a 0 las variables
    while ((end = waitpid(-1, &stat, WNOHANG)) > 0) {
        // Si es un proceso Background
        if (end == jobs_list[0].pid) {
            jobs_list[0].pid = 0;
            jobs_list[0].estado = 'F';
            memset(jobs_list[0].cmd, '\0', strlen(jobs_list[0].cmd));
        } else {
            int pos = jobs_list_find(end);
            if (WIFEXITED(stat)) {
#if DEBUG_4
                printf("[Proceso hijo %d (ps f) finalizado con exit code %d]\n",
                       end, WEXITSTATUS(stat));
#endif
            } else {
                if (WIFSIGNALED(stat)) {
#if DEBUG_4
                    printf(
                        "[Proceso hijo %d (ps f) finalizado con exit code "
                        "%d]\n",
                        end, WTERMSIG(stat));
#endif
                }
            }
#if DEBUG_5
            printf(
                "[reaper()→ Proceso hijo %d en background (%s) finalizado con "
                "exit code %d]\n",
                end, jobs_list[pos].cmd, signum);
#endif
            jobs_list_remove(pos);
        }
    }
}
// Manejador propio para la señal SIGINT (Ctrl+C)
void ctrlc(int signum) {
    signal(SIGINT, ctrlc);
    // si hay un proceso en el foregrpund
    if (jobs_list[0].pid > 0) {
        // si el proceso no es el del minishell
        if (strcmp(jobs_list[0].cmd, mi_shell)) {
#if DEBUG_4  // señal provisional
            printf("la señal no es del minishell");
#endif
            kill(jobs_list[0].pid, SIGTERM);
        } else {
#if DEBUG_4
            fprintf(stderr,
                    "\nSeñal SIGTERM no enviada debido a que el proceso en "
                    "foreground es el shell");
#endif
        }
    } else {
#if DEBUG_1
        fprintf(stderr,
                "\nSeñal SIGTERM no enviada debido a que no hay proceso en "
                "foreground");
#endif
    }
    printf("\n");
    fflush(stdout);
}

/**
 * Función: void ctrlz(int signum)
 * --------------------
 * Descripción:
 * Suspende el proceso actual y lo situa en Background.
 *
 * Argumentos:
 *      - signum: señal que llama a la función
 *
 * Salida:
 *      - void
 */

void ctrlz(int signum) {
    signal(SIGTSTP, ctrlz);
    pid_t pid = getpid();

#if DEBUG_5
    printf(
        "\n[ctrlz()→ Soy el proceso con PID %d, el proceso en foreground es %d "
        "(%s)]\n",
        pid, jobs_list[0].pid, jobs_list[0].cmd);
#endif
    if (jobs_list[0].pid > 0) {
        if (strcmp(jobs_list[0].cmd, mi_shell) == 0) {
#if DEBUG_5
            printf(
                "[ctrlz() -> Señal %d (SIGTSTP) no enviada debido a que el "
                "proceso en "
                "foreground es un minishell]\n",
                signum);
#endif
        } else {
            // Paramos el proceso Foreground
            kill(jobs_list[0].pid, SIGSTOP);
#if DEBUG_5
            printf(
                "[ctrlz()→ Señal %d (SIGTSTP) enviada a %d (%s) por %d (%s)]\n",
                signum, jobs_list[0].pid, jobs_list[0].cmd, getpid(), mi_shell);
#endif
            // Ponemos el proceso el jobs_list
            jobs_list[0].estado = 'D';
            jobs_list_add(jobs_list[0].pid, jobs_list[0].estado,
                          jobs_list[0].cmd);

            // Reseteamos foreground
            jobs_list[0].pid = 0;
            jobs_list[0].estado = 'N';
            memset(jobs_list[0].cmd, '\0', COMMAND_LINE_SIZE);
        }
    } else {
#if DEBUG_5
        printf(
            "[ctrlz() -> Señal %d (SIGTSTP) no enviada debido a que el "
            "proceso en "
            "foreground es un minishell]\n",
            signum);
#endif
    }

    fflush(stdout);
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
    NJobs = 0;
    memset(line, 0, COMMAND_LINE_SIZE);
    /// señales
    signal(SIGCHLD, reaper);
    signal(SIGINT, ctrlc);
    signal(SIGTSTP, ctrlz);

    while (1) {
        if (read_line(line)) {
            execute_line(line);
        }
    }
    return 0;
}