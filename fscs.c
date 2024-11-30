#include "graph.h"
#include <stdlib.h>
#include "fscs.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

int fscs(Graph *graph, Solution **solutions)
{
    int num_supermarkets, num_citizens;
    Vertex *citizens = getCitizens(graph, &num_citizens);
    Vertex *supermarkets = getSupermarkets(graph, &num_supermarkets);

    // Calcula o número máximo de soluções possível, como sendo o mínimo entre o número de supermercados e o número de cidadãos.
    int max_solutions = num_supermarkets < num_citizens ? num_supermarkets : num_citizens;
    //Solution *solutions = (Solution*)malloc(max_solutions * sizeof(Solution));

    // Inicializa a lista de Soluções (lista de caminhos válidos), lista de nós visitados, e o número de soluções.
    /*for (int i = 0; i < max_solutions; i++) {
        solutions[i].path = (int *)malloc(graph->rows * graph->cols * sizeof(int));
        solutions[i].path_length = 0; // Inicializa o tamanho do caminho como 0
        solutions[i].citizen_id = -1; // Opcional: inicializa com um valor inválido
    }
    */

    int num_solutions = 0;

    // Invoca Find Safe Citizen e guarda o resultado numa variável result
    for (int i = 0; i < max_solutions; i++)
    {
        int *visited = (int *)malloc(graph->rows * graph->cols * sizeof(int));
        int *visited_total = (int *)malloc(graph->rows * graph->cols * sizeof(int));
        printf("\nExecuting findSafeCitizen for citizen %d\n", citizens[i].id);
        int result = findSafeCitizen(graph, &citizens[i], visited_total, visited, solutions, num_solutions);
        for (int i = 0; i < sizeof(solutions) / sizeof(Solution); i++)
        {
            printf("%d", solutions[i]->citizen_id);
        }
        free(visited);
        free(visited_total);
        printf("Resultado encontrado para o cidadão %d", solutions[i]->citizen_id);

        // Se result for diferente de zero incrementa número de soluções
        if (result != 0)
        {
            num_solutions++;
        }
    }

    return num_solutions; // Retorna o número de soluções encontradas
}

int findSafeCitizen(Graph *graph, Vertex *citizen, int *visited_total, int *visited, Solution **solutions, int num_solutions)
{
    // Inicializa a lista dos próximos nós
    srand(time(NULL)); // Inicializa a semente do gerador de números aleatórios
    int *next_nodes = (int *)malloc(graph->cols * graph->rows * sizeof(int));
    int max_nodes = graph->cols * graph->rows;
    memset(next_nodes, 0, max_nodes * sizeof(int));

    // Atribuir o citizen_id ao solution
    // Criar solução
    Solution *solution = (Solution *)malloc(sizeof(Solution));
    int *arr = getConnectedVertexes(graph, *citizen);
    //Ignorar vértices com nodes inválidos
    if(arr[0] > max_nodes){
        printf("💩Não existe mais nenhum caminho para o cidadão percorrer💩\n");
        return 0;
    }

    solution->citizen_id = citizen->id;
    // solution->path = (int *)malloc(graph->rows * graph->cols * sizeof(int)); // Allocate memory for the path array
    solution->path_length = 1;
    solution->path = malloc(solution->path_length * sizeof(int));
    solution->path[0] = citizen->id;
    printf("\nFinding path for citizen %d\n", citizen->id);
    // Adiciona o cidadão à lista de visitados para evitar que ele seja visitado novamente
    visited[0] = citizen->id;

    // Obtem os sucessores do cidadão
    int *temp_nodes = getConnectedVertexes(graph, *citizen);
    for (int i = 0; i < citizen->edge_count; i++)
    {
        next_nodes[i] = temp_nodes[i];
        printf("    Added to next node list: %d\n", next_nodes[i]);
    }
    free(temp_nodes);

    // Se a lista dos próximos nós for vazia, sai e devolve 0
    if (next_nodes[0] == 0)
    {
        free(next_nodes);
        printf("Não foi encontrado nenhum caminho!\n");
        free(solution);
        return 0;
    }

    while (1)
    { // Continua enquanto existirem nós para visitar
        int nodecount = 0;
        for (int i = 0; i < graph->cols * graph->rows; i++)
        {
            if (next_nodes[i] > 0)
            {
                nodecount++;
            }
        }
        if (nodecount == 0)
        { // Sai se não houverem nós disponíveis
            free(next_nodes);
            printf("Não foi encontrado nenhum caminho!\n");
            free(solution->path);
            free(solution);
            return 0;
        }
        // Se todos os nós tiverem já sido visitados retorna 0
        int all_visited = 1;
        for (int i = 0; i < graph->cols * graph->rows; i++)
        {
            if (next_nodes[i] > 0)
            {
                all_visited = 0;
                break;
            }
        }
        if (all_visited)
        {
            free(next_nodes);
            printf("Não foi encontrado nenhum caminho!\n");
            free(solution);
            return 0;
        }

        // Escolhe aleatoriamente um nó da lista
        printf("    Node pool(%d): ", nodecount);
        for (int i = 0; i < graph->cols * graph->rows; i++)
        {
            if (next_nodes[i] > 0)
            {
                printf("%d ", next_nodes[i]);
            }
        }

        int next_node = 0;

        if (nodecount == 1)
        {
            for (int i = 0; i < graph->cols * graph->rows; i++)
            {
                if (next_nodes[i] > 0)
                {
                    next_node = next_nodes[i];
                    break;
                }
            }
        }
        else
        {
            do
            {
                next_node = next_nodes[rand() % nodecount];
                // printf("\n    Picked node: %d\n", next_node);
            } while (next_node <= 0);
        }

        printf("    Chosen node: %d\n", next_node);

        // Remove o nó da lista de próximos nós
        for (int i = 0; i < graph->cols * graph->rows; i++)
        {
            if (next_nodes[i] == next_node)
            {
                next_nodes[i] = 0;
                printf("    Removed node %d from next node list\n", next_node);
                break;
            }
        }

        // Verifica se o nó é um supermercado

        if (getVertexById(graph, next_node)->type == 's')
        {
            printf("    Found supermarket at node %d\n", next_node);

            // Adiciona o nó aos visitados e à solução
            for (int i = 0; i < graph->rows * graph->cols; i++)
            {
                if (visited[i] == 0)
                {
                    visited[i] = next_node;
                    break;
                }
            }

            solution->path[solution->path_length++] = next_node;

            // Remove os arcos que terminam no nó supermercado
            for (int i = 0; i < graph->rows; i++)
            {
                for (int j = 0; j < graph->cols; j++)
                {
                    for (int k = 0; k < graph->vertices[i][j].edge_count; k++)
                    {
                        if (graph->vertices[i][j].edges[k].target_id == next_node)
                        {
                            removeEdge(graph, &graph->vertices[i][j], next_node);
                            printf("    Removed edge from node %d to node %d\n", graph->vertices[i][j].id, next_node);
                        }
                    }
                }
            }
            // Remove os arcos que terminam em nós visitados
            for (int i = 0; i < graph->rows * graph->cols; i++)
            {
                if (visited[i] > 0)
                {
                    for (int j = 0; j < graph->rows; j++)
                    {
                        for (int k = 0; k < graph->cols; k++)
                        {
                            for (int l = 0; l < graph->vertices[j][k].edge_count; l++)
                            {
                                if (graph->vertices[j][k].edges[l].target_id == visited[i])
                                {
                                    removeEdge(graph, &graph->vertices[j][k], visited[i]);
                                    printf("    Removed edge from node %d to node %d\n", graph->vertices[j][k].id, visited[i]);
                                }
                            }
                        }
                    }
                }
            }

            free(next_nodes);
            // Print path
            printf("    Path: ");
            for (int i = 0; i < solution->path_length; i++)
            {
                printf("%d ", solution->path[i]);
            }
            printf("\n");
            return 1;
        }

        // Verifica se o nó já foi visitado
        int was_visited = 0;
        for (int i = 0; i < graph->rows * graph->cols; i++)
        {
            if (visited[i] == next_node)
            {
                was_visited = 1;
                printf("    Node %d was already visited\n", next_node);
                break;
            }
        }

        if (!was_visited)
        {
            printf("    Node %d was not visited\n", next_node);
            solution->path[solution->path_length++] = next_node;
            for (int i = 0; i < graph->rows * graph->cols; i++)
            {
                if (visited[i] == 0)
                {
                    visited[i] = next_node;
                    printf("    Added to visited list: %d\n", next_node);
                    break;
                }
            }
            for (int i = 0; i < graph->rows * graph->cols; i++)
            {
                if (visited_total[i] == 0)
                {
                    visited_total[i] = next_node;
                    printf("    Added to total visited list: %d\n", next_node);
                    break;
                }
            }

            // Atualiza next_nodes com os sucessores do nó atual
            memset(next_nodes, 0, graph->cols * graph->rows * sizeof(int));
            Vertex *current_vertex = getVertexById(graph, next_node);
            for (int i = 0; i < current_vertex->edge_count; i++)
            {
                next_nodes[i] = current_vertex->edges[i].target_id;
                printf("    Added to next node list: %d\n", next_nodes[i]);
            }
            // Mostrar caminho até agora
            printf("    Path so far: ");
            for (int i = 0; i < solution->path_length; i++)
            {
                printf("%d ", solution->path[i]);
            }
            printf("\n");
        }
    }
    return 1;
}
