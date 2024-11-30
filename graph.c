#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

// Cria um grafo com base na grade (avenidas x ruas)
Graph *createGraph(int rows, int cols)
{
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    graph->rows = rows;
    graph->cols = cols;

    // Alocação da matriz de vértices
    graph->vertices = (Vertex **)malloc(rows * sizeof(Vertex *));
    for (int i = 0; i < rows; i++)
    {
        graph->vertices[i] = (Vertex *)malloc(cols * sizeof(Vertex));
    }

    // Inicialização dos vértices
    int id = 1;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            graph->vertices[i][j].id = id++;
            graph->vertices[i][j].avenue = i + 1;
            graph->vertices[i][j].street = j + 1;
            graph->vertices[i][j].type = ' ';
            graph->vertices[i][j].edges = NULL; // Nenhuma aresta inicialmente
            graph->vertices[i][j].edge_count = 0;
        }
    }

    // Conexões automáticas entre vértices adjacentes
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            Vertex *v = &graph->vertices[i][j];

            // Conecta ao vértice ao norte
            if (i > 0)
                addEdge(graph, v, graph->vertices[i - 1][j].id);
            // Conecta ao vértice ao sul
            if (i < rows - 1)
                addEdge(graph, v, graph->vertices[i + 1][j].id);
            // Conecta ao vértice a este
            if (j < cols - 1)
                addEdge(graph, v, graph->vertices[i][j + 1].id);
            // Conecta ao vértice à oeste
            if (j > 0)
                addEdge(graph, v, graph->vertices[i][j - 1].id);
        }
    }

    return graph;
}

// Adiciona um vértice do tipo especificado ('c' ou 's') apenas se as coordenadas forem válidas
void addVertex(Graph *graph, int avenue, int street, char type)
{
    if (street <= graph->rows && avenue <= graph->cols)
    {
        int i = street - 1;
        int j = avenue - 1;
        graph->vertices[i][j].type = type;
    }
}

// Exibe o grafo na forma de grade
void displayGraph(Graph *graph)
{
    printf("Graph Representation (Avenues x Streets):\n");

    for (int i = 0; i < graph->rows; i++)
    {
        // Exibe os vértices na linha atual
        for (int j = 0; j < graph->cols; j++)
        {
            Vertex *v = &graph->vertices[i][j];
            printf("[%d - %c]", v->id, v->type);

            // Verifica conexão à direita (leste)
            if (j < graph->cols - 1)
            {
                int connected = 0;
                for (int k = 0; k < v->edge_count; k++)
                {
                    if (v->edges[k].target_id == graph->vertices[i][j + 1].id)
                    {
                        connected = 1;
                        break;
                    }
                }
                printf("%s", connected ? " -" : "  ");
            }
        }
        printf("\n");

        // Exibe conexões para a próxima linha (norte-sul)
        if (i < graph->rows - 1)
        {
            for (int j = 0; j < graph->cols; j++)
            {
                Vertex *v = &graph->vertices[i][j];
                int connected = 0;
                for (int k = 0; k < v->edge_count; k++)
                {
                    if (v->edges[k].target_id == graph->vertices[i + 1][j].id)
                    {
                        connected = 1;
                        break;
                    }
                }
                printf("%s ", connected ? "| " : "  ");
                if (j < graph->cols - 1)
                    printf("  ");
            }
            printf("\n");
        }
    }

    // Exibe as conexões detalhadas entre os vértices
    printf("\nVertex Connections:\n");
    for (int i = 0; i < graph->rows; i++)
    {
        for (int j = 0; j < graph->cols; j++)
        {
            Vertex *v = &graph->vertices[i][j];
            printf("Vertex %d (Avenue %d, Street %d) connects to: ", v->id, v->avenue, v->street);
            for (int k = 0; k < v->edge_count; k++)
            {
                printf("%d ", v->edges[k].target_id);
            }
            printf("\n");
        }
    }
}

// Libera a memória alocada para o grafo
void freeGraph(Graph *graph)
{
    // Libera as arestas de cada vértice

    // Libera as linhas da matriz de vértices
    for (int i = 0; i < graph->rows; i++)
    {
        free(graph->vertices[i]);
    }

    // Libera a matriz de vértices
    free(graph->vertices);

    // Libera a estrutura do grafo
    free(graph);
}

void listCitizensAndSupermarkets(Graph *graph)
{
    int num_citizens, num_supermarkets = 0;
    Vertex *citizens = getCitizens(graph, &num_citizens);
    Vertex *supermarkets = getSupermarkets(graph, &num_supermarkets);

    printf("Citizens:\n");
    for (int i = 0; i < num_citizens; i++)
    {
        printf(" - Avenue %d, Street %d (ID: %d)\n",
               citizens[i].avenue,
               citizens[i].street,
               citizens[i].id);
    }

    printf("\nSupermarkets:\n");
    for (int i = 0; i < num_supermarkets; i++)
    {
        printf(" - Avenue %d, Street %d (ID: %d)\n",
               supermarkets[i].avenue,
               supermarkets[i].street,
               supermarkets[i].id);
    }

    free(citizens);
    free(supermarkets);
}

void addEdge(Graph *graph, Vertex *v, int target_id)
{
    v->edges = (Edge *)realloc(v->edges, (v->edge_count + 1) * sizeof(Edge));
    v->edges[v->edge_count].target_id = target_id;
    v->edge_count++;
}

void removeEdge(Graph *graph, Vertex *v, int target_id)
{
    for (int i = 0; i < v->edge_count; i++)
    {
        if (v->edges[i].target_id == target_id)
        {
            for (int j = i; j < v->edge_count - 1; j++)
            {
                v->edges[j] = v->edges[j + 1];
            }
            v->edge_count--;
            v->edges = (Edge *)realloc(v->edges, v->edge_count * sizeof(Edge));
            break;
        }
    }
}

Vertex *getCitizens(Graph *graph, int *num_citizens)
{
    Vertex *citizens = (Vertex *)malloc(graph->rows * graph->cols * sizeof(Vertex));
    int count = 0;
    *num_citizens = 0;
    for (int i = 0; i < graph->rows; i++)
    {
        for (int j = 0; j < graph->cols; j++)
        {
            if (graph->vertices[i][j].type == 'c')
            {
                citizens[count] = graph->vertices[i][j];
                count++;
                (*num_citizens)++;
            }
        }
    }
    return citizens;
}

Vertex *getSupermarkets(Graph *graph, int *num_supermarkets)
{
    Vertex *supermarkets = (Vertex *)malloc(graph->rows * graph->cols * sizeof(Vertex));
    int count = 0;
    *num_supermarkets = 0;
    for (int i = 0; i < graph->rows; i++)
    {
        for (int j = 0; j < graph->cols; j++)
        {
            if (graph->vertices[i][j].type == 's')
            {
                supermarkets[count] = graph->vertices[i][j];
                count++;
                (*num_supermarkets)++;
            }
        }
    }
    return supermarkets;
}

Vertex *getVertexById(Graph *graph, int id)
{
    for (int i = 0; i < graph->rows; i++)
    {
        for (int j = 0; j < graph->cols; j++)
        {
            if (graph->vertices[i][j].id == id)
            {
                return &graph->vertices[i][j];
            }
        }
    }
    return NULL; // ID não encontrado
}

int areNeighbors(Graph *graph, Vertex *v1, Vertex *v2)
{
    for (int i = 0; i < v1->edge_count; i++)
    {
        if (v1->edges[i].target_id == v2->id || v2->edges[i].target_id == v1->id)
        {
            return 1;
        }
    }
    return 0;
}

int *getConnectedVertexes(Graph *graph, Vertex v)
{
    if (v.edge_count == 0)
    {
        return NULL; // Nenhum vértice conectado
    }

    // Aloca memória para armazenar os IDs dos vértices conectados
    int *connected = (int *)malloc(v.edge_count * sizeof(int));
    if (connected == NULL)
    {
        printf("Error: Memory allocation failed in getConnectedVertexes.\n");
        exit(1);
    }

    // Copia os IDs das arestas conectadas
    for (int i = 0; i < v.edge_count; i++)
    {
        connected[i] = v.edges[i].target_id;
    }

    return connected;
}
