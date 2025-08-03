/*
- Criar o socket do servidor (socket, bind, listen, accept)
- Aceitar conexões de múltiplos clientes (com select, poll, ou threads)
- Receber mensagens dos clientes
- Encaminhar para funções da lógica do leilão (auction.c)
- Enviar respostas e atualizações para os clientes
*/

#include "../headers/server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

// Array de clientes
int clientes[MAX_CLIENTES] = {0};

void iniciar_servidor()
{
    int servidor_fd;
    struct sockaddr_in endereco;

    // Criar socket
    servidor_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor_fd == 0)
    {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    // Bind
    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = INADDR_ANY;
    endereco.sin_port = htons(PORTA);

    if (bind(servidor_fd, (struct sockaddr *)&endereco, sizeof(endereco)) < 0)
    {
        perror("Erro no bind");
        close(servidor_fd);
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(servidor_fd, 3) < 0)
    {
        perror("Erro no listen");
        close(servidor_fd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor ouvindo na porta %d...\n", PORTA);

    // Loop principal
    tratar_conexoes(servidor_fd);
}

void tratar_conexoes(int servidor_fd)
{
    int max_sd, atividade;
    int novo_socket;
    struct sockaddr_in endereco;
    socklen_t addrlen = sizeof(endereco);
    char buffer[TAM_BUFFER];

    fd_set conjunto_leitura;

    while (1)
    {
        FD_ZERO(&conjunto_leitura);
        FD_SET(servidor_fd, &conjunto_leitura);
        max_sd = servidor_fd;

        // Adicionar clientes existentes ao conjunto
        for (int i = 0; i < MAX_CLIENTES; i++)
        {
            int sd = clientes[i];
            if (sd > 0)
                FD_SET(sd, &conjunto_leitura);
            if (sd > max_sd)
                max_sd = sd;
        }

        // Esperar por atividade
        atividade = select(max_sd + 1, &conjunto_leitura, NULL, NULL, NULL);
        if ((atividade < 0) && (errno != EINTR))
        {
            perror("Erro no select");
            continue;
        }

        // Nova conexão
        if (FD_ISSET(servidor_fd, &conjunto_leitura))
        {
            novo_socket = accept(servidor_fd, (struct sockaddr *)&endereco, &addrlen);
            if (novo_socket < 0)
            {
                perror("Erro no accept");
                continue;
            }

            printf("Novo cliente conectado: socket %d\n", novo_socket);

            // Adicionar cliente ao array
            for (int i = 0; i < MAX_CLIENTES; i++)
            {
                if (clientes[i] == 0)
                {
                    clientes[i] = novo_socket;
                    break;
                }
            }
        }

        // Mensagens dos clientes
        for (int i = 0; i < MAX_CLIENTES; i++)
        {
            int sd = clientes[i];
            if (FD_ISSET(sd, &conjunto_leitura))
            {
                int valread = read(sd, buffer, TAM_BUFFER);
                if (valread == 0)
                {
                    printf("Cliente %d desconectado\n", sd);
                    close(sd);
                    clientes[i] = 0;
                }
                else
                {
                    buffer[valread] = '\0';
                    printf("Recebido do cliente %d: %s\n", sd, buffer);
                    lidar_com_mensagem(sd, buffer);
                }
            }
        }
    }
}

void lidar_com_mensagem(int cliente_fd, char *mensagem)
{
    // Lógica inicial de resposta simples
    if (strncmp(mensagem, "PING", 4) == 0)
    {
        char resposta[] = "PONG\n";
        send(cliente_fd, resposta, strlen(resposta), 0);
    }
    else
    {
        char resposta[] = "Comando não reconhecido\n";
        send(cliente_fd, resposta, strlen(resposta), 0);
    }
}
