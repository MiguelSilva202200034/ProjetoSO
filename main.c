#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <unistd.h>
#include "graph.h"
#include "fscs.h"

#define MAX 100

Graph graph;
Solution solutions[MAX];
int num_solutions = 0;
sem_t sem_graph, sem_solution;

int main() {
    // Inicializa semáforos
    sem_init(&sem_graph, 1, 1);
    sem_init(&sem_solution, 1, 1);

    // Inicializa o grafo
    init_graph(&graph);
    graph.num_vertices = 11; // Exemplo com 3x3 cruzamentos, 2 supermercados, 2 cidadãos

    // Adiciona arestas do exemplo
    add_edge(&graph, 0, 1); // Cidadão 0
    add_edge(&graph, 0, 5); // Cidadão 1
    add_edge(&graph, 1, 2);
    add_edge(&graph, 5, 8);
    add_edge(&graph, 2, 3); // Supermercado
    add_edge(&graph, 8, 10); // Supermercado

    // Chama FSCS
    int citizens = 2, supermarkets = 2;
    int num_solutions = fscs(&graph, solutions, citizens, supermarkets);

    // Imprime resultados
    printf("Número de soluções: %d\n", num_solutions);
    for (int i = 0; i < num_solutions; i++) {
        printf("Cidadão %d: ", solutions[i].citizen_id);
        for (int j = 0; j < solutions[i].path_length; j++) {
            printf("%d ", solutions[i].path[j]);
        }
        printf("\n");
    }

    // Libera recursos
    sem_destroy(&sem_graph);
    sem_destroy(&sem_solution);

    return 0;
}