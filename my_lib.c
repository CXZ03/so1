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
    struct my_stack_node *new_node = (struct my_stack_node *)malloc(sizeof(struct my_stack_node));
    if (new_node == NULL) {
        return -1; // Error de asignación de memoria
    }

    // Asigna los datos al nuevo nodo
    new_node->data = data;

    // Conecta el nuevo nodo al superior.
    new_node->next = stack->top;

    // Actualiza el puntero "top" para que apunte al nuevo nodo
    stack->top = new_node;

    return 0; // Éxito
}
