#include "fscs.h"
#include <semaphore.h>
#include <stdlib.h>

extern sem_t sem_graph;
extern sem_t sem_solution;


int fscs(Graph *graph, Solution *solutions, int citizens, int supermarkets) {
    int max_solutions = citizens < supermarkets ? citizens : supermarkets;
    int num_solutions = 0;

    for (int i = 0; i < citizens && num_solutions < max_solutions; i++) {
        // Invoca o algoritmo Find Safe Citizen
        int result = find_safe_citizen(graph, solutions, &num_solutions, i, 1, graph->num_vertices - 1);

        if (result > 0) {
            sem_wait(&sem_solution);
            num_solutions++;
            sem_post(&sem_solution);
        }
    }
    return num_solutions;
}

int find_safe_citizen(Graph *g, Solution *solutions, int *num_solutions, int citizen_id, int start, int end) {
    int current = start;
    int path[MAX];
    int path_length = 0;

    int queue[MAX], front = 0, rear = 0;
    queue[rear++] = current;

    while (front < rear) {
        current = queue[front++];
        path[path_length++] = current;

        if (current == end) {
            sem_wait(&sem_solution);
            solutions[*num_solutions] = (Solution){citizen_id, {0}, path_length};
            for (int i = 0; i < path_length; i++) {
                solutions[*num_solutions].path[i] = path[i];
            }
            (*num_solutions)++;
            sem_post(&sem_solution);
            return 1;
        }

        for (int i = 0; i < MAX; i++) {
            if (g->adj[current][i] && !g->visited[i]) {
                sem_wait(&sem_graph);
                g->visited[i] = 1;
                sem_post(&sem_graph);
                queue[rear++] = i;
            }
        }
    }
    return 0;
}