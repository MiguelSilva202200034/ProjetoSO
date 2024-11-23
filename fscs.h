#ifndef FSCS_H
#define FSCS_H

#include "graph.h"

typedef struct {
    int citizen_id;
    int path[MAX];
    int path_length;
} Solution;

int find_safe_citizen(Graph *g, Solution *solutions, int *num_solutions, int citizen_id, int start, int end);

int fscs(Graph *graph, Solution *solutions, int citizens, int supermarkets);

#endif // FSCS_H