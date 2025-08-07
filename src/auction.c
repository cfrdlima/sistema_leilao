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
#include <sys/socket.h>

static int participantes_fd[MAX_PARTICIPANTES];
static char participantes_nome[MAX_PARTICIPANTES][50];
static int num_participantes = 0;
static Item item_atual;
static float maior_lance = 0;
static char vencedor[50] = "";
static int ativo = 0;

void inicializar_leilao(Item item)
{
    item_atual = item;
    num_participantes = 0;
    maior_lance = item.lance_minimo;
    strcpy(vencedor, "Ninguém");
    ativo = 1;
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

int registrar_lance(const char *usuario, float valor)
{
    if (!ativo)
        return -1;

    if (valor > maior_lance)
    {
        maior_lance = valor;
        strncpy(vencedor, usuario, sizeof(vencedor));
        return 1; // aceito
    }
    else
    {
        return 0; // rejeitado
    }
}

float obter_maior_lance()
{
    return maior_lance;
}

const char *obter_vencedor()
{
    return vencedor;
}

int leilao_ativo()
{
    return ativo;
}

int get_participante_fd(int i)
{
    if (i >= 0 && i < num_participantes)
        return participantes_fd[i];
    return -1;
}

int get_total_participantes()
{
    return num_participantes;
}
