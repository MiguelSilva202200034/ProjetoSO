#ifndef FSCS_H
#define FSCS_H

#include "graph.h"

typedef struct {
    int citizen_id;
    int *path;       // Ponteiro para armazenar o caminho dinâmico
    int path_length; // Tamanho do caminho
} Solution;

int find_safe_citizen(Graph *g, Solution *solutions, int *num_solutions, int citizen_id, int start, int end);
int fscs(Graph *graph, Solution *solutions, int citizens, int supermarkets);

#endif // FSCS_H