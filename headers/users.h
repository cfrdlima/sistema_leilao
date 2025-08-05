#ifndef USERS_H
#define USERS_H

#define MAX_USUARIOS 10

typedef struct
{
    char nome[50];
    char senha[50];
} Usuario;

void inicializar_usuarios();
int autenticar_usuario(const char *nome, const char *senha);

#endif
