#include "my_lib.h"

/* SEMANA 4 */

/**
 * Función: my_strlen
 * ------------------
 * Cuenta la longitud de un String.
 *
 * Parámetros:
 * -    const char *str: el puntero al String que contar.
 *
 * Salida:
 * -    size_t: longitud del String.
 */
size_t my_strlen(const char *str)
{
    size_t len = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        len++;
    }
    return len;
}
/**
 * Función: my_strcmp
 * ------------------
 * Compara las cadenas apuntadas por str1 y str2.
 *
 * Parámetros:
 * -    const char *str1: el puntero al String 1 que comparar.
 *
 * -    const char *str2: el puntero al String 2 que comparar.
 *
 * Salida:
 * -    int: la diferencia ASCII que hay entre el primer carácter distinto entre
 *      los dos Strings. Positivo si el String 1 es mayor que el String 2, de lo
 *      contrario, negativo, y 0 en el caso de que sean iguales.
 */
int my_strcmp(const char *str1, const char *str2)
{
    int i = 0;
    while ((str1[i] == str2[i]) & (str1[i] != '\0') & (str2[i] != '\0'))
    {
        i++;
    }
    if (str1[i] == str2[i])
    {
        return 0;
    }
    else if (str1[i] > str2[i])
    {
        return str1[i] - str2[i];
    }
    else
        return str1[i] - str2[i];
}
/**
 * Función: my_strcpy
 * ------------------
 * Copia todo el contenido de src en la memoria apuntada por dest.
 *
 * Parámetros:
 * -    char *dest: el puntero que apunta la zona de memoria donde se guarda
 *      la copia.
 *
 * -    const char *src: el puntero que apunta el String que copiar.
 *
 * Salida:
 * -    char *: devuelve el puntero que apunta la zona con la copia
 */
char *my_strcpy(char *dest, const char *src)
{
    int i;
    for (i = 0; src[i] != '\0'; i++)
    {
        dest[i] = src[i];
    }
    dest[i] = '\0';
    return dest;
}
/**
 * Función: my_strncpy
 * ------------------
 * Copia n caracteres de src en la memoria apuntada por dest.
 *
 * Parámetros:
 * -    char *dest: el puntero que apunta la zona de memoria donde se guarda
 *      la copia.
 *
 * -    const char *src: el puntero que apunta el String que copiar.
 *
 * Salida:
 * -    char *: devuelve el puntero que apunta la zona con la copia
 */
char *my_strncpy(char *dest, const char *src, size_t n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        dest[i] = src[i];
    }
    // dest[i] = '\0'; por si hay que poner nulo al final
    return dest;
}

/**
 * Función: my_strcat
 * ------------------
 * Concatena el contenido de src a dest.
 *
 * Parámetros:
 * -    char *dest: el puntero que apunta la zona de memoria donde se guarda
 *      el resultado.
 *
 * -    const char *src: el puntero que apunta el String que va ser concatenado.
 *
 * Salida:
 * -    char *: devuelve el puntero que apunta la zona de memoria con
 *      el resultado.
 */
char *my_strcat(char *dest, const char *src)
{
    // a nos guiará a colocarnos al final de dest
    char *dest1 = dest;
    while (*dest1 != '\0')
    {
        dest1++;
    }
    int i;
    for (i = 0; src[i] != '\0'; i++)
    {
        *dest1++ = src[i];
    }
    *dest1 = '\0';
    return dest;
}
/**
 * Función: my_strchr
 * ------------------
 * Busca el primer carácter en el String que coincide con c
 * y devuelve este mismo.
 *
 * Parámetros:
 * -    const char *str: el puntero que apunta al String donde buscamos
 *      el carácter.
 *
 * -    int c: el carácter en número ASCII que buscamos.
 *
 * Salida:
 * -    char *: devuelve el puntero que apunta la zona de memoria del carácter.
 */
char *my_strchr(const char *str, int c)
{
    while (*str != '\0')
    {
        if (*str == (char)c)
        {
            return (char *)str;
        }
        str++;
    }
    return NULL;
}

/* SEMANA 5-6 */

/**
 * Función: my_stack_init
 * ----------------------
 * Inicializa el struct my_stack con el tamaño de los datos de la pila.
 *
 * Parámetros:
 * -    int size: tamaño de los datos de cada nodo de la pila.
 *
 * Salida:
 * -    struct my_stack *: devuelve el puntero a la pila inicializada.
 */
struct my_stack *my_stack_init(int size)
{
    struct my_stack *Pila = malloc(sizeof(struct my_stack));
    Pila->size = size;
    Pila->top = NULL;
    return Pila;
}

/**
 * Función: my_stack_push
 * ----------------------
 * Inserta un nuevo nodo con los datos introducidos seguido del top de la pila.
 *
 * Parámetros:
 * -    struct my_stack *stack: pila donde insertar el nuevo nodo.
 *
 * -    void *data: dato que guardar en el nodo que será introducido a la pila.
 *
 * Salida:
 * -    int: 0 si ha ejecutado con éxito, de lo contrario -1.
 */
int my_stack_push(struct my_stack *stack, void *data)
{
    // Realiza las comprobaciones necesarias sobre la pila.
    if (stack == NULL)
    {
        return -1; // La pila no existe
    }
    if (stack->size <= 0)
    {
        return -1; // La pila no tiene un tamaño suficiente.
    }
    struct my_stack_node *nodo;
    nodo = malloc(sizeof(struct my_stack_node));
    nodo->data = data;

    if (stack->top == NULL)
    {
        stack->top = nodo;
        nodo->next = NULL;
    }
    else
    {
        nodo->next = stack->top;
        stack->top = nodo;
    }
    return 0;
}

/**
 * Función: my_stack_pop
 * ---------------------
 * Devuelve el top del stack, y lo elimina de la pila.
 *
 * Parámetros:
 * -    struct my_stack *stack: pila donde sacar el nodo.
 *
 * Salida:
 * -    void *: devuelve el nodo sacado.
 */
void *my_stack_pop(struct my_stack *stack)
{
    struct my_stack_node *nodo = stack->top;
    void *datos = NULL;
    // Antes de sacar ningún elemento de la pila, comprueba que no esté vacía.
    if (nodo == NULL)
    {
        return NULL; // La pila está vacía.
    }

    stack->top = nodo->next;
    datos = nodo->data;

    // Liberar el espacio en el que estaba el nodo que acabamos de sacar.
    free(nodo);

    return datos;
}

/**
 * Función: my_stack_len
 * ---------------------
 * Devuelve el número de nodos recorriendo toda la pila.
 *
 * Parámetros:
 * -    struct my_stack *stack: pila donde contar los nodos.
 *
 * Salida:
 * -    int: cantidad de nodos que hay en la pila.
 */
int my_stack_len(struct my_stack *stack)
{
    int contador = 0;
    struct my_stack_node *nodo = stack->top;
    while (nodo != NULL)
    {
        contador++;
        nodo = nodo->next;
    }
    return contador;
}

/**
 * Función: my_stack_purge
 * ---------------------
 * Elimina todos los nodos en la pila, y el puntero de la pila.
 *
 * Parámetros:
 * -    struct my_stack *stack: pila que eliminar.
 *
 * Salida:
 * -    int: cantidad de bytes liberados.
 */
int my_stack_purge(struct my_stack *stack)
{
    int bytes_liberados = 0;    /* contador de espacio liberado */
    struct my_stack_node *node; /* nodo auxiliar */

    /* recorremos toda la pila hasta liberar el último */
    while (stack->top != NULL)
    {
        bytes_liberados += sizeof(*node); /* tamaño de los nodos */
        bytes_liberados += stack->size;   /* tamaño de los datos */

        /* aprovechasmos la función my_stack_pop para ir liberando el top de
        cada iteración y así toda la pila */
        free(my_stack_pop(stack));
    }
    free(stack); /* liberamos la memoria que contiene la pila */

    /* por último los bytes que ocupa el puntero de la pila */
    bytes_liberados += sizeof(struct my_stack);
    return bytes_liberados;
}

/**
 * Función: FAuxiliarRecursiva
 * -----------------------
 * Devuelve la cantidad de bytes escritos.
 *
 * Parámetros:
 * -    struct my_stack_node *nodo: nodo donde queremos empezar a escribir.
 *
 * -    int fich: identificador del fichero donde escribimos.
 *
 * -    int sz: tamaño de cada escritura.
 *
 * Salida:
 * -    int: cantidad de bytes escritos.
 */
int FAuxiliarRecursiva(struct my_stack_node *nodo, int fich, int sz)
{
    int bytes = 0;

    /* Hacemos la llamada recursiva antes del write para que empiece a escribir 
    en la parte inferior de la pila*/
    if (nodo->next)
    {
        bytes = FAuxiliarRecursiva(nodo->next, fich, sz);
    }

    return write(fich, nodo->data, sz) + bytes;
}

/**
 * Función: my_stack_write
 * -----------------------
 * Primero guarda el tamaño de los datos de la pila en un int al inicio del
 * fichero. Luego guarda todos los datos de la pila desde abajo hasta top de
 * la pila dentro del fichero.
 *
 * Parámetros:
 * -    struct my_stack *stack: pila de la que copiar.
 *
 * -    char *filename: fichero al cual copiar.
 *
 * Salida:
 * -    int: cantidad de elementos copiados.
 */
int my_stack_write(struct my_stack *stack, char *filename)
{
    /* Control de error del stack */
    if (stack == NULL)
    {
        fprintf(stderr, "Error: Pila inexistente");
        return -1;
    }

    /* Control de error del top de la pila */
    if (stack->top == NULL)
    {
        fprintf(stderr, "Error: Pila vacia");
        return -1;
    }

    int fich;
    int sz;
    struct my_stack_node *nodo;

    nodo = stack->top;
    fich = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    sz = stack->size;

    /* Control de open() */
    if (fich == -1)
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return -1;
    }

    /* Escritura de tamaño de los datos en un int */
    if (write(fich, &stack->size, sizeof(sz)) == -1)
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return -1;
    }

    /* Escritura con una función recursiva de los datos restantes */
    FAuxiliarRecursiva(nodo, fich, sz);
    if (close(fich) == -1)
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return -1;
    }

    return my_stack_len(stack);
}

/**
 * Función: my_stack_read
 * -----------------------
 * Primero lee el tamaño de los datos del fichero que está al inicio.
 * Luego lee todos los datos del fichero y los guarda en una pila nueva.
 *
 * Parámetros:
 * -    char *filename: fichero del cual copiar.
 *
 * Salida:
 * -    struct my_stack *: puntero a la pila.
 */
struct my_stack *my_stack_read(char *filename)
{
    int bytes;
    int fich;
    int sz;
    struct my_stack *pila;
    void *datos;

    fich = open(filename, O_RDONLY, S_IRUSR);
    /* Control de open() */
    if (fich == -1)
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return NULL;
    }

    bytes = read(fich, &sz, sizeof(int)); /* leemos el tamaño de los datos*/
    if (bytes == -1)
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return NULL;
    }
    
    /* Pila donde guardar los datos leidos */
    pila = my_stack_init(sz);   

    /* Reserva espacio donde guardar cada lectura */   
    datos = malloc(pila->size);   
    bytes = read(fich, datos, pila->size); /* Leemos los datos epicamente */
    while (bytes == pila->size)
    {
        my_stack_push(pila, datos);
        datos = malloc(pila->size);
        bytes = read(fich, datos, pila->size);
    }
    free(datos); /* liberamos la memoria del espacio auxiliar */
    close(fich);
    return pila;
}
