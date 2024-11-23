#ifndef GRAPH_H
#define GRAPH_H

typedef struct {
    int **adj;       // Ponteiro para matriz de adjacência dinâmica
    int *visited;    // Array para marcar os nós visitados
    int num_vertices;
} Graph;

int init_graph(Graph *g, int num_vertices);
void add_edge(Graph *g, int v1, int v2);
void remove_edge(Graph *g, int v1, int v2);
void free_graph(Graph *g);

#endif // GRAPH_H