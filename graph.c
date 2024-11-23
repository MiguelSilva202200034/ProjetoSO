#include "graph.h"
#include <string.h>

void init_graph(Graph *g) {
    memset(g, 0, sizeof(Graph));
}

void add_edge(Graph *g, int v1, int v2) {
    g->adj[v1][v2] = 1;
    g->adj[v2][v1] = 1; // Bidirecional
    g->num_vertices++;
}

void remove_edge(Graph *g, int v1, int v2) {
    g->adj[v1][v2] = 0;
    g->adj[v2][v1] = 0;
    g->num_vertices--;
}