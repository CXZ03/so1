/* Librerias */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "my_lib.h"

/* Constantes */
#define N_THREADS 10
#define N_ITERATION 1000000

/* Variables */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct my_stack *stack;

/* Funciones */
void *worker(void *ptr);
int imprimirContenidoStack(struct my_stack_node *top);

int main(int arg_cont, char *argv[]) {
    // Tenemos que verificar que existe
    if (argv[1] == NULL) {
        fprintf(stderr, "Error: se ha de introducir un nombre de Fichero\n");
        return -1;
    }

    // Imprimimos los parametros
    printf("Threads: %d, Iterations: %d\n", N_THREADS, N_ITERATION);

    stack = my_stack_read(argv[1]);  // Leemos la pila guardada
    int longitudStack = my_stack_len(stack);

    printf("stack->size: %d\n", stack->size);
    printf("\noriginal stack lenght: %d\n", longitudStack);
    printf("original stack content:\n");
    imprimirContenidoStack(stack->top);

    int *data = 0;
    // Inicializamos a 0 en caso de no estar inicializada
    if (stack == NULL) {
        stack = my_stack_init(1);
        for (int i = my_stack_len(stack); i < 10; i++) {
            data = malloc(sizeof *data);
            *data = 0;
            my_stack_push(stack, data);
        }
    }

    // Si la pila tiene menos de 10, agregar restantes apuntando a 0
    if (my_stack_len(stack) < 10) {
        for (int i = my_stack_len(stack); i < 10; i++) {
            data = malloc(sizeof *data);
            *data = 0;
            my_stack_push(stack, data);
        }
    }

    longitudStack = my_stack_len(stack);
    printf("\nnew stack lenght: %d\n", longitudStack);
    imprimirContenidoStack(stack->top);
    printf("\n");
    // for (int i = 0; i < longitudStack - 1; i++) {
    //     printf("%d\n", *((int *)(stack->top->data++)));
    // }

    // Creamos los hilos
    pthread_t threads[N_THREADS];
    for (int i = 0; i < N_THREADS; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
        printf("%d) Thread %ld created\n", i, threads[i]);
    }

    // Esperar que cada hilo acabe
    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    longitudStack = my_stack_len(stack);
    printf("\nfinal stack lenght: %d\n", longitudStack);
    imprimirContenidoStack(stack->top);
    // for (int i = 0; i < longitudStack - 1; i++) {
    //     printf("%d\n", *((int *)(stack->top->data++)));
    // }

    printf("\nWritten elements from stack to file: %d\n",
           my_stack_write(stack, argv[1]));
    printf("Released bytes: %d\n", my_stack_purge(stack));
    printf("Bye form main\n");
    pthread_exit(NULL);
}

void *worker(void *ptr) {
    for (int i = 0; i < N_ITERATION; i++) {
        // Entramos en zona crítica
        pthread_mutex_lock(&mutex);
        // printf("Soy el hilo %ld ejecutando pop\n", pthread_self());
        void *data = my_stack_pop(stack);  // Sacamos dato del stack
        pthread_mutex_unlock(&mutex);      // Salimos de la zona crítica
        (*((int *)data))++;                // Incrementamos el contenido en 1
        // Entramos en zona crítica
        pthread_mutex_lock(&mutex);
        // printf("Soy el hilo %ld ejecutando push\n", pthread_self());
        my_stack_push(stack, data);    // Insertamos dato incrementado
        pthread_mutex_unlock(&mutex);  // Salimos de la zona crítica
    }
    pthread_exit(NULL);  // Acabamos la función
}

int imprimirContenidoStack(struct my_stack_node *top) {
    if (top == NULL) {
        return 0;
    }
    printf("%d\n", *((int *)top->data));
    return imprimirContenidoStack(top->next);
}
