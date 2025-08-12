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
#include <time.h>

static int participantes_fd[MAX_PARTICIPANTES];
static char participantes_nome[MAX_PARTICIPANTES][50];
static int num_participantes = 0;
static Item item_atual;
static float maior_lance = 0;
static char vencedor[50] = "";
static int ativo = 0;
static time_t tempo_inicio;
static int encerrado = 0;

/**
 * Inicializa um novo leilão com o item especificado.
 *
 * Este método limpa o estado do leilão anterior e configura o novo leilão
 * com o item especificado. O lance mínimo, o tempo de duração e o nome do
 * item são copiados. O vencedor é inicializado como "Ninguém" e o estado do
 * leilão é setado como ativo.
 *
 * @param item Item a ser leiloado
 */
void inicializar_leilao(Item item)
{
    item_atual = item;
    num_participantes = 0;
    maior_lance = item.lance_minimo;
    strcpy(vencedor, "Ninguém");
    ativo = 1;
    encerrado = 0;
    tempo_inicio = time(NULL);
}

/**
 * Adiciona um participante no leilão.
 *
 * Este método adiciona um participante ao leilão atual. O participante é identificado
 * pelo seu nome de usuário e pelo descritor de arquivo do socket de comunicação com o
 * cliente. A função retorna 1 se o participante for adicionado com sucesso e 0 se já
 * houver o número máximo de participantes.
 *
 * @param usuario Nome de usuário do participante
 * @param socket_fd Descritor de arquivo do socket de comunicação com o participante
 * @return 1 se o participante for adicionado com sucesso, 0 caso contrário
 */
int adicionar_participante(const char *usuario, int socket_fd)
{
    if (num_participantes >= MAX_PARTICIPANTES)
        return 0;

    participantes_fd[num_participantes] = socket_fd;
    strncpy(participantes_nome[num_participantes], usuario, sizeof(participantes_nome[0]));
    num_participantes++;
    return 1;
}

/**
 * Retorna o número total de participantes atualmente no leilão.
 *
 * @return Número de participantes
 */
int total_participantes()
{
    return num_participantes;
}

/**
 * Envia a mensagem de início do leilão para todos os participantes.
 *
 * A mensagem enviada é do tipo "LEILAO_INICIO <nome_item> <lance_minimo> <tempo_duracao>" e
 * é enviada para todos os participantes adicionados com adicionar_participante.
 *
 * @see adicionar_participante
 */
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

/**
 * Registra um lance de um participante no leilão.
 *
 * A função verifica se o leilão está ativo e se o lance é maior que o lance atual.
 * Se o lance for maior, o nome do participante é gravado como vencedor e o lance é
 * gravado como o maior lance. A função retorna 1 se o lance for aceito e 0 se for
 * rejeitado.
 *
 * Se o leilão não estiver ativo, a função retorna -1.
 *
 * @param usuario Nome do participante que fez o lance
 * @param valor Valor do lance
 * @return 1 se o lance for aceito, 0 se for rejeitado, -1 se o leilão não estiver ativo
 */
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

/**
 * Atualiza o estado do leilão verificando o tempo decorrido.
 *
 * Se o leilão estiver ativo e não encerrado, calcula o tempo decorrido desde o início.
 * Se o tempo decorrido for maior ou igual ao tempo de duração do item, encerra o leilão.
 */

void atualizar_leilao()
{
    if (!ativo || encerrado)
        return;

    time_t agora = time(NULL);
    double elapsed = difftime(agora, tempo_inicio);

    if (elapsed >= item_atual.tempo_duracao)
    {
        encerrar_leilao();
    }
}

/**
 * Encerra o leilão atual e notifica os participantes.
 *
 * Muda o estado do leilão para "encerrado" e envia uma mensagem para
 * todos os participantes informando o vencedor e o lance vencedor.
 * Limpa o estado do leilão para novo uso.
 */
void encerrar_leilao()
{
    ativo = 0;
    encerrado = 1;

    char fim[100];
    snprintf(fim, sizeof(fim),
             "LEILAO_FIM %s %.2f\n",
             vencedor,
             maior_lance);

    for (int i = 0; i < num_participantes; i++)
    {
        send(participantes_fd[i], fim, strlen(fim), 0);
    }

    printf("Leilão encerrado. Vencedor: %s (%.2f)\n", vencedor, maior_lance);

    // Limpa estado
    num_participantes = 0;
    maior_lance = 0;
    vencedor[0] = '\0';
}

int leilao_foi_encerrado()
{
    return encerrado;
}

int participante_ja_esta(const char *usuario)
{
    for (int i = 0; i < num_participantes; i++)
    {
        if (strcmp(participantes_nome[i], usuario) == 0)
            return 1;
    }
    return 0;
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
