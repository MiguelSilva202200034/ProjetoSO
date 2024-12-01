#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "input.h"
#include "graph.h"
#include "fscs.h"
#include "time.h"

typedef struct
{
    Solution *solutions; // Caminho representado por um vetor de inteiros
    int cost;            // Soma dos path lengths das soluções do array de solutions
    sem_t semaphore;
} SharedSolution;

int main(int argc, char *argv[])
{
    SharedSolution *sharedSolution = mmap(NULL, sizeof(SharedSolution),
                                          PROT_READ | PROT_WRITE,
                                          MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (argc != 4)
    {
        fprintf(stderr, "Uso: %s <input_file> <num_processes> <max_time_ms>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    int num_processes = atoi(argv[2]);
    int max_time_ms = atoi(argv[3]);

    int num_avenues, num_streets, num_supermarkets, num_citizens;
    Location *supermarkets, *citizens;

    if (read_input(filename, &num_avenues, &num_streets, &num_supermarkets, &num_citizens, &supermarkets, &citizens) != 0)
    {
        fprintf(stderr, "Erro ao ler o arquivo de entrada.\n");
        return EXIT_FAILURE;
    }
    // Cria o grafo
    Graph *city = createGraph(num_streets, num_avenues);

    int max_solutions = num_supermarkets < num_citizens ? num_supermarkets : num_citizens;

    // Adiciona supermercados ('s') no grafo
    for (int i = 0; i < num_supermarkets; i++)
    {
        addVertex(city, supermarkets[i].avenue, supermarkets[i].street, 's');
    }

    // Adiciona cidadãos ('c') no grafo
    for (int i = 0; i < num_citizens; i++)
    {
        addVertex(city, citizens[i].avenue, citizens[i].street, 'c');
    }

    sem_init(&sharedSolution->semaphore, 1, 1);
    sharedSolution->cost = (num_streets * num_avenues) * max_solutions;

    int bestPid;
    // Criar processos
    pid_t pids[num_processes];
    for (int i = 0; i < num_processes; i++)
    {
        pids[i] = fork();
        if (pids[i] < 0)
        {
            perror("Falha ao criar processo");
            exit(EXIT_FAILURE);
        }

        // Código do processo filho
        if (pids[i] == 0)
        {
            time_t start_time = time(NULL);
            while (time(NULL) - start_time < max_time_ms)
            {
                int numSolutions = 0;
                Solution *result = fscs(city, &numSolutions);
                int totalPathLength = 0;
                for (int i = 0; i < numSolutions; i++)
                {
                    totalPathLength += result[i].path_length;
                }
                // Atualizar o melhor resultado de forma controlada
                sem_wait(&sharedSolution->semaphore);
                printf("Total do caminho: %d\n", totalPathLength);
                printf("Custo da shared solution: %d\n", sharedSolution->cost);
                if (totalPathLength < sharedSolution->cost)
                {
                    sharedSolution->solutions = result;
                    sharedSolution->cost = totalPathLength;
                    printf("🥳🥳Processo %d encontrou um novo melhor resultado: %d🥳🥳\n", getpid(), sharedSolution->cost);
                    bestPid = getpid();
                }
                printf("Processo %d executou o algoritmo!", getpid());
                sem_post(&sharedSolution->semaphore);

                // Simular algum tempo de execução
                usleep(100000); // 100ms
            }
            exit(EXIT_SUCCESS);
        }
    }

    // Código do processo pai
    for (int i = 0; i < num_processes; i++)
    {
        waitpid(pids[i], NULL, 0); // Esperar todos os filhos terminarem
    }

    // Limpeza de recursos
    sem_destroy(&sharedSolution->semaphore); // Destruir o semáforo
    if (munmap(sharedSolution, sizeof(SharedSolution)) == -1)
    {
        perror("Erro ao liberar memória compartilhada");
        return EXIT_FAILURE;
    }

    free(city);

    return EXIT_SUCCESS;
}