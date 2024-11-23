#include "graph.h"
#include <stdlib.h>
#include <string.h>

int init_graph(Graph *g, int num_vertices) {
    g->num_vertices = num_vertices;

    // Aloca memória para matriz de adjacência
    g->adj = (int **)malloc(num_vertices * sizeof(int *));
    for (int i = 0; i < num_vertices; i++) {
        g->adj[i] = (int *)calloc(num_vertices, sizeof(int));
    }

    // Aloca memória para array de visitados
    g->visited = (int *)calloc(num_vertices, sizeof(int));

    return num_vertices;
}

void add_edge(Graph *g, int v1, int v2) {
    if (v1 >= 0 && v1 < g->num_vertices && v2 >= 0 && v2 < g->num_vertices) {
        g->adj[v1][v2] = 1;
        g->adj[v2][v1] = 1; // Bidirectional
    }
}

void remove_edge(Graph *g, int v1, int v2) {
    g->adj[v1][v2] = 0;
    g->adj[v2][v1] = 0;
}

void free_graph(Graph *g) {
    for (int i = 0; i < g->num_vertices; i++) {
        free(g->adj[i]);
    }
    free(g->adj);
    free(g->visited);
}