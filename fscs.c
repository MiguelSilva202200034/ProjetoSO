#include "fscs.h"
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

extern sem_t sem_graph;
extern sem_t sem_solution;

int random_next_node(int *list, int size) {
    if (size == 0) return -1;
    return list[rand() % size];
}

int find_safe_citizen(Graph *g, Solution *solutions, int *num_solutions, int citizen_id, int start, int end) {
    int current = start;
    int *path = (int *)malloc(g->num_vertices * sizeof(int)); // Caminho dinâmico
    int path_length = 0;

    int queue[g->num_vertices], front = 0, rear = 0;
    queue[rear++] = current;

    while (front < rear) {
        current = queue[front++];
        path[path_length++] = current;

        if (current == end) {
            sem_wait(&sem_solution);
            solutions[*num_solutions].citizen_id = citizen_id;
            solutions[*num_solutions].path = (int *)malloc(path_length * sizeof(int));
            solutions[*num_solutions].path_length = path_length;
            for (int i = 0; i < path_length; i++) {
                solutions[*num_solutions].path[i] = path[i];
            }
            (*num_solutions)++;
            sem_post(&sem_solution);
            free(path); // Libera caminho temporário
            return 1;
        }

        int next_nodes[g->num_vertices];
        int next_count = 0;

        for (int i = 0; i < g->num_vertices; i++) {
            if (g->adj[current][i] && !g->visited[i]) {
                next_nodes[next_count++] = i;
            }
        }

        int next = random_next_node(next_nodes, next_count);
        if (next != -1) {
            sem_wait(&sem_graph);
            g->visited[next] = 1;
            sem_post(&sem_graph);
            queue[rear++] = next;
        }
    }

    free(path);
    return 0;
}

int fscs(Graph *graph, Solution *solutions, int citizens, int supermarkets) {
    int max_solutions = citizens < supermarkets ? citizens : supermarkets;
    int num_solutions = 0;

    for (int i = 0; i < citizens && num_solutions < max_solutions; i++) {
        if (find_safe_citizen(graph, solutions, &num_solutions, i, 0, graph->num_vertices - 1) > 0) {
            sem_wait(&sem_solution);
            num_solutions++;
            sem_post(&sem_solution);
        }
    }

    return num_solutions;
}
