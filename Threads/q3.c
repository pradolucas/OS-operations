#include <stdlib.h> /* system call - exit */
#include <stdio.h> // Standard I/O 
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

int max(int *p, int n){
    int max = *p;
    for(int i=0; i < n; i++){
        if(*(p+i) > max)
            max = *(p+i);
    }
    descriptive_stats[0]=max;
    // printf("\nMax: %d ", max);
    return max;
}

int min(int *p, int n){
    int min = *p;
    for(int i=0; i < n; i++){
        if(*(p+i) < min)
            min = *(p+i);
    }
    descriptive_stats[1]=min;
    // printf("\nMin:  %d ", min);
    return min;
}

int mean(int *p, int n){
    float mean, sum; 
    sum = 0;
    for(int i=0; i < n; i++){
        sum += *(p+i);
    }    
    mean = sum/n;
    descriptive_stats[2]=mean;
    // printf("\nMean: %d ", mean);
    return mean;
}

int mode(int *p, int n){
    int count, max_count=0, max_value = *p;

    for(int i=0; i < n; i++){
        count=0;
        for(int j=i+1; j<n; j++)
            if(*(p+i) == *(p+j))
                count+=1;
        if(count > max_count){
            max_value = *(p+i);
            max_count = count;
        }
    }    
    descriptive_stats[3]=max_value;
    // printf("\nMode: %d , %d times ", max_value, max_count);
    return max_value;
}

int main(){
    struct timeval start_time, final_time;
    gettimeofday(&start_time, NULL);

    for(int i = 0; i < n; i++){
        *(pg+i) = rand() % 10 + 1;  // Valores entre 1 e 100
    }
    
    print_vec(pg, n);

    // threadless
    max(pg, n);
    min(pg, n);
    mean(pg, n);
    mode(pg, n);

    printf("Max: %0.2f", descriptive_stats[0]);
    printf("\nMin:  %0.2f", descriptive_stats[1]);
    printf("\nMean: %0.2f", descriptive_stats[2]);
    printf("\nMode: %0.2f\n", descriptive_stats[3]);

    gettimeofday(&final_time, NULL);
    printf("\nseconds : %ld, micro seconds : %ld\n",
    final_time.tv_sec-start_time.tv_sec, final_time.tv_usec-start_time.tv_usec);
    return 0;
}
