#ifndef INPUT_H
#define INPUT_H
// Estrutura para representar uma localização (avenida e rua)
typedef struct
{
    int avenue;
    int street;
} Location;
// Função para ler os dados de entrada do arquivo
int read_input(const char *filename, int *num_avenues, int *num_streets,
               int *num_supermarkets, int *num_citizens, Location **supermarkets, Location **citizens);
#endif