#include<stdio.h>
#include<pthread.h> // POSIX Thread 
#include<string.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

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
    printf("\nentrando na Critical Section.....\n");
    while(CompareAndSwap(&mutex->flag, 0, 1) == 1);

}


void unlock(lock_t *mutex){
    mutex->flag = 0;
    printf("\nsaindo da Critical Section.....\n");
}


int aloca_recurso (int recurso_desejado){
    lock_t MUTEX;
    init(&MUTEX);
    lock(&MUTEX);
    //CRITICAL REGION
    if (RECURSO_MAXIMO < recurso_desejado){
        pid_t thr_id = gettid();
        printf("\n  Thread %d tentou pegar %d recursos, total %d.....\n", thr_id, recurso_desejado, RECURSO_MAXIMO);
        unlock(&MUTEX);
        //CRITICAL REGION
        return -1;
    }
    else{   
        RECURSO_MAXIMO -= recurso_desejado;
        pid_t thr_id = gettid();
        printf("\n  Thread %d pegou %d recursos, total %d.....\n",  thr_id, recurso_desejado, RECURSO_MAXIMO);
        unlock(&MUTEX);
        //CRITICAL REGION
        return 0;
    }
}
// Essa funcao devolve uma certa quantidade de recursos alocados
int desaloca_recurso (int recurso_devolvido){
    lock_t MUTEX;
    init(&MUTEX);
    lock(&MUTEX);

    //CRITICAL REGION
    RECURSO_MAXIMO += recurso_devolvido;
    //CRITICAL REGION

    pid_t thr_id = gettid();
    printf("\n  Thread %d devolveu %d recursos, total %d.....\n", thr_id, recurso_devolvido, RECURSO_MAXIMO);
    unlock(&MUTEX);
    return 0;
}

void *call (void *t){
    int thread = (int *) t;
    int recs = 0, returned=0, wanted=5;


    for (int i=0; i<3; i++){
        while(aloca_recurso(wanted) == -1){ // Tenta o recurso até estar disponível; chance de liberar recurso já utilizado
            sleep(2);
            if(rand() % 2 && recs>0){ // rand 0-1 & está utilizando recursos
                desaloca_recurso(recs);
                recs=0;
                returned=1;
            }
        }
        if(~returned)
            recs += wanted;
        
        if(rand() % 2 && recs>0){ // rand 0-1 & está utilizando recursos; chance de liberar recurso já utilizado
            desaloca_recurso(recs);
            recs=0;
        }
    }

    if(recs>0){ // Desaloca restante dos recursos
        desaloca_recurso(recs);
    }

    pthread_exit(NULL);
}


int main(){
    int nthreads = 2;
    pthread_t thr[nthreads];

    
    int i=0, err;
    while(i < nthreads)
    {
        err = pthread_create(&thr[i], NULL, call, (void *) i);
        if (err != 0)
            printf("\nErro ao criar a thread:[%s]", strerror(err));
        i++;
    }

    // Join to master thread
    i=0;
    while(i < nthreads)
    {
        (void) pthread_join(thr[i], NULL);
        i++;
    }
    
    return 0;
}