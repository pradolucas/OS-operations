#include <stdlib.h> /* system call - exit */
#include <stdio.h> // Standard I/O 
#include<pthread.h> // POSIX Thread 
#include <sys/time.h> // Lib de gettimeofday

int G[20];
float descriptive_stats[4]; 
int n = 20;
int *pg = G;

void print_vec(int *p, int n) {
    printf("\n");
    for(int i=0; i < n; i++){
        printf("%d ", *(p+i));
    }
    printf("\n");
}

void *max_thread(void *p){
    int *cp = (int *) p;
    int max = *cp;
    for(int i=0; i < n; i++){
        if(*(cp+i) > max)
            max = *(cp+i);
    }
    descriptive_stats[0]=max;
    pthread_exit(NULL);
}
void *min_thread(void *p){
    int *cp = (int *) p;
    int min = *cp;

    for(int i=0; i < n; i++){
        if(*(cp+i) < min)
            min = *(cp+i);
    }    
    descriptive_stats[1]=min;
    pthread_exit(NULL);
}

void *mean_thread(void *p){
    float mean, sum=0; 
    int *cp = (int *) p;

    for(int i=0; i < n; i++){
        sum += *(cp+i);
    }    
    mean = sum/n;
    descriptive_stats[2]=mean;
    pthread_exit(NULL);
}
void *mode_thread(void *p){
    int *cp = (int *) p;
    int count, max_count = 0, max_value = *cp;

    for(int i=0; i < n; i++){
        count=0;
        for(int j=i+1; j<n; j++)
            if(*(cp+i) == *(cp+j))
                count+=1;
        if(count > max_count){
            max_value = *(cp+i);
            max_count = count;
        }
    }    
    descriptive_stats[3]=max_value;
    pthread_exit(NULL);
}

int main(){
    struct timeval start_time, final_time;
    gettimeofday(&start_time, NULL);
    
    for(int i = 0; i < n; i++){
        *(pg+i) = rand() % 10 + 1;  // Valores entre 1 e 100
    }
    print_vec(pg, n);

    //thread
    pthread_t thr_max, thr_min, thr_mean, thr_mode;  
    pthread_create(&thr_max, NULL, max_thread, pg);
    pthread_create(&thr_min, NULL, min_thread, pg);
    pthread_create(&thr_mean, NULL, mean_thread, pg);
    pthread_create(&thr_mode, NULL, mode_thread, pg);

    // Join to master thread
    (void) pthread_join(thr_max, NULL);
    (void) pthread_join(thr_min, NULL);
    (void) pthread_join(thr_mean, NULL);
    (void) pthread_join(thr_mode, NULL);    

    printf("Max: %0.2f", descriptive_stats[0]);
    printf("\nMin:  %0.2f", descriptive_stats[1]);
    printf("\nMean: %0.2f", descriptive_stats[2]);
    printf("\nMode: %0.2f", descriptive_stats[3]);

    gettimeofday(&final_time, NULL);
    printf("\nseconds : %ld, micro seconds : %ld\n",
    final_time.tv_sec-start_time.tv_sec, final_time.tv_usec-start_time.tv_usec);

    return 0;
}
