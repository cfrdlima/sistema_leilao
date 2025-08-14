/*
- Criar o socket do servidor (socket, bind, listen, accept)
- Aceitar conexões de múltiplos clientes (com select, poll, ou threads)
- Receber mensagens dos clientes
- Encaminhar para funções da lógica do leilão (auction.c)
- Enviar respostas e atualizações para os clientes
*/

#include "../headers/server.h"
#include "../headers/auction.h"
#include "../headers/users.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>

// Array de clientes
int clientes[MAX_CLIENTES] = {0};

#define MAX_ITENS 5
Item leiloes_disponiveis[MAX_ITENS];
static int leilao_pendente_id = -1;

void inicializar_itens_leilao() {
    // Item 1
    leiloes_disponiveis[0].id = 0;
    strcpy(leiloes_disponiveis[0].nome_item, "Playstation_5");
    leiloes_disponiveis[0].lance_minimo = 2000.0;
    leiloes_disponiveis[0].tempo_duracao = 60;
    leiloes_disponiveis[0].finalizado = 0;

    // Item 2
    leiloes_disponiveis[1].id = 1;
    strcpy(leiloes_disponiveis[1].nome_item, "Fone_de_Ouvido_Bluetooth");
    leiloes_disponiveis[1].lance_minimo = 250.0;
    leiloes_disponiveis[1].tempo_duracao = 60;
    leiloes_disponiveis[1].finalizado = 0;

    // Item 3
    leiloes_disponiveis[2].id = 2;
    strcpy(leiloes_disponiveis[2].nome_item, "Smartwatch");
    leiloes_disponiveis[2].lance_minimo = 700.0;
    leiloes_disponiveis[2].tempo_duracao = 60;
    leiloes_disponiveis[2].finalizado = 0;

    // Item 4
    leiloes_disponiveis[3].id = 3;
    strcpy(leiloes_disponiveis[3].nome_item, "Cadeira_Gamer");
    leiloes_disponiveis[3].lance_minimo = 900.0;
    leiloes_disponiveis[3].tempo_duracao = 60;
    leiloes_disponiveis[3].finalizado = 0;

    // Item 5
    leiloes_disponiveis[4].id = 4;
    strcpy(leiloes_disponiveis[4].nome_item, "Teclado_Mecanico");
    leiloes_disponiveis[4].lance_minimo = 350.0;
    leiloes_disponiveis[4].tempo_duracao = 60;
    leiloes_disponiveis[4].finalizado = 0;
}

void iniciar_servidor() {
    inicializar_usuarios();
    inicializar_itens_leilao();

    int servidor_fd;
    struct sockaddr_in endereco;

    // Criar socket
    servidor_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor_fd == 0) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    // Bind
    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = INADDR_ANY;
    endereco.sin_port = htons(PORTA);

    if (bind(servidor_fd, (struct sockaddr *)&endereco, sizeof(endereco)) < 0) {
        perror("Erro no bind");
        close(servidor_fd);
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(servidor_fd, 3) < 0) {
        perror("Erro no listen");
        close(servidor_fd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor ouvindo na porta %d...\n", PORTA);

    // Loop principal
    tratar_conexoes(servidor_fd);
}

void tratar_conexoes(int servidor_fd) {
    int max_sd, atividade;
    int novo_socket;
    struct sockaddr_in endereco;
    socklen_t addrlen = sizeof(endereco);
    char buffer[TAM_BUFFER];

    fd_set conjunto_leitura;
    struct timeval timeout;

    while (1) {
        FD_ZERO(&conjunto_leitura);
        FD_SET(servidor_fd, &conjunto_leitura);
        max_sd = servidor_fd;

        // Adicionar clientes existentes ao conjunto
        for (int i = 0; i < MAX_CLIENTES; i++) {
            int sd = clientes[i];
            if (sd > 0)
                FD_SET(sd, &conjunto_leitura);
            if (sd > max_sd)
                max_sd = sd;
        }

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        // Esperar por atividade
        atualizar_leilao();

        atividade = select(max_sd + 1, &conjunto_leitura, NULL, NULL, &timeout);

        if ((atividade < 0) && (errno != EINTR)) {
            perror("Erro no select");
            continue;
        }

        // Nova conexão
        if (FD_ISSET(servidor_fd, &conjunto_leitura)) {
            novo_socket = accept(servidor_fd, (struct sockaddr *)&endereco, &addrlen);
            if (novo_socket < 0) {
                perror("Erro no accept");
                continue;
            }

            printf("Novo cliente conectado: socket %d\n", novo_socket);

            // Adicionar cliente à lista
            for (int i = 0; i < MAX_CLIENTES; i++) {
                if (clientes[i] == 0) {
                    clientes[i] = novo_socket;
                    break;
                }
            }
        }

        // Mensagens dos clientes
        for (int i = 0; i < MAX_CLIENTES; i++) {
            int sd = clientes[i];
            if (FD_ISSET(sd, &conjunto_leitura)) {
                int valread = read(sd, buffer, TAM_BUFFER);
                if (valread == 0) {
                    printf("\nCliente %d desconectado\n", sd);
                    registrar_logout(sd);
                    close(sd);
                    clientes[i] = 0;
                } else {
                    buffer[valread] = '\0';
                    printf("\nRecebido do cliente %d: %s", sd, buffer);
                    lidar_com_mensagem(sd, buffer);
                }
            }
        }
    }
}

void lidar_com_mensagem(int cliente_fd, char *mensagem) {
    if (strncmp(mensagem, "PING", 4) == 0) {
        char resposta[] = "PONG\n";
        send(cliente_fd, resposta, strlen(resposta), 0);
    } 
    
    // LOGIN
    else if (strncmp(mensagem, "LOGIN ", 6) == 0) {
        char nome[50], senha[50];
        if (sscanf(mensagem + 6, "%s %s", nome, senha) == 2) {
            if (esta_logado(cliente_fd)) {
                printf("Enviado para o cliente %d: LOGIN_DONE\n", cliente_fd);
                send(cliente_fd, "LOGIN_DONE\n", 11, 0);
            } else if (autenticar_usuario(nome, senha)) {
                registrar_login(nome, cliente_fd);
                printf("Enviado para o cliente %d: LOGIN_OK\n", cliente_fd);
                send(cliente_fd, "LOGIN_OK\n", 9, 0);
            } else {
                printf("Enviado para o cliente %d: LOGIN_FAIL\n", cliente_fd);
                send(cliente_fd, "LOGIN_FAIL\n", 11, 0);
            }
        } else {   
            printf("Enviado para o cliente %d: LOGIN_FAIL\n", cliente_fd);
            send(cliente_fd, "LOGIN_FAIL\n", 11, 0);
        }

    // LOGOUT
    } else if (strncmp(mensagem, "LOGOUT", 6) == 0) {
        registrar_logout(cliente_fd);
        printf("Enviado para o cliente %d: LOGOUT_OK\n", cliente_fd);
        send(cliente_fd, "LOGOUT_OK\n", 10, 0);

    // INFO
    } else if (strncmp(mensagem, "INFO", 4) == 0) {
        const char *user = usuario_por_socket(cliente_fd);
        char mensagem_para_enviar[256]; 
        if (user) {
            printf("Enviado para o cliente %d: INFO_OK %s\n", cliente_fd, user);
            snprintf(mensagem_para_enviar, sizeof(mensagem_para_enviar), "INFO_OK %s\n", user);
            send(cliente_fd, mensagem_para_enviar, strlen(mensagem_para_enviar), 0);
        }
        else {
            printf("Enviado para o cliente %d: INFO_FAIL\n", cliente_fd);
            send(cliente_fd, "INFO_FAIL\n", 10, 0);
        }
    } 
    
    // Listar leiloes
    else if (strncmp(mensagem, "LISTAR_LEILOES", 14) == 0) {
        printf("Enviado para o cliente %d: INICIO_LISTA_LEILOES\n", cliente_fd);
        send(cliente_fd, "INICIO_LISTA_LEILOES\n", 22, 0);
        
        for (int i = 0; i < MAX_ITENS; i++) {
            if (!leiloes_disponiveis[i].finalizado) {
                char item_info[200];

                snprintf(item_info, sizeof(item_info), "ITEM_LEILAO %d %s %.2f\n",
                    leiloes_disponiveis[i].id,
                    leiloes_disponiveis[i].nome_item,
                    leiloes_disponiveis[i].lance_minimo);
                
                printf("Enviado para o cliente %d: ITEM_LEILAO %d\n", cliente_fd, leiloes_disponiveis[i].id);
                send(cliente_fd, item_info, strlen(item_info), 0);
            }
        }
        
        printf("Enviado para o cliente %d: FIM_LISTA_LEILOES\n", cliente_fd);
        send(cliente_fd, "FIM_LISTA_LEILOES\n", 18, 0);
    }

    // Entrar no leilao
    else if (strncmp(mensagem, "ENTRAR_LEILAO ", 14) == 0) {
        const char *usuario = usuario_por_socket(cliente_fd);
        if (!usuario) {
            printf("Enviado para o cliente %d: PRECISA_LOGAR\n", cliente_fd);
            send(cliente_fd, "PRECISA_LOGAR\n", 14, 0);
            return;
        }

        if (participante_ja_esta(usuario)) {
            printf("Enviado para o cliente %d: USUARIO_JA_ESTA_NO_LEILAO\n", cliente_fd);
            send(cliente_fd, "ENTRAR_JA_ESTA_NO_LEILAO\n", 25, 0);
            return;
        }

        int leilao_id;
        if (sscanf(mensagem + 14, "%d", &leilao_id) != 1) {
            printf("Enviado para o cliente %d: ENTRAR_FORMATO_FAIL\n", cliente_fd);
            send(cliente_fd, "ENTRAR_FORMATO_FAIL\n", 20, 0);
            return;
        }

        if (leilao_id < 0 || leilao_id >= MAX_ITENS || leiloes_disponiveis[leilao_id].finalizado) {
            printf("Enviado para o cliente %d: ENTRAR_INDISPONIVEL\n", cliente_fd);
            send(cliente_fd, "ENTRAR_INDISPONIVEL\n", 20, 0);
            return;
        }

        if (get_total_participantes() == 0) {
            leilao_pendente_id = leilao_id;
        } else {
            if (leilao_id != leilao_pendente_id) {
                char aviso[100];
                snprintf(aviso, sizeof(aviso), "ENTRAR_LEILAO_AGUARDANDO %d\n", leilao_pendente_id);
                printf("Enviado para o cliente %d: ENTRAR_LEILAO_AGUARDANDO\n", cliente_fd);
                send(cliente_fd, aviso, strlen(aviso), 0);
                return;
            }
        }

        if (adicionar_participante(usuario, cliente_fd)) {
            if (get_total_participantes() >= 2) {
                Item item_a_leiloar = leiloes_disponiveis[leilao_pendente_id];
                leiloes_disponiveis[leilao_pendente_id].finalizado = 1;

                inicializar_leilao(item_a_leiloar);
                printf("Leilão do item '%s' (ID: %d) iniciado com %d participantes.\n", item_a_leiloar.nome_item, leilao_pendente_id, get_total_participantes());

                enviar_inicio_leilao();
                leilao_pendente_id = -1;
            } else {
                printf("Enviado para o cliente %d: ENTRAR_AGUARDE\n", cliente_fd);
                send(cliente_fd, "ENTRAR_AGUARDE\n", 55, 0);
            }
        } 
    } 
    
    // Lances
    else if (strncmp(mensagem, "LANCE ", 6) == 0) {
        const char *usuario = usuario_por_socket(cliente_fd);
        if (!usuario) {
            printf("Enviado para o cliente %d: PRECISA_LOGAR\n", cliente_fd);
            send(cliente_fd, "PRECISA_LOGAR\n", 14, 0);
            return;
        } else if (!leilao_ativo()) {
            printf("Enviado para o cliente %d: LANCE_NAO_ATIVO\n", cliente_fd);
            send(cliente_fd, "LANCE_NAO_ATIVO\n", 16, 0);
        } else {
            float valor;
            if (sscanf(mensagem + 6, "%f", &valor) == 1)
            {
                int resultado = registrar_lance(usuario, valor);
                if (resultado == 1)
                {
                    char aviso[100];
                    snprintf(aviso, sizeof(aviso), "LANCE_NOVO %s %.2f\n", usuario, valor);

                    for (int i = 0; i < get_total_participantes(); i++)
                    {
                        int fd = get_participante_fd(i);
                        if (fd != -1) {
                            printf("Enviado para o cliente %d: LANCE_NOVO\n", fd);
                            send(fd, aviso, strlen(aviso), 0);
                        }
                    }
                }
                else if (resultado == 0)
                {
                    printf("Enviado para o cliente %d: LANCE_REJEITADO\n", cliente_fd);
                    send(cliente_fd, "LANCE_REJEITADO\n", 17, 0);
                }
            }
            else
            {
                printf("Enviado para o cliente %d: LANCE_FORMATO_FAIL\n", cliente_fd);
                send(cliente_fd, "LANCE_FORMATO_FAIL\n", 19, 0);
            }
        }
    }

    // Comandos nao existentes
    else {
        char resposta[] = "COMANDO_FAIL\n";
        printf("Enviado para o cliente %d: COMANDO_FAIL\n", cliente_fd);
        send(cliente_fd, resposta, strlen(resposta), 0);
    }
}