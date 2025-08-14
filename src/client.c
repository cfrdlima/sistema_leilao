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
#include <sys/select.h>
#include <errno.h>

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
    // Enviar mensagem de boas-vindas
    char boas_vindas[] =
        "Bem-vindo ao sistema de leilão!\n"
        "Comandos disponíveis:\n"
        "  - LOGIN <nome> <senha>\n"
        "  - INFO\n"
        "  - LISTAR_LEILOES\n"
        "  - LOGOUT\n"
        "  - ENTRAR_LEILAO <id>\n\n";

    printf("%s", boas_vindas);

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
    fd_set read_fds;
    int max_sd;

    // Defina o file descriptor do standard input (teclado)
    int stdin_fd = STDIN_FILENO; 

    while (1)
    {
        FD_ZERO(&read_fds); // Limpa o conjunto de descritores

        // Adiciona o teclado (stdin) ao conjunto
        FD_SET(stdin_fd, &read_fds);

        // Adiciona o socket do servidor ao conjunto
        FD_SET(socket_fd, &read_fds);

        // Determina o maior descritor de arquivo para o select()
        max_sd = (socket_fd > stdin_fd) ? socket_fd : stdin_fd;

        // Imprime o prompt para o usuário
        printf("> ");
        fflush(stdout); // Garante que o ">" apareça imediatamente

        // Espera por atividade em um dos descritores (teclado ou socket)
        int activity = select(max_sd + 1, &read_fds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            perror("select error");
            break;
        }

        // 1. Checa se há atividade no socket do servidor (mensagem recebida)
        if (FD_ISSET(socket_fd, &read_fds)) {
            int valread = read(socket_fd, buffer, TAM_BUFFER - 1);
            if (valread <= 0)
            {
                printf("\nConexão encerrada pelo servidor.\n");
                break;
            }

            buffer[valread] = '\0';
            
            char *linha = strtok(buffer, "\n");
            while (linha != NULL)
            {
                char tipo_msg_temp[100];
                if (sscanf(linha, "%99s", tipo_msg_temp) != 1) {
                    linha = strtok(NULL, "\n");
                    continue; 
                }

                // LOGIN e LOGOUT
                if (strcmp(tipo_msg_temp, "LOGIN_OK") == 0) {
                    printf("\rServidor: Bem Vindo!\n");
                } else if (strcmp(tipo_msg_temp, "LOGIN_DONE") == 0) {
                    printf("\rServidor: Voce ja esta logado!\n");
                } else if (strcmp(tipo_msg_temp, "LOGIN_FAIL") == 0) {
                    printf("\rServidor: Erro ao logar\n");
                } else if (strcmp(tipo_msg_temp, "LOGOUT_OK") == 0) {
                    printf("\rServidor: Logout feito. Volte sempre!\n");
                } else if (strcmp(tipo_msg_temp, "PRECISA_LOGAR") == 0) {
                    printf("\rServidor: Voce deve estar logado para entrar.\n");
                }

                // INFO
                else if (strcmp(tipo_msg_temp, "INFO_OK") == 0) {
                    char user_name[50];
                    sscanf(linha, "%*s %49s", user_name);
                    printf("\rServidor: Login ativo: %s\n", user_name);
                } else if (strcmp(tipo_msg_temp, "INFO_FAIL") == 0) {
                    printf("\rServidor: Nao ha um login ativo no momento!\n");
                } 
                
                // LISTAR LEILOES
                else if (strcmp(tipo_msg_temp, "INICIO_LISTA_LEILOES") == 0) {
                    printf("\r=== Leilões Disponíveis ===\n");
                } else if (strcmp(tipo_msg_temp, "ITEM_LEILAO") == 0) {
                    int id;
                    char nome[50];
                    float lance;

                    int itens_lidos = sscanf(linha, "%*s %d %49s %f", &id, nome, &lance);
                    if (itens_lidos == 3) {
                        printf("\r---------------------------\n");
                        printf("  ID %d: %s\n  (Lance Mínimo: R$%.2f)\n", id, nome, lance);
                    }
                } else if (strcmp(tipo_msg_temp, "FIM_LISTA_LEILOES") == 0) {
                    printf("\r---------------------------\n");
                } 
                 
                // ENTRAR NO LEILAO
                else if (strcmp(tipo_msg_temp, "ENTRAR_JA_ESTA_NO_LEILAO") == 0) {
                    printf("\rServidor: Voce ja esta participando deste leilao, aguarde.\n");
                } else if (strcmp(tipo_msg_temp, "ENTRAR_FORMATO_FAIL") == 0) {
                    printf("\rServidor: Comando errado, entre com ENTRAR_LEILAO <id>.\n");
                } else if (strcmp(tipo_msg_temp, "ENTRAR_INDISPONIVEL") == 0) {
                    printf("\rServidor: Este leilao nao esta disponivel.\n");
                } else if (strcmp(tipo_msg_temp, "ENTRAR_LEILAO_AGUARDANDO") == 0) {
                    int id;
                    
                    int itens_lidos = sscanf(linha, "%*s %d", &id);

                    if (itens_lidos == 1) {
                        printf("\rServidor: Entre no leilao pendente. ID: %d\n", id);
                    }
                } else if (strcmp(tipo_msg_temp, "ENTRAR_AGUARDE") == 0) {
                    printf("\rServidor: Voce entrou no leilao. Aguarde a entrada dos outros participantes\n");
                } 
                
                // INICIO DO LEILAO E LANCES
                else if (strcmp(tipo_msg_temp, "INICIAR_LEILAO") == 0) {
                    char nome[50];
                    float lance_min;
                    int duracao;

                    int itens_lidos = sscanf(linha, "%*s %49s %f %d", nome, &lance_min, &duracao);
                    if (itens_lidos == 3) {
                        printf("\r\n====================================\n");
                        printf("\r      LEILÃO INICIADO!\n");
                        printf("\r------------------------------------\n");
                        printf("\r  Item: %s\n", nome);
                        printf("\r  Lance Inicial: R$ %.2f\n", lance_min);
                        printf("\r  Duração: %d segundos\n", duracao);
                        printf("\r====================================\n");
                        printf("\rFaça seu lance com: LANCE <valor>\n");
                    }
                } else if (strcmp(tipo_msg_temp, "LANCE_NAO_ATIVO") == 0) {
                    printf("\rServidor: Voce deve entrar em um leilao ativo para enviar lances.\n");
                } else if (strcmp(tipo_msg_temp, "LANCE_NOVO") == 0) {
                    char usuario[50];
                    float valor;

                    int itens_lidos = sscanf(linha, "%*s %49s %f", usuario, &valor);

                    if (itens_lidos == 2) {
                        printf("\rServidor: %.2f para %s!\n", valor, usuario);
                    }

                } else if (strcmp(tipo_msg_temp, "LANCE_REJEITADO") == 0) {
                    printf("\rServidor: Seu lance deve ser um valor maior que o valor atual!\n");
                } else if (strcmp(tipo_msg_temp, "LANCE_FORMATO_FAIL") == 0) {
                    printf("\rServidor: Comando errado, de lances com LANCE <valor>.\n");
                } else if (strcmp(tipo_msg_temp, "LANCE_TEMPO_ESTENDIDO") == 0) {
                    printf("\rServidor: O tempo do leilao foi estendido em mais 10 segundos!\n");
                } else if (strcmp(tipo_msg_temp, "LANCE_DOLE_UMA") == 0) {
                    printf("\rServidor: DOLE UMA!\n");
                } else if (strcmp(tipo_msg_temp, "LANCE_DOLE_DUAS") == 0) {
                    printf("\rServidor: DOLE DUAS!\n");
                } else if (strcmp(tipo_msg_temp, "LANCE_DOLE_TRES") == 0) {
                    printf("\rServidor: DOLE TRES!\n");
                } else if (strcmp(tipo_msg_temp, "LEILAO_SEM_VENCEDOR") == 0) {
                    printf("\rServidor: Nao houve lances validos! Leilao sem vencedor.\n");
                } else if (strcmp(tipo_msg_temp, "LEILAO_COM_VENCEDOR") == 0) {
                    char usuario[50];
                    float valor;

                    int itens_lidos = sscanf(linha, "%*s %49s %f", usuario, &valor);

                    if (itens_lidos == 2) {
                        printf("\r====================================\n");
                        printf("\r        LEILÃO ENCERRADO!         \n");
                        printf("\r------------------------------------\n");
                        printf("\r  Vencedor: %s\n", usuario);
                        printf("\r  Lance Final: R$ %.2f\n", valor);
                        printf("\r====================================\n");
                        printf("\rParabéns ao vencedor!\n");
                    }
                } 
                else {
                    printf("\rServidor: Comando nao reconhecido!\n");
                }

                linha = strtok(NULL, "\n");
            }
        }

        // 2. Checa se há atividade no teclado (usuário digitou algo)
        if (FD_ISSET(stdin_fd, &read_fds))
        {
            if (fgets(buffer, TAM_BUFFER, stdin) != NULL)
            {
                send(socket_fd, buffer, strlen(buffer), 0);
            }
        }
    }

    close(socket_fd);
}
