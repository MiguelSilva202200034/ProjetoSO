
typedef struct Solution {
    int citizen_id;
    int* path;
    int path_length;
} Solution;


int fscs(Graph *graph, Solution **solutions);

int findSafeCitizen(Graph *graph, Vertex *citizen, int *visited_total, int *visited, Solution **solutions, int num_solutions);