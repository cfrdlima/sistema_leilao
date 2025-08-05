/*
- Lógica do leilão no servidor
- Iniciar novo leilão
- Controlar tempo restante
- Validar lances
- Determinar vencedor
*/

#include "../headers/auction.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

static int participantes_fd[MAX_PARTICIPANTES];
static char participantes_nome[MAX_PARTICIPANTES][50];
static int num_participantes = 0;
static Item item_atual;

void inicializar_leilao(Item item)
{
    item_atual = item;
    num_participantes = 0;
}

int adicionar_participante(const char *usuario, int socket_fd)
{
    if (num_participantes >= MAX_PARTICIPANTES)
        return 0;

    participantes_fd[num_participantes] = socket_fd;
    strncpy(participantes_nome[num_participantes], usuario, sizeof(participantes_nome[0]));
    num_participantes++;
    return 1;
}

int total_participantes()
{
    return num_participantes;
}

void enviar_inicio_leilao()
{
    char msg[256];
    snprintf(msg, sizeof(msg),
             "LEILAO_INICIO %s %.2f %d\n",
             item_atual.nome_item,
             item_atual.lance_minimo,
             item_atual.tempo_duracao);

    for (int i = 0; i < num_participantes; i++)
    {
        send(participantes_fd[i], msg, strlen(msg), 0);
    }

    printf("Leilão iniciado com %d participantes.\n", num_participantes);
}
