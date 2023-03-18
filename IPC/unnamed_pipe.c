#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

const int n = 15;
typedef struct cotacao
{
    int currency_ori;
    int currency_dest;
    int cot;
    char data_hora[30];
} cotacao;

void print_cotacao(cotacao *display)
{

    printf("\nOrigin Currency\t\t  Target Currency \t\t Price \t\t datetime\n");
    printf("\t%d\t\t%d\t\t%d\t%s\n", display->currency_ori, display->currency_dest, display->cot, display->data_hora);
}

void gerador_aleatorio_cotacao(cotacao * cot){
    time_t mytime = time(NULL);
    char * time_str = ctime(&mytime);
    time_str[strlen(time_str)-1] = '\0';

    cot->currency_ori = rand();
    cot->currency_dest = rand();
    cot->cot = rand();
    strcpy(cot->data_hora, time_str);
}

int main()
{
    int pipefds[2];
    cotacao *coletor[n];
    cotacao *displayer;

    displayer = (cotacao *) malloc(sizeof(cotacao));

    for (int i = 0; i < n; i++)
    {
        coletor[i] = (cotacao *)malloc(sizeof(cotacao));
        
    }

    // Cria o PIPE
    if (pipe(pipefds) < 0)
    {
        printf("Erro na criação do PIPE\n");
        return 1;
    }

    pid_t pid;
    pid = fork();
    for (int i = 0; i < n; i++)
    {
        
        if (pid == 0)
        {
            read(pipefds[0], displayer, sizeof(cotacao));
            printf("Sou o PROCESSO FILHO lendo a mensagem\n");
            print_cotacao(displayer);
            // exit(EXIT_SUCCESS);
        }
        else
        {   
            gerador_aleatorio_cotacao(coletor[i]);
            printf("Sou o PROCESSO PARENTE gravando a mensagem\n");
            write(pipefds[1], coletor[i], sizeof(cotacao));
            sleep(0.5);
        }
    }
}
