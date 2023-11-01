#include "my_lib.h"

size_t my_strlen(const char *str){
    size_t len = 0;
    for(int i=0;str[i]!='\0';i++){
    len++;
    }
    return len;
}
//compara caracter a caracter hasta encontrar uno difernte y calcula la diferncia
int my_strcmp(const char *str1, const char *str2){
    int i=0;
    while((str1[i]==str2[i])&(str1[i] != '\0' )&(str2[i] != '\0')){
       i++; 
    }
    if(str1[i]==str2[i]){
        return 0;
    }else if(str1[i]>str2[i]){
        return str1[i]-str2[i];
    }else 
     return str1[i]-str2[i];
}
//Copia el contenido de src a dest
char *my_strcpy(char *dest, const char *src){
     int i;
    for (i=0;src[i]!='\0';i++){
        dest[i] = src[i];
    }
    dest[i] = '\0';
    return dest;
}
//copia  el numero de caracters en dest pasado por size_t n
char *my_strncpy(char *dest, const char *src, size_t n){
   int i;
    for (i=0;i<n;i++){
        dest[i] = src[i];
    }
    //dest[i] = '\0'; por si hay que poner nulo al final
    return dest;
}

//Copia src a partir del unlo de dest
char *my_strcat(char *dest, const char *src){
    //a nos guiará a colocarnos al final de dest
    char *dest1= dest;
    while(*dest1!='\0'){
        dest1++;
    }
    int i;
    for (i=0;src[i]!='\0';i++){
        *dest1++ = src[i];
        
    }
    *dest1 = '\0';
    return dest;
    
}
//devuelve el punter al encontrar el caracter
char *my_strchr(const char *str, int c){
     
   while(*str!='\0'){
       if(*str==(char)c){
           return (char *)str;

       }
       str++;
   }
   return NULL;
}

////////////////////////////Parte trabajo 1B
//reserva un espacio de memoria de la pila 
//devolviendo el puntero de este
struct my_stack *my_stack_init (int size){
    my_stack Pila;
    Pila.size = size;
    Pila.*top= NULL;
    return Pila;

}

int my_stack_push (struct my_stack *stack, void *data){

    // Realiza las comprobaciones necesarias sobre la pila.
    if (stack==NULL) {
        return -1; // La pila no existe
    }
    if (stack->size<=0) {
        return -1; // La pila no tiene un tamaño suficiente.
    }

    // Crea un nuevo nodo para contener los datos
    struct my_stack_node *nuevo_nodo = (struct my_stack_node *)malloc(sizeof(struct my_stack_node));
    if (nuevo_nodo == NULL) {
        return -1; // Error de asignación de memoria
    }

    // Asigna los datos al nuevo nodo
    nuevo_nodo->data = data;

    // Conecta el nuevo nodo al superior.
    nuevo_nodo->next = stack->top;

    // Actualiza el puntero "top" para que apunte al nuevo nodo
    stack->top = nuevo_nodo;

    return 0; // Éxito
}


void *my_stack_pop (struct my_stack *stack){

    // Antes de sacar ningún elemento de la pila, comprueba que no esté vacía.
    if (stack->top == NULL) {
        return NULL; // La pila está vacía.
    }

    // Guardar el puntero a los datos del nodo superior.
    void *datos = stack->top->data;

    // Guardar el puntero al nodo superior de la pila.
    struct my_stack_node *temporal = stack->top;

    // Actualizar el puntero "top" para que apunte al siguiente nodo de la pila.
    stack->top = stack->top->next;

    //Liberar el espacio en el que estaba el nodo que acabamos de sacar.
    free(temporal);

    return datos;
}

int my_stack_len (struct my_stack *stack){
    int contador=0;
    void *aux = stack->top;
    while((aux->next) != NULL){
        aux = aux->next;
        contador++;
    }
    return contador;
}

//Este código es el de arriba pero menos optimizado, 
//en caso que el de arriba no funcione miramos este, 
//si el de arriba funciona, este debe ser eliminado.
//
//int my_stack_len (struct my_stack *stack){
//    if(stack->top == NULL){
//        return 0;
//    }
//    int contador=1;
//    void *aux = stack->top->next;
//    while((aux->next) != NULL){
//        aux = aux->next;
//        contador++;
//    }
//    return contador;
//}

int my_stack_purge (struct my_stack *stack){
    int bytes_liberados=0;
    void *aux;
    while(stack->top!=NULL){
        aux = stack->top;
        stack->top=stack->top->next;
        free(aux);
        bytes_liberados+=16;
    }
    free(stack);
    bytes_liberados+=16;
    return bytes_liberados;
}

//Este método es el mismo que el anterior pero  utilizando my_stack_pop 
//en lugar de hacer las operaciones dentro del mismo metodo.
//int my_stack_purge (struct my_stack *stack){
//    int bytes_liberados=0;
//    while(stack->top!=NULL){
//        my_stack_pop(stack);
//        bytes_liberados+=16;
//    }
//    free(stack);
//    bytes_liberados+=16;
//    return bytes_liberados;
//}

int my_stack_write (struct my_stack *stack, char *filename){

}

struct my_stack *my_stack_read (char *filename){

}

