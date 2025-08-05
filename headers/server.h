// headers/server.h
// server.h - Cabeçalho do servidor de leilão
#ifndef SERVER_H
#define SERVER_H

// Constantes
#define PORTA 8080
#define MAX_CLIENTES 10
#define TAM_BUFFER 1024

// Funções exportadas
void iniciar_servidor();
void tratar_conexoes(int servidor_fd);
void lidar_com_mensagem(int cliente_fd, char *mensagem);

#endif
