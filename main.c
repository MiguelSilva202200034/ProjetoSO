#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include "graph.h"
#include "fscs.h"
#include "input.h"

#define MAX 100

Graph graph;
Solution solutions[MAX];
int num_solutions = 0;
sem_t sem_graph, sem_solution;


int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <input_file> <num_processes> <max_time_ms>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Inicializa semáforos
    sem_init(&sem_graph, 1, 1);
    sem_init(&sem_solution, 1, 1);

    // Definir o ficheiro de entrada e os parâmetros de execução
    const char *filename = argv[1];
    int num_processes = atoi(argv[2]);
    int max_time_ms = atoi(argv[3]);

    // Variáveis para armazenar as informações lidas
    int num_avenues, num_streets, num_supermarkets, num_citizens;
    Location *supermarkets, *citizens;

    // Ler os dados de entrada
    if (read_input(filename, &num_avenues, &num_streets, &num_supermarkets, &num_citizens, &supermarkets, &citizens) != 0) {
        fprintf(stderr, "Erro ao ler o arquivo de entrada.\n");
        return EXIT_FAILURE;
    }

    // Inicializa grafo
    int num_vertices = num_avenues * num_streets + 2; 
    init_graph(&graph, num_vertices);

    // Adiciona arestas horizontais (conexões entre avenidas)
    for (int i = 0; i < num_avenues; i++) {
        for (int j = 0; j < num_streets - 1; j++) {
            int v1 = i * num_streets + j;       // Vértice (avenida, rua)
            int v2 = i * num_streets + j + 1;   // Vértice (avenida, rua + 1)
            if (v1 != 0 && v2 != num_vertices - 1) {
                add_edge(&graph, v1, v2);  // Aresta entre eles (horizontal)
            }
        }
    }

    // Adiciona arestas verticais (conexões entre ruas)
    for (int i = 0; i < num_avenues - 1; i++) {
        for (int j = 0; j < num_streets; j++) {
            int v1 = i * num_streets + j;       // Vértice (avenida, rua)
            int v2 = (i + 1) * num_streets + j; // Vértice (avenida + 1, rua)
            if (v1 != 0 && v2 != num_vertices - 1) {
                add_edge(&graph, v1, v2);  // Aresta entre eles (vertical)
            }
        }
    }

    // Adicionar arestas no grafo com base nas localizações dos cidadãos e supermercados
    for (int i = 0; i < num_supermarkets; i++) {
        int supermarket_index = supermarkets[i].avenue * num_streets + supermarkets[i].street;
        add_edge(&graph, supermarket_index, num_vertices - 1);  // Adiciona aresta para o supermercado
    }

    for (int i = 0; i < num_citizens; i++) {
        int citizen_index = citizens[i].avenue * num_streets + citizens[i].street;
        add_edge(&graph, 0, citizen_index);  // Cidadãos conectados ao ponto inicial
    }

    num_solutions = fscs(&graph, solutions, num_citizens, num_supermarkets);

    printf("Nº Ruas: %d\n NºAvenidas: %d\n NºSupermercados: %d\n NºCidadãos: %d\n", num_streets,num_avenues,num_supermarkets,num_citizens);
    
    printf("Localização dos supermercados:\n");
    for (int i = 0; i < num_supermarkets; i++) {
        printf("Supermercado %d: Avenida %d, Rua %d\n", i+1, supermarkets[i].avenue, supermarkets[i].street);
    }

    printf("Localização dos cidadãos:\n");
    for (int i = 0; i < num_citizens; i++) {
        printf("Cidadão %d: Avenida %d, Rua %d\n", i+1, citizens[i].avenue, citizens[i].street);
    }



    printf("Número de soluções: %d\n", num_solutions);
    for (int i = 0; i < num_solutions; i++) {
        printf("Cidadão %d: ", solutions[i].citizen_id);
        for (int j = 0; j < solutions[i].path_length; j++) {
            printf("%d ", solutions[i].path[j]);
        }
        printf("\n");
    }


    free(supermarkets);
    free(citizens);

    sem_destroy(&sem_graph);
    sem_destroy(&sem_solution);

    return 0;
}
