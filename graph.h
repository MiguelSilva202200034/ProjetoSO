#ifndef GRAPH_H
#define GRAPH_H

#define MAX 100

typedef struct {
    int adj[MAX][MAX]; // Matriz de adjacência
    int visited[MAX];  // Marca os vértices visitados
    int num_vertices;
} Graph;

void init_graph(Graph *g);
void add_edge(Graph *g, int v1, int v2);
void remove_edge(Graph *g, int v1, int v2);

#endif // GRAPH_H***