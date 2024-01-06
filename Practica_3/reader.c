/* Librerias */
#include <limits.h>
#include <stdio.h>

#include "my_lib.h"

/* Constantes */
#define N_THREADS 10;

/* Variables */
struct my_stack *stack;

/* Funciones */

int main(int argc, char *argv[]) {
    if (argv[1] == NULL) {
        fprintf(stderr, "Error: se ha de introducir un nombre de Fichero\n");
        return -1;
    }

    stack = my_stack_read(argv[1]);  // Leemos la pila guardada

    if (stack == NULL) {
        fprintf(stderr, "Error: pila leida es vacia\n");
        return -1;
    }

    int longitudPila = my_stack_len(stack);
    int sumaTotalPila = 0;
    int minimoPila = INT_MAX;
    int maximoPila = 0;
    float mediaPila = 0;

    // Limitamos la longitud de la pila cuando es mayor que 10
    if (longitudPila > 10) {
        longitudPila = 10;
    }

    printf("Stack length: %d\n", longitudPila);

    for (int i = 0; i < longitudPila; i++) {
        // Extraemos data de la pila
        int *data;
        data = (int *)my_stack_pop(stack);

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

    printf("Items: %d Sum: %d Min: %d Max: %d Average: %f\n", longitudPila,
           sumaTotalPila, minimoPila, maximoPila, mediaPila);

    my_stack_purge(stack);
}