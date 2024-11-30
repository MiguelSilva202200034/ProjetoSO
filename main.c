#include <stdio.h>
#include "graph.h"
#include "fscs.h"
#include <stdlib.h>
#include "input.h"

int main(int argc, char *argv[])
{
    //if (argc != 4) {
    //    fprintf(stderr, "Uso: %s <input_file> <num_processes> <max_time_ms>\n", argv[0]);
    //    return EXIT_FAILURE;
    //}

    //const char *filename = argv[1];
    //int num_processes = atoi(argv[2]);
    //int max_time_ms = atoi(argv[3]);

    //int num_avenues, num_streets, num_supermarkets, num_citizens;
    //Location *supermarkets, *citizens;

    //if (read_input(filename, &num_avenues, &num_streets, &num_supermarkets, &num_citizens, &supermarkets, &citizens) != 0) {
    //    fprintf(stderr, "Erro ao ler o arquivo de entrada.\n");
    //    return EXIT_FAILURE;
    //}

    // Configuração do exemplo no enunciado
    int M = 3; // Número de avenidas
    int N = 3; // Número de ruas

    // Coordenadas dos supermercados e cidadãos
    int supermarkets[][2] = {{3, 2}, {3, 3}};
    int citizens[][2] = {{2, 2}, {1, 1}};
    int num_citizens = 2;
    int num_supermarkets = 2;

    // Cria o grafo
    Graph *city = createGraph(N, M);

    // Adiciona supermercados ('s') no grafo
    for (int i = 0; i < num_supermarkets; i++)
    {
        addVertex(city, supermarkets[i][0], supermarkets[i][1], 's');
    }

    // Adiciona cidadãos ('c') no grafo
    for (int i = 0; i < num_citizens; i++)
    {
        addVertex(city, citizens[i][0], citizens[i][1], 'c');
    }

    // Exibe o grafo
    displayGraph(city);

    // Lista todos os cidadãos e supermercados
    printf("\n");
    listCitizensAndSupermarkets(city);
    
    int numSolutions = 0;
    //Solution* solutionsFound = fscs(city, &solutions, &numSolutions);
    Solution *solutions = fscs(city, &numSolutions);
    printf("Numero de soluções encontradas: %d\n", numSolutions);
    // Mostrar caminhos
    for (int i = 0; i < numSolutions; i++)
    {
        printf("\nPath for citizen %d: \n", solutions[i].citizen_id);
        printf("\nSize of solution: %d\n", solutions[i].path_length);
        for (int j = 0; j < solutions[i].path_length; j++)
        {
            if (solutions[i].path != NULL)
            {
                printf("%d ", solutions[i].path[j]);
            }
            else
            {
                printf("Path is NULL for citizen %d\n", i);
            }
        }
        printf("\n");
    }

    // Libera a memória
    freeGraph(city);
    printf("Limpou o grafo!\n");
    for (int i = 0; i < numSolutions; i++)
    {
        if (solutions[i].path)
        { // Evita liberar ponteiro nulo
            free(solutions[i].path);
            printf("\nLiberou o caminho da solução %d!\n", i);
        }
    }
    free(solutions);
    printf("\nLimpou as solucoes!\n");
    
    return EXIT_SUCCESS;
}
