#include "../headers/users.h"
#include <string.h>
#include <stdio.h>

static Usuario usuarios[MAX_USUARIOS];
static UsuarioLogado logados[MAX_USUARIOS];
static int total_usuarios = 0;

void inicializar_usuarios()
{
    strcpy(usuarios[0].nome, "joao");
    strcpy(usuarios[0].senha, "123");

    strcpy(usuarios[1].nome, "maria");
    strcpy(usuarios[1].senha, "abc");

    total_usuarios = 2;

    for (int i = 0; i < MAX_USUARIOS; i++)
    {
        logados[i].ativo = 0;
        logados[i].socket_fd = -1;
    }
}

int autenticar_usuario(const char *nome, const char *senha)
{
    for (int i = 0; i < total_usuarios; i++)
    {
        if (strcmp(usuarios[i].nome, nome) == 0 &&
            strcmp(usuarios[i].senha, senha) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int registrar_login(const char *nome, int socket_fd)
{
    for (int i = 0; i < MAX_USUARIOS; i++)
    {
        if (!logados[i].ativo)
        {
            strcpy(logados[i].nome, nome);
            logados[i].socket_fd = socket_fd;
            logados[i].ativo = 1;
            return 1;
        }
    }
    return 0; // sem espaço
}

void registrar_logout(int socket_fd)
{
    for (int i = 0; i < MAX_USUARIOS; i++)
    {
        if (logados[i].ativo && logados[i].socket_fd == socket_fd)
        {
            logados[i].ativo = 0;
            logados[i].socket_fd = -1;
            logados[i].nome[0] = '\0';
            return;
        }
    }
}

const char *usuario_por_socket(int socket_fd)
{
    for (int i = 0; i < MAX_USUARIOS; i++)
    {
        if (logados[i].ativo && logados[i].socket_fd == socket_fd)
        {
            return logados[i].nome;
        }
    }
    return NULL;
}

int esta_logado(int socket_fd)
{
    return usuario_por_socket(socket_fd) != NULL;
}
