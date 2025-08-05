#ifndef AUCTION_H
#define AUCTION_H

#define MAX_PARTICIPANTES 10

typedef struct
{
    char nome_item[100];
    float lance_minimo;
    int tempo_duracao; // segundos
} Item;

void inicializar_leilao(Item item);
int adicionar_participante(const char *usuario, int socket_fd);
int total_participantes();
void enviar_inicio_leilao();

#endif
