#ifndef USERS_H
#define USERS_H

#define MAX_USUARIOS 10

typedef struct
{
    char nome[50];
    char senha[50];
} Usuario;

typedef struct
{
    char nome[50];
    int socket_fd;
    int ativo;
} UsuarioLogado;

void inicializar_usuarios();
int autenticar_usuario(const char *nome, const char *senha);

// Login/logout
int registrar_login(const char *nome, int socket_fd);
void registrar_logout(int socket_fd);

// Consulta
const char *usuario_por_socket(int socket_fd);
int esta_logado(int socket_fd);

#endif
