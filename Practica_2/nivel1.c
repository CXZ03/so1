/* Practica 2: Nivel 1 */
/* Autores: Guillem, Elena, Xiaozhe */

/* Declaración de debugs */
#define DEBUG_1 1;

/* Declaración de librerias */
#include <stdio.h>

/* Declaración de constantes */
#define COMMAND_LINE_SIZE 1024;     // tamaño máximo de linea de commando
#define ARGS_SIZE 64;               // tamaño máximo de argumentos

/* Declaración de Funciones */
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

/* Declaración de funciones adicionales */
int imprimir_prompt();

/* Declaración de variables */
char line[COMMAND_LINE_SIZE];       // almacena la línea de comando
char *args[ARGS_SIZE];              // almacena los argumentos disponibles

int main(){
    while(true){
        if(read_line(line)){
            execute_line(line);
        }
    }
}

char *read_line(char *line){
    if(fgets(line, sizeof(line), stdin) == NULL){       // leemos la linea y
        fprintf(stderr, "Error al leer la línea.\n")    // comprobamos error
    }else{
        
    }

    return line;
}

int imprimir_prompt(){
    #if DEBUG_1
    printf("llamada a la función: imprimir_prompt()\n");
    #endif

    return 0;
}


