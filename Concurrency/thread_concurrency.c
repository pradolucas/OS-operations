#include<stdio.h>
#include<pthread.h> // POSIX Thread 
#include<string.h>

#define MAX 10;
int RECURSO_MAXIMO = MAX
typedef struct __lock_t { int flag; } lock_t;

void init(lock_t *mutex){
    /* 0 -> lock is available
       1 -> held  */ 
    mutex->flag=0;
}
int CompareAndSwap(int *ptr, int expected, int new){
    int actual = *ptr;
    if(actual == expected)
        *ptr = new;
    return actual;
}
void lock(lock_t *mutex){
    while(CompareAndSwap(&mutex->flag, 0, 1) == 1);
}
void unlock(lock_t *mutex){
    mutex->flag = 0;
}


int aloca_recurso (int recurso_desejado){
    if (RECURSO_MAXIMO < recurso_desejado)
        return -1;
    else{
        RECURSO_MAXIMO -= recurso_desejado;
        return 0;
    }
}
int desaloca_recurso (int recurso_devolvido){
    RECURSO_MAXIMO += recurso_devolvido;
    return 0;
}

void *call (void *t){
    int thread = (int *) t;
    int wanted=2, recs=0;
    lock_t MUTEX;
    init(&MUTEX);

    for (int i=0; i<3; i++){
        //CRITICAL REGION
        lock(&MUTEX);
        if(aloca_recurso(wanted) == 0){
            recs += wanted;
        }
        //CRITICAL REGION
        unlock(&MUTEX);
    }
    
    //CRITICAL REGION
    lock(&MUTEX);
    desaloca_recurso(recs);
    unlock(&MUTEX);
    //CRITICAL REGION

    pthread_exit(NULL);
}


int main(){
    int nthreads = 3, i=0, err;
    pthread_t thr[nthreads];

    while(i < nthreads){
        err = pthread_create(&thr[i], NULL, call, (void *) i);
        if (err != 0)
            printf("\nErro ao criar a thread:[%s]", strerror(err));
        i++;
    }

    // Join to master thread
    for(int i = 0; i < nthreads; i++){
        (void) pthread_join(thr[i], NULL);
    }

    return 0;
}