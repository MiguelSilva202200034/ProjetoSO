#ifndef GRAPH_H
#define GRAPH_H

typedef struct Edge
{
    int target_id; // ID do vértice conectado

} Edge;

typedef struct Vertex
{
    int id;         // Identificador único
    int avenue;     // Número da avenida
    int street;     // Número da rua
    char type;      // 's' para supermercado, 'c' para cidadão, ou ' ' para vazio
    Edge *edges;    // Lista de arestas (conexões)
    int edge_count; // Número de arestas
} Vertex;

typedef struct Graph
{
    int rows;          // Número de ruas
    int cols;          // Número de avenidas
    Vertex **vertices; // Matriz de vértices
} Graph;

// Funções para manipulação do grafo
Graph *createGraph(int rows, int cols);

void addVertex(Graph *graph, int avenue, int street, char type);
void displayGraph(Graph *graph);

void freeGraph(Graph *graph);
void listCitizensAndSupermarkets(Graph *graph);

void addEdge(Graph *graph, Vertex *v, int target_id);

Vertex *getCitizens(Graph *graph, int *num_citizens);

Vertex *getSupermarkets(Graph *graph, int *num_supermarkets);
Vertex *getVertexById(Graph *graph, int id);

int areNeighbors(Graph *graph, Vertex *v1, Vertex *v2);
void removeEdge(Graph *graph, Vertex *v, int target_id);
int *getConnectedVertexes(Graph *graph, Vertex v);

#endif
