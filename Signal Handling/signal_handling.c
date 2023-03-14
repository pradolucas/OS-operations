#include <sys/wait.h> /* system call - wait */
#include <stdint.h>   /* system call - wait */
#include <stdlib.h>   /* system call - exit */
#include <unistd.h>   /* system call - fork, exec, sleep */
#include <stdio.h>

#include <stdbool.h>
#include <signal.h>/* Lib - System Call Signal */

// Variaveis globais
int file1Open = true; /* Arquivo hipotetico 1 */
int file2Open = true; /* Arquivo hipotetico 2 */
int valor1 = 500;

void fun()
{
    printf("\nFechando arquivos 1 e 2; O conteúdo de valor1 é: %d\n", valor1);
    file1Open = false;
    file2Open = false;
}

void handler(int signum)
{
    char input;
    if (signum == SIGTERM)
    {
        fun();
        exit(-1);
        // signal(SIGINT,SIG_DFL); // Default signal handling or exit? 
    }
    else if (signum == SIGINT)
    {
        printf("\nDigite X/x se deseja realmente parar o processo: ");
        scanf("%s", &input);
        if (input == 'X' || input == 'x'){
            fun();
            exit(-1);
            // signal(SIGTERM,SIG_DFL);   // Default signal handling or exit?
        }
        else
            return;
    }
}


int main()
{
    signal(SIGINT, handler);
    signal(SIGTERM, handler);

    pid_t pid;
    pid = fork();
    if (!pid)
    {
        printf("Sou o FILHO. %d\n", getpid());
        return 0;
    }
    else if (pid)
    {
        printf("Sou o PAI. %d\n", getpid());
        valor1 /= 20;
        wait(NULL);
        while (1);
        
        printf("O status do file1Open eh %d e o file2Open eh %d \n", file1Open,
               file2Open);
        printf(" O valor1 = %d\n", valor1);
        return 0;
    }
}
