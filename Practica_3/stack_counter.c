
#define N_THREADS 10;


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int arg_cont, char *argv[])
{
    int *data=0;
//tenemos que verificar que existe 
    if(argv[1]=NULL){
        printf("No existe la pila, hasta luego");
        exit(0);
    }else{
        stack= my_stack_read(argv[1]);
    }
//inicializamos a 0 en caso de no estar inicializada
 if(stack=NULL){
    stack= my_stack_init(1);
    for(int i=my_stack_len(stack);i<10;i++){
            data= malloc(sizeof(int));
            data=0;
            my_stack_push(stack,data);
        }

 }else{
///La pila ya esta inicializada
    //si la pila tiene menos de 10, agregar restantes apuntando a 0
    if(my_stack_len(stack)<10){
        for(int i=my_stack_len(stack);i<10;i++){
            data= malloc(sizeof(int));
            data=0;
            my_stack_push(stack,data);
        }

    }
    // si tiene mas de 10 se ignoran pero no se eliminan
 }

 //creamos los hilos 
  pthread_t threads[N_THREADS];

 for(int i=0;i<N_THREADS;i++){
    pthread_create(&threads[i],NULL,worker,NULL);

 }


}

