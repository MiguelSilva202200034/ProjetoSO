#include <stdio.h>
#include <stdlib.h>
#include "input.h"

int read_input(const char *filename, int *num_avenues, int *num_streets, 
                int *num_supermarkets, int *num_citizens, 
                Location **supermarkets, Location **citizens) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o ficheiro");
        return -1;  // Retorna erro se não conseguir abrir o arquivo
    }

    // Ler o número de avenidas e ruas
    if (num_avenues == NULL || num_streets == NULL) {
        fprintf(stderr, "Ponteiros num_avenues e num_streets são nulos!\n");
        fclose(file);
        return -1;
    }
    fscanf(file, "%d %d", num_avenues, num_streets);

    // Ler o número de supermercados e cidadãos
    if (num_supermarkets == NULL || num_citizens == NULL) {
        fprintf(stderr, "Ponteiros num_supermarkets e num_citizens são nulos!\n");
        fclose(file);
        return -1;
    }
    fscanf(file, "%d %d", num_supermarkets, num_citizens);

    // Alocar memória para as localizações de supermercados e cidadãos
    *supermarkets = (Location *)malloc(*num_supermarkets * sizeof(Location));
    *citizens = (Location *)malloc(*num_citizens * sizeof(Location));

    if (*supermarkets == NULL || *citizens == NULL) {
        fprintf(stderr, "Falha na alocação de memória para supermercados ou cidadãos!\n");
        fclose(file);
        return -1;
    }

    // Ler as localizações dos supermercados
    for (int i = 0; i < *num_supermarkets; i++) {
        fscanf(file, "%d %d", &(*supermarkets)[i].avenue, &(*supermarkets)[i].street);
    }

    // Ler as localizações dos cidadãos
    for (int i = 0; i < *num_citizens; i++) {
        fscanf(file, "%d %d", &(*citizens)[i].avenue, &(*citizens)[i].street);
    }

    fclose(file);
    return 0;  // Retorna 0 se tudo correr bem
}