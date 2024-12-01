#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include "input.h"
#include "graph.h"
#include "fscs.h"
#include <limits.h>

// Estrutura para soluções compartilhadas
typedef struct
{
    Solution *solutions; // Array de soluções
    int cost;            // Soma dos path lengths das soluções
    sem_t semaphore;     // Semáforo para controle de acesso
} SharedSolution;

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Uso: %s <input_file> <num_processes> <max_time_ms>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    int num_processes = atoi(argv[2]);
    int max_time_ms = atoi(argv[3]);

    // Leitura dos dados do arquivo
    int num_avenues, num_streets, num_supermarkets, num_citizens;
    Location *supermarkets, *citizens;

    if (read_input(filename, &num_avenues, &num_streets, &num_supermarkets, &num_citizens, &supermarkets, &citizens) != 0)
    {
        fprintf(stderr, "Erro ao ler o arquivo de entrada.\n");
        return EXIT_FAILURE;
    }

    // Criação do grafo
    Graph *city = createGraph(num_streets, num_avenues);
    if (!city)
    {
        fprintf(stderr, "Erro ao criar o grafo.\n");
        return EXIT_FAILURE;
    }

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

    int max_solutions = num_supermarkets < num_citizens ? num_supermarkets : num_citizens;

    // Memória compartilhada para soluções
    SharedSolution *sharedSolution = mmap(NULL, sizeof(SharedSolution),
                                          PROT_READ | PROT_WRITE,
                                          MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sharedSolution == MAP_FAILED)
    {
        perror("Erro ao mapear memória compartilhada");
        return EXIT_FAILURE;
    }

    sem_init(&sharedSolution->semaphore, 1, 1);
    sharedSolution->solutions = (Solution *)calloc(max_solutions, sizeof(Solution));
    sharedSolution->cost = INT_MAX; // Inicializa custo com um valor alto

    // Criar processos
    pid_t pids[num_processes];
    for (int i = 0; i < num_processes; i++)
    {
        pids[i] = fork();
        if (pids[i] < 0)
        {
            perror("Erro ao criar processo");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0)
        {
            // Código do processo filho
            struct timespec start_time, current_time;
            clock_gettime(CLOCK_MONOTONIC, &start_time);

            while (1)
            {
                clock_gettime(CLOCK_MONOTONIC, &current_time);
                int elapsed_time = (current_time.tv_sec - start_time.tv_sec) * 1000 +
                                   (current_time.tv_nsec - start_time.tv_nsec) / 1000000;

                if (elapsed_time >= max_time_ms)
                    break;

                int numSolutions = 0;
                Solution *result = fscs(city, &numSolutions);
                int totalPathLength = 0;

                for (int i = 0; i < numSolutions; i++)
                {
                    totalPathLength += result[i].path_length;
                }

                // Atualiza a melhor solução de forma concorrente
                sem_wait(&sharedSolution->semaphore);
                if (totalPathLength < sharedSolution->cost)
                {
                    sharedSolution->cost = totalPathLength;

                    // Copiar soluções para a memória compartilhada
                    for (int i = 0; i < numSolutions; i++)
                    {
                        sharedSolution->solutions[i].citizen_id = result[i].citizen_id;
                        sharedSolution->solutions[i].path_length = result[i].path_length;
                        sharedSolution->solutions[i].path = (int *)malloc(result[i].path_length * sizeof(int));
                        memcpy(sharedSolution->solutions[i].path, result[i].path, result[i].path_length * sizeof(int));
                    }
                    printf("🥳 Processo %d encontrou uma nova melhor solução: %d 🥳\n", getpid(), sharedSolution->cost);
                }
                sem_post(&sharedSolution->semaphore);

                // Libera memória da solução local
                for (int i = 0; i < numSolutions; i++)
                {
                    free(result[i].path);
                }
                free(result);

                usleep(100000); // Pausa de 100ms
            }
            exit(EXIT_SUCCESS);
        }
    }

    // Código do processo pai
    for (int i = 0; i < num_processes; i++)
    {
        waitpid(pids[i], NULL, 0); // Espera todos os filhos terminarem
    }

    // Exibe a melhor solução encontrada
    printf("\nMelhor solução encontrada (Custo: %d):\n", sharedSolution->cost);
    for (int i = 0; i < max_solutions; i++)
    {
        printf("Cidadão %d: Caminho [", sharedSolution->solutions[i].citizen_id);
        for (int j = 0; j < sharedSolution->solutions[i].path_length; j++)
        {
            printf("%d%s", sharedSolution->solutions[i].path[j], (j == sharedSolution->solutions[i].path_length - 1) ? "" : ", ");
        }
        printf("] (Comprimento: %d)\n", sharedSolution->solutions[i].path_length);
    }

    // Limpeza
    sem_destroy(&sharedSolution->semaphore);
    for (int i = 0; i < max_solutions; i++)
    {
        free(sharedSolution->solutions[i].path);
    }
    free(sharedSolution->solutions);
    munmap(sharedSolution, sizeof(SharedSolution));
    free(city);
    free(supermarkets);
    free(citizens);

    return EXIT_SUCCESS;
}
