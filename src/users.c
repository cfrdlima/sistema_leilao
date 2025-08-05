#include "../headers/users.h"
#include <string.h>

static Usuario usuarios[MAX_USUARIOS];
static int total_usuarios = 0;

void inicializar_usuarios()
{
    // Cadastra usuários fixos (poderia vir de arquivo depois)
    strcpy(usuarios[0].nome, "joao");
    strcpy(usuarios[0].senha, "123");

    strcpy(usuarios[1].nome, "maria");
    strcpy(usuarios[1].senha, "abc");

    total_usuarios = 2;
}

int autenticar_usuario(const char *nome, const char *senha)
{
    for (int i = 0; i < total_usuarios; i++)
    {
        if (strcmp(usuarios[i].nome, nome) == 0 &&
            strcmp(usuarios[i].senha, senha) == 0)
        {
            return 1; // Sucesso
        }
    }
    return 0; // Falha
}
