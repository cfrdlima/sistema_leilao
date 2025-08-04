// headers/client.h
#ifndef CLIENT_H
#define CLIENT_H

#define TAM_BUFFER 1024

int conectar_ao_servidor(const char *ip, int porta);
void loop_cliente(int socket_fd);

#endif
