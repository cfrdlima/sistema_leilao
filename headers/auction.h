#ifndef AUCTION_H
#define AUCTION_H

#define MAX_PARTICIPANTES 10

typedef struct
{
    int id;
    char nome_item[100];
    float lance_minimo;
    int tempo_duracao; // segundos
    int participantes; 
    int finalizado;
} Item;

void inicializar_leilao(Item item);
int adicionar_participante(const char *usuario, int socket_fd);
int total_participantes();
void enviar_inicio_leilao();
int registrar_lance(const char *usuario, float valor);
float obter_maior_lance();
const char *obter_vencedor();
int leilao_ativo();
int get_participante_fd(int i);
int get_total_participantes();
void atualizar_leilao();
void encerrar_leilao();
int leilao_foi_encerrado();
int participante_ja_esta(const char *usuario);

#endif
