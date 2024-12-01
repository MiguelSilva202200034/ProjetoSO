#include "graph.h"
#include <stdlib.h>
#include "fscs.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

Solution *fscs(Graph *graph, int *numSolutions)
{
    int num_supermarkets, num_citizens;
    Vertex *citizens = getCitizens(graph, &num_citizens);
    Vertex *supermarkets = getSupermarkets(graph, &num_supermarkets);

    // Calcula o número máximo de soluções possível, como sendo o mínimo entre o número de supermercados e o número de cidadãos.
    int max_solutions = num_supermarkets < num_citizens ? num_supermarkets : num_citizens;
    Solution *solutions = (Solution *)calloc(max_solutions, sizeof(Solution));
    // printf("Max solutions: %d", max_solutions);
    if (solutions == NULL)
    {
        fprintf(stderr, "Erro: Falha ao alocar memória para soluções.\n");
        return NULL;
    }

    int num_solutions = 0;

    // Invoca Find Safe Citizen e guarda o resultado numa variável result
    for (int i = 0; i < max_solutions; i++)
    {
        int *visited = (int *)malloc(graph->rows * graph->cols * sizeof(int));
        int *visited_total = (int *)malloc(graph->rows * graph->cols * sizeof(int));
        printf("\nExecuting findSafeCitizen for citizen %d\n", citizens[i].id);
        int result = findSafeCitizen(graph, &citizens[i], visited_total, visited, solutions, num_solutions);
        // printf("Numero de solucoes: %d\n", num_solutions);
        // for (int i = 0; i < num_solutions; i++)
        //{
        // printf("Cidadao numero: %d\n", solutions[i].citizen_id);
        //}
        free(visited);
        free(visited_total);

        // Se result for diferente de zero incrementa número de soluções
        if (result != 0)
        {
            printf("\nResultado encontrado para o cidadão %d\n", solutions[i].citizen_id);
            num_solutions++;
        }
    }
    // for (int i = 0; i < num_solutions; i++)
    //{
    //     printf("Citizen id %d: %d\n", i, solutions[i].citizen_id);
    // }
    *numSolutions = num_solutions;
    return solutions; // Retorna o número de soluções encontradas
}

int findSafeCitizen(Graph *graph, Vertex *citizen, int *visited_total, int *visited, Solution *solutions, int num_solutions)
{
    // printf("Numero a ser incrementado de solucoes: %d\n", num_solutions);
    //  Inicializa a lista dos próximos nós
    srand(time(NULL)); // Inicializa a semente do gerador de números aleatórios
    int max_nodes = graph->cols * graph->rows;
    int *next_nodes = (int *)malloc(max_nodes * sizeof(int));
    if (!next_nodes)
    {
        fprintf(stderr, "Erro ao alocar memória para next_nodes.\n");
        return 0;
    }
    memset(next_nodes, 0, max_nodes * sizeof(int));

    solutions[num_solutions].path = (int *)calloc(max_nodes, sizeof(int));
    if (!solutions[num_solutions].path)
    {
        free(next_nodes);
        fprintf(stderr, "Erro ao alocar memória para caminho.\n");
        return 0;
    }

    // Atribuir o citizen_id ao solution
    // Criar solução
    // Solution *solution = (Solution *)malloc(sizeof(Solution));
    int *arr = getConnectedVertexes(graph, *citizen);
    int numberOfNeighbours = sizeof(arr) / sizeof(arr[0]);
    for (int i = 0; i < numberOfNeighbours; i++)
    {
        if (arr[i] > max_nodes)
        {
            printf("💩Não existe mais nenhum caminho para o cidadão percorrer💩\n");
            if (solutions[num_solutions].path != NULL)
            {
                free(solutions[num_solutions].path);
                printf("Path length: %d", solutions[num_solutions].path_length);
            }
            return 0;
        }
    }

    solutions[num_solutions].citizen_id = citizen->id;
    solutions[num_solutions].path_length = 1;
    solutions[num_solutions].path[0] = citizen->id;
    // printf("Id: %d", solutions[num_solutions].citizen_id);
    // printf("\nFinding path for citizen %d\n", citizen->id);
    //  Adiciona o cidadão à lista de visitados para evitar que ele seja visitado novamente
    visited[0] = citizen->id;

    // Obtem os sucessores do cidadão
    int *temp_nodes = getConnectedVertexes(graph, *citizen);
    for (int i = 0; i < citizen->edge_count; i++)
    {
        next_nodes[i] = temp_nodes[i];
        // printf("    Added to next node list: %d\n", next_nodes[i]);
    }
    free(temp_nodes);

    // Se a lista dos próximos nós for vazia, sai e devolve 0
    if (next_nodes[0] == 0)
    {
        free(next_nodes);
        printf("Não foi encontrado nenhum caminho!\n");
        free(solutions[num_solutions].path);
        // free(solutions);
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
            free(solutions[num_solutions].path);
            // free(solutions);
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
            free(solutions[num_solutions].path);
            // free(solutions);
            return 0;
        }

        // Escolhe aleatoriamente um nó da lista
        // printf("    Node pool(%d): ", nodecount);
        int validNodes[nodecount];
        int validIndex = 0;

        for (int i = 0; i < graph->cols * graph->rows; i++)
        {
            if (next_nodes[i] > 0)
            {
                validNodes[validIndex++] = next_nodes[i];
                // printf("%d ", next_nodes[i]);
            }
        }
        // printf("\n");

        // Escolhe o próximo nó aleatoriamente
        int next_node = 0;
        if (nodecount == 1)
        {
            next_node = validNodes[0]; // Apenas um nó válido
        }
        else
        {
            int valid_index = rand() % nodecount; // Índice aleatório dentro dos limites
            next_node = validNodes[valid_index];
        }
        // printf("    Chosen node: %d\n", next_node);

        // Remove o nó da lista de próximos nós
        for (int i = 0; i < graph->cols * graph->rows; i++)
        {
            if (next_nodes[i] == next_node)
            {
                next_nodes[i] = 0;
                // printf("    Removed node %d from next node list\n", next_node);
                break;
            }
        }

        // Verifica se o nó é um supermercado

        if (getVertexById(graph, next_node)->type == 's')
        {
            // printf("    Found supermarket at node %d\n", next_node);

            // Adiciona o nó aos visitados e à solução
            for (int i = 0; i < graph->rows * graph->cols; i++)
            {
                if (visited[i] == 0)
                {
                    visited[i] = next_node;
                    break;
                }
            }

            solutions[num_solutions].path[solutions[num_solutions].path_length++] = next_node;

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
                            // printf("    Removed edge from node %d to node %d\n", graph->vertices[i][j].id, next_node);
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
                                    // printf("    Removed edge from node %d to node %d\n", graph->vertices[j][k].id, visited[i]);
                                }
                            }
                        }
                    }
                }
            }

            free(next_nodes);
            // Print path
            // printf("    Path: ");
            // for (int i = 0; i < solutions[num_solutions].path_length; i++)
            //{
            //    printf("%d ", solutions[num_solutions].path[i]);
            //}
            // printf("\n");
            return 1;
        }

        // Verifica se o nó já foi visitado
        int was_visited = 0;
        for (int i = 0; i < graph->rows * graph->cols; i++)
        {
            if (visited[i] == next_node)
            {
                was_visited = 1;
                // printf("    Node %d was already visited\n", next_node);
                break;
            }
        }

        if (!was_visited)
        {
            // printf("    Node %d was not visited\n", next_node);
            solutions[num_solutions].path[solutions[num_solutions].path_length++] = next_node;
            for (int i = 0; i < graph->rows * graph->cols; i++)
            {
                if (visited[i] == 0)
                {
                    visited[i] = next_node;
                    // printf("    Added to visited list: %d\n", next_node);
                    break;
                }
            }
            for (int i = 0; i < graph->rows * graph->cols; i++)
            {
                if (visited_total[i] == 0)
                {
                    visited_total[i] = next_node;
                    // printf("    Added to total visited list: %d\n", next_node);
                    break;
                }
            }

            // Atualiza next_nodes com os sucessores do nó atual
            memset(next_nodes, 0, graph->cols * graph->rows * sizeof(int));
            Vertex *current_vertex = getVertexById(graph, next_node);
            for (int i = 0; i < current_vertex->edge_count; i++)
            {
                if (getVertexById(graph, current_vertex->edges[i].target_id)->type != 'c')
                {
                    next_nodes[i] = current_vertex->edges[i].target_id;
                    // printf("    Added to next node list: %d\n", next_nodes[i]);
                }
            }
            // Mostrar caminho até agora
            // printf("    Path so far: ");
            // for (int i = 0; i < solutions[num_solutions].path_length; i++)
            //{
            //    printf("%d ", solutions[num_solutions].path[i]);
            //}
            // printf("\n");
        }
    }
    return 1;
}
