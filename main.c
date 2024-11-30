#include <stdio.h>
#include "graph.h"
#include "fscs.h"
#include <stdlib.h>

int main()
{
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
    for (int i = 0; i < 2; i++)
    {
        addVertex(city, supermarkets[i][0], supermarkets[i][1], 's');
    }

    // Adiciona cidadãos ('c') no grafo
    for (int i = 0; i < 2; i++)
    {
        addVertex(city, citizens[i][0], citizens[i][1], 'c');
    }

    // Exibe o grafo
    displayGraph(city);

    // Lista todos os cidadãos e supermercados
    printf("\n");
    listCitizensAndSupermarkets(city);

    // Executa o algoritmo
    int max_solutions = num_supermarkets < num_citizens ? num_supermarkets : num_citizens;
    Solution *solutions = (Solution *)malloc(max_solutions * sizeof(Solution));
    int num_solutions = fscs(city, &solutions);
    // Mostrar caminhos
    for (int i = 0; i < num_solutions; i++)
    {
        printf("\nPath for citizen %d: \n", solutions[i].citizen_id);
        printf("Size of solution: %d\n", solutions[i].path_length);
        /*for (int j = 0; j < solutions[i].path_length; j++)
        {
            printf("%d ", solutions[i].path[j]);
        }*/
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
    for (int i = 0; i < num_solutions; i++)
    {
        if (solutions[i].path)
        { // Evita liberar ponteiro nulo
            free(solutions[i].path);
            printf("Liberou o caminho da solução %d!\n", i);
        }
    }
    /*for (int i = 0; i < num_solutions; i++)
    {
        free(&solutions[i].path);
        printf("Limpou um caminho da solucao!\n");
        free(&solutions[i]);
        printf("Limpou uma solucao!\n");
    }*/
    free(solutions);
    printf("Limpou as solucoes!\n");

    return 0;
}
