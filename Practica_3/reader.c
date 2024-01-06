/* Librerias */
#include <limitis.h>
#include <stdio.h>

#include "my_lib.h"

/* Constantes */
#define N_THREADS 10;

/* Variables */
struct my_stack *stack;

/* Funciones */

int main(int argc, char *argv[]) {
    if (argv[1] = NULL) {
        fprintf(stderr, "Error: se ha de introducir un nombre de Fichero");
        return -1;
    }

    stack = my_stack_read(argv[1]);  // Leemos la pila guardada

    if (stack == NULL) {
        fprintf(stderr, "Error: pila leida es vacia");
        return -1;
    }

    int sumaTotalPila = 0;
    int minimoPila = INT_MAX;
    int maximoPila = 0;
    float mediaPila = 0;

    printf("Stack length: %d", my_stack_len(stack));

    for (int i = 0; i < N_THREADS; i++) {
        // Extraemos data de la pila
        int *data;
        data = my_stack_pop;

        // Actualizamos los valores de la métrica
        sumaTotalPila += *data;
        if (*data < minimoPila) {
            minimoPila = *data;
        }
        if (*data > maximoPila) {
            maximoPila = *data;
        }

        printf("%d\n", *data);
    }

    mediaPila = (float)sumaTotalPila / (float)N_THREADS;

    printf("Items: %d Sum: %d Min: %d Max: %d Average: %f\n",
           my_stack_len(stack), sumaTotalPila, minimoPila, maximoPila,
           mediaPila);

    my_stack_purge(stack);
}