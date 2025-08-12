// main_cliente.c
#include "headers/client.h"

int main()
{
    // Cuidar o ip para conexao, esta como localhost por default, mas se o servidor ficar em outro computador, deve-se alterar aqui para o ip correto, para saber o ip no linux basta digitar ifconfig
    int sock = conectar_ao_servidor("127.0.0.1", 8080);
    loop_cliente(sock);
    return 0;
}
