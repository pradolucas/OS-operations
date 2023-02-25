#include <sys/wait.h> /* system call - wait */
#include <stdint.h> /* system call - wait */
#include <stdlib.h> /* system call - exit */
#include <unistd.h> /* system call - fork, exec, sleep */
#include <stdio.h>


int G[5] = {0, 1, 2, 3, 4};
int n = 5;
int *pg = G;

void print_vec(int *p, int n) {
    for(int i=0; i < n; i++){
        printf("%d ", *(p+i));
    }
    printf("\n");
}

void sub_vec(int *p, int n) {
    for(int i=0; i < n; i++){
        *(p+i) = *(p+i)-1;
    }
    print_vec(p, n);
}
int main()
   {
        print_vec(pg, n);
        pid_t pid = fork();
        if(pid == 0){
            printf("Child PID: %d  ", getpid ( ));
            sub_vec(pg, n);
            exit(1);
        }
        else if(pid > 0){
            int ret1, status1;
            ret1 = wait(&status1) ;
            if ((status1&255) == 0) {
                printf("Child process exited with %d status\n", WEXITSTATUS(status1));
                printf("Parent PID: %d  ", getpid ( ));
                print_vec(pg, n);
            }
        }
        else{
            perror ("fork");
            exit(EXIT_FAILURE);
        }
        
        return 0;
   }
