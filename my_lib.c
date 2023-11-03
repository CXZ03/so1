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
    struct my_stack *Pila = malloc(sizeof(struct my_stack));
    Pila->size=size;
    Pila->top=NULL;
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
    struct my_stack_node *nodo;
    nodo = malloc(sizeof(struct my_stack_node));
     nodo->data = data;
    
    if(stack->top == NULL){
        stack->top = nodo;
        nodo->next = NULL;
    }else{
        nodo->next = stack->top;
        stack->top = nodo;
    }
    return 0;
}


void *my_stack_pop (struct my_stack *stack){
    struct my_stack_node *nodo = stack->top;
    void *datos = NULL;
    // Antes de sacar ningún elemento de la pila, comprueba que no esté vacía.
    if (nodo == NULL) {
        return NULL; // La pila está vacía.
    }

    stack->top = nodo->next;
    datos = nodo->data;

    //Liberar el espacio en el que estaba el nodo que acabamos de sacar.
    free(nodo);

    return datos;
}

int my_stack_len (struct my_stack *stack){
    int contador=0;
    struct my_stack_node *nodo = stack->top;
    while(nodo!=NULL){
        contador++;
        nodo=nodo->next;
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
   int bytes_liberados = 0;    /* contador de espacio liberado */
    struct my_stack_node *node; /* nodo auxiliar */

    while (stack->top != NULL) /* recorremos toda la pila hasta liberar el último */
    {
        bytes_liberados += sizeof(*node);   /* tamaño de los nodos */
        bytes_liberados += stack->size;     /* tamaño de los datos */
        free(my_stack_pop(stack));          /* aprovechasmos la función my_stack_pop para ir liberando el top de ada iteración y así toda la pila */
    }
    free(stack); /* liberamos la memoria que contiene la pila */
    bytes_liberados += sizeof(struct my_stack); /* por último los bytes que ocupa el puntero de la pila */
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
//Funcion Auxiliar recursiva para escrribir el fichero
int FAuxiliarRecursiva(struct my_stack_node *nodo, int fich, int sz){
   
    int bytes = 0;
    if (nodo->next)
    {
        
        bytes = FAuxiliarRecursiva(nodo->next, fich, sz);
    }

    
    return write(fich, nodo->data, sz) + bytes;
}

int my_stack_write (struct my_stack *stack, char *filename){

int fich;
int sz;
    struct my_stack_node*nodo;

    nodo=stack->top;
    fich=open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    sz= stack->size;
    //Errores
if(fich==-1){
    return -1;
}
if(write(fich, &stack->size, sizeof(sz)) == -1){
    return -1;
}
 FAuxiliarRecursiva(nodo,fich,sz);
    close(fich);
    return my_stack_len(stack);
}



struct my_stack *my_stack_read (char *filename){
int bytes;
int fich;
int sz;
struct my_stack *pila;
void *datos;

    fich= open(filename, O_RDONLY, S_IRUSR);
//error
    if(fich==-1){
        return NULL;
    }

bytes= read(fich, &sz, sizeof(int));
    if(bytes==-1){
        return NULL;
    }
    pila = my_stack_init(sz);
   datos = malloc(pila->size);
   bytes = read(fich, datos, pila->size);
while (bytes == pila->size){
   my_stack_push(pila,datos);
   datos=malloc(pila->size);
   bytes=read(fich,datos,pila->size);
}
    free(datos);
    close(fich);
    return pila;
}
