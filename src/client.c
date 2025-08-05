/*
- Criar o socket do cliente
- Conectar ao servidor
- Ler entrada do usuário (ex: "BID 100")
- Enviar mensagens para o servidor
- Receber atualizações (ex: "AUCTION_END João 500")
- Exibir na tela (com ajuda de ui.c)
*/

// src/client.c
#include "../headers/client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

/**
 * Conecta ao servidor com o endereço IP e porta informados.
 *
 * @param ip O endereço IP do servidor.
 * @param porta A porta do servidor.
 *
 * @returns O file descriptor do socket criado.
 *
 * @note Se houver algum erro na cria o do socket, bind ou conex o, fecha o socket e sai do programa com um c digo de erro.
 */
int conectar_ao_servidor(const char *ip, int porta)
{
    int sock;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(porta);

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)
    {
        perror("Endereço inválido");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Erro na conexão");
        exit(EXIT_FAILURE);
    }

    printf("Conectado ao servidor %s:%d\n", ip, porta);
    return sock;
}

/**
 * Executa o loop principal do cliente para comunicação com o servidor.
 *
 * @param socket_fd O descritor de arquivo do socket conectado ao servidor.
 *
 * @details
 * Este loop permite ao cliente enviar mensagens digitadas pelo usuário para o servidor
 * e receber respostas. O loop continua até que o usuário interrompa a execução ou
 * a conexão com o servidor seja encerrada.
 *
 * - Captura a entrada do usuário e envia a mensagem para o servidor.
 * - Lê as respostas do servidor e as exibe no terminal.
 * - Encerra a conexão se o servidor fechar ou se houver erro na leitura.
 */

void loop_cliente(int socket_fd)
{
    char buffer[TAM_BUFFER];
    char entrada[TAM_BUFFER];

    while (1)
    {
        printf("> ");
        if (fgets(entrada, TAM_BUFFER, stdin) == NULL)
            break;

        send(socket_fd, entrada, strlen(entrada), 0);

        int valread = read(socket_fd, buffer, TAM_BUFFER - 1);
        if (valread <= 0)
        {
            printf("Conexão encerrada pelo servidor.\n");
            break;
        }
        buffer[valread] = '\0';
        printf("Servidor: %s", buffer);
    }

    close(socket_fd);
}
