#include "../headers/users.h"
#include <string.h>
#include <stdio.h>

static Usuario usuarios[MAX_USUARIOS];
static UsuarioLogado logados[MAX_USUARIOS];
static int total_usuarios = 0;

/**
 * Inicializa o vetor de usu rios com os usu rios "joao" e "maria".
 *
 * Os usu rios s o adicionados em um vetor est tico, com o nome e a
 * senha de cada um. O n mero de usu rios inicializados   2.
 *
 * Al m disso, inicializa o vetor de usu rios logados, setando todos
 * como inativos e sem socket associado.
 */
void inicializar_usuarios()
{
    strcpy(usuarios[0].nome, "yuri");
    strcpy(usuarios[0].senha, "123");

    strcpy(usuarios[1].nome, "kalani");
    strcpy(usuarios[1].senha, "456");

    strcpy(usuarios[2].nome, "claudinei");
    strcpy(usuarios[2].senha, "789");

    total_usuarios = 3;

    for (int i = 0; i < MAX_USUARIOS; i++)
    {
        logados[i].ativo = 0;
        logados[i].socket_fd = -1;
    }
}

/**
 * Autentica um usu rio com base no nome e senha.
 *
 * Essa fun o verifica se o nome e a senha recebidos como par metro
 * correspondem a algum usu rio cadastrado no sistema.
 *
 * @param nome Nome do usu rio
 * @param senha Senha do usu rio
 * @return 1 se o usu rio for autenticado com sucesso, 0 caso contr rio
 */
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

/**
 * Registra um login de um usu rio no sistema.
 *
 * Essa fun o recebe o nome do usu rio e o descritor de arquivo do socket
 * de comunica o com o cliente e registra o login no sistema. A fun o
 * retorna 1 se o usu rio for registrado com sucesso e 0 caso contr rio
 * (se j  houver o n mero m ximo de usu rios logados).
 *
 * @param nome Nome do usu rio
 * @param socket_fd Descritor de arquivo do socket de comunica o com o cliente
 * @return 1 se o usu rio for registrado com sucesso, 0 caso contr rio
 */
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

/**
 * Registra um logout de um usu rio no sistema.
 *
 * Essa fun o recebe o descritor de arquivo do socket de comunica o com o cliente
 * e registra o logout no sistema. A fun o procura o usu rio logado
 * correspondente ao socket e marca-o como inativo, cancelando
 * o login.
 *
 * @param socket_fd Descritor de arquivo do socket de comunica o com o cliente
 */
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

/**
 * Retorna o nome do usu rio logado que est  associado ao socket especificado.
 *
 * A fun o procura o usu rio logado correspondente ao socket e retorna o seu nome.
 * Se n o houver nenhum usu rio logado com o socket especificado, a fun o
 * retorna NULL.
 *
 * @param socket_fd Descritor de arquivo do socket de comunica o com o cliente
 * @return Nome do usu rio logado associado ao socket, ou NULL se n o houver
 */
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

/**
 * Verifica se o usu rio com o socket especificado est  logado no sistema.
 *
 * A fun o verifica se o usu rio logado correspondente ao socket especificado
 * existe e est  ativo. Se o usu rio existir e estiver ativo, a fun o retorna 1.
 * Caso contr rio, a fun o retorna 0.
 *
 * @param socket_fd Descritor de arquivo do socket de comunica o com o cliente
 * @return 1 se o usu rio estiver logado, 0 caso contr rio
 */
int esta_logado(int socket_fd)
{
    return usuario_por_socket(socket_fd) != NULL;
}
