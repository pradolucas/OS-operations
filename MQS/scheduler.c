#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const int prior_levels = 4;

// struct de input
typedef struct args
{
    int id;
    char process_name[30];
    int prior;
    int cpu_burst;
} args;

// linked list
typedef struct Node
{
    int id;
    int burst;
    char process_name[30];
    struct Node *next;
} node_t;

// process execution info
typedef struct process_info
{
    int id;
    int time;
    int burst;
    char process_name[30];
    struct process_info *next;
} process_info_t;

void printLinkedList(node_t *head)
{
    if (head == NULL)
    {
        printf("Empty list");
    }
    while (head != NULL)
    {
        printf("(%d, %d) ", head->id, head->burst);
        head = head->next;
    }
}
void printArgs(args **lst_args, int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("\nprocess_name: %s, prior: %d, cpu_burst: %d", lst_args[i]->process_name, lst_args[i]->prior, lst_args[i]->cpu_burst);
    }
    printf("\n");
}
void printSchedulerState(node_t *prior_queues[])
{
    for (int i = 0; i < prior_levels; i++)
    {
        printf("\n\tP%d queue: ", i);
        printLinkedList(prior_queues[i]);
    }
    printf("\n\n");
}
void printWaitingTime(process_info_t *waiting_time[], int n_procs)
{
    int sum = 0;
    if (waiting_time == NULL)
    {
        printf("Empty list");
    }

    printf("\nPROCESS\t\tPROCESS NAME\t\tBURST TIME\tWAITING TIME");
    for (int i = 0; i < n_procs; i++)
    {
        process_info_t *head = waiting_time[i];
        if (head != NULL)
        {
            printf("\n%d \t\t %s \t\t\t %d \t\t %d", head->id, head->process_name, head->burst, head->time);
            sum += head->time;
            head = head->next;
        }
    }
    printf("\nMean Waiting time = %f\n", (float)sum / n_procs);
}

void insertSortLL(node_t **head, int data, int burst, char process_name[30])
{
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->id = data;
    new_node->burst = burst;
    // new_node->process_name = malloc(30 * sizeof(char));
    strcpy(new_node->process_name, process_name);
    // new_node->process_name = process_name;
    new_node->next = NULL;

    if (*head == NULL || (*head)->burst >= burst)
    {
        new_node->next = *head;
        *head = new_node;
    }
    else
    {
        node_t *current = *head;
        while (current->next != NULL && current->next->burst <= burst)
            current = current->next;
        new_node->next = current->next;
        current->next = new_node;
    }
}
void removeByIndexLL(node_t **head, int pos)
{

    if ((*head) == NULL || (*head)->next == NULL)
    { // empty list
        *head = NULL;
        return;
    }
    else if (pos == 0)
    { // start of the list
        node_t *next_node = (*head)->next;
        free(*head);
        *head = next_node;
        return;
    }

    // Any other case
    node_t *current_node = *head;
    for (int i = 0; i < pos - 1; i++)
    {
        current_node = current_node->next;
    }
    node_t *temp_node = current_node->next;
    current_node->next = temp_node->next;
    free(temp_node);
}

void input_handling(args *procs[], int n_procs)
{
    for (int i = 0; i < n_procs; i++)
    {
        procs[i] = (args *)malloc(sizeof(struct args));
    }

    for (int i = 0; i < n_procs; i++)
    {
        printf("\n[Processo %d] Nome do Processo, Prioridade, CPU Burst: ", i);
        procs[i]->id = i;
        scanf("%s %d %d", procs[i]->process_name, &procs[i]->prior, &procs[i]->cpu_burst);
    }
    printf("\n");
}

void init_scheduler(node_t *prior_queues[],
                    process_info_t *waiting_time[],
                    args *procs[],
                    int n_procs)
{
    // prior_levels;
    // order by CPU-Burst, SJF; Insertion sort
    for (int i = 0; i < prior_levels; i++)
    {
        prior_queues[i] = (node_t *)malloc(sizeof(node_t));
        prior_queues[i] = NULL;
    }
    for (int i = 0; i < n_procs; i++)
    {
        waiting_time[i] = (process_info_t *)malloc(sizeof(struct process_info));                                 // allocating memory
        insertSortLL(&prior_queues[procs[i]->prior], procs[i]->id, procs[i]->cpu_burst, procs[i]->process_name); // inserting values in priority queue
    }

    printSchedulerState(prior_queues);
}

void aging(node_t *prior_queues[])
{
    int pos;
    for (int i = prior_levels - 1; i >= 1; i--)
    { // No upper priority beyond 0
        pos = 0;
        if (prior_queues[i] != NULL)
        {
            node_t *current = prior_queues[i];
            while (current->next != NULL)
            {
                current = current->next;
                pos++;
            }

            printf("\tSelecionado Aging :: (prior, id, burst): (%d, %d, %d) -> (%d, %d, %d) ", i, current->id, current->burst, i - 1, current->id, current->burst);
            
            // Insert in upper priority
            insertSortLL(&prior_queues[i - 1], current->id, current->burst, current->process_name);
            
            // remove last item
            removeByIndexLL(&prior_queues[i], pos);

            break;
        }
    }
}

void scheduling(node_t *prior_queues[], process_info_t *waiting_time[])
{
    int time_unit = 0;
    int k = 0, w_cont = 0;
    while (k < prior_levels)
    {
        while (prior_queues[k] != NULL)
        {

            int id = prior_queues[k]->id;
            int burst = prior_queues[k]->burst;

            // Saving metrics
            waiting_time[w_cont]->id = id;
            waiting_time[w_cont]->burst = burst;
            waiting_time[w_cont]->time = time_unit;
            strcpy(waiting_time[w_cont]->process_name, prior_queues[k]->process_name);
            w_cont++;

            // "Executing" process
            printf("Executing process %d\n", id);
            removeByIndexLL(&prior_queues[k], 0);
            for (int j = 1; j <= burst; j++)
            {
                time_unit++;
                printf("time: %d\n", time_unit);
                if (time_unit % 10 == 0)
                { // each 10 time units perform aging
                    aging(prior_queues);
                    printSchedulerState(prior_queues);
                }
            }
            printf("Finished process %d", id);
            printSchedulerState(prior_queues);
            k = 0;
        }
        k++;
    }
}

void freeScheduler(node_t *prior_queues[],
                   process_info_t *waiting_time[],
                   args *procs[],
                   int n_procs)
{
    for (int i = 0; i < n_procs; i++)
    {
        free(waiting_time[i]);
        free(procs[i]);
    }

    for (int i = 0; i < prior_levels; i++)
    {
        node_t *head = prior_queues[i];
        node_t *tmp;
        while (head != NULL)
        {
            tmp = head;
            head = head->next;
            free(tmp);
        }
    }
}

int main()
{

    int n_procs;
    printf("Quantos Processos serão executados? ");
    scanf("%d", &n_procs);

    args *procs[n_procs];
    process_info_t *waiting_time[n_procs];
    node_t *prior_queues[prior_levels];

    // Recepcao dos Processos
    input_handling(procs, n_procs);

    // Instanciate scheduler
    init_scheduler(prior_queues, waiting_time, procs, n_procs);

    // Escalonar os Processos
    scheduling(prior_queues, waiting_time);

    // Apresentar os Resultados.
    printWaitingTime(waiting_time, n_procs);

    // free structures
    freeScheduler(prior_queues, waiting_time, procs, n_procs);

    return 0;
}

