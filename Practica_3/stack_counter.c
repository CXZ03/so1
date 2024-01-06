/* Librerias */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "my_lib.c"

/* Constantes */
#define N_THREADS 10;
#define N_ITERATION 1000000;

/* Variables */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct my_stack *stack;

/* Funciones */
void *worker(void *ptr);

int main(int arg_cont, char *argv[]) {
    int *data = 0;
    // Tenemos que verificar que existe
    if (argv[1] = NULL) {
        fprintf(stderr, "Error: se ha de introducir un nombre de Fichero");
        return -1;
    }

    stack = my_stack_read(argv[1]);  // Leemos la pila guardada

    // Inicializamos a 0 en caso de no estar inicializada
    if (stack = NULL) {
        stack = my_stack_init(1);
        for (int i = my_stack_len(stack); i < 10; i++) {
            data = malloc(sizeof(int));
            *data = 0;
            my_stack_push(stack, data);
        }
    }

    // Si la pila tiene menos de 10, agregar restantes apuntando a 0
    if (my_stack_len(stack) < 10) {
        for (int i = my_stack_len(stack); i < 10; i++) {
            data = malloc(sizeof(int));
            *data = 0;
            my_stack_push(stack, data);
        }
    }

    // Si tiene mas de 10 se ignoran pero no se eliminan

    // Creamos los hilos
    pthread_t threads[N_THREADS];
    for (int i = 0; i < N_THREADS; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    if (pthread_join() != 0) {
        fprintf(stderr, "Error: pthread_join()");
    }
    my_stack_write(stack, argv[1]);
    my_stack_purge(stack);
    pthread_exit(NULL);
}

void *worker(void *ptr) {
    for (int i = 0; i < N_ITERATION; i++) {
        pthread_mutex_lock(&mutex);        // Entramos en zona crítica
        void *data = my_stack_pop(stack);  // Sacamos dato del stack
        my_stack_push(stack, ++(*data));   // Insertamos dato incrementando 1
        pthread_mutex_unlock(&mutex);      // Salimos de la zona crítica
        pthread_exit(NULL);                // Acabamos la función
    }
}
