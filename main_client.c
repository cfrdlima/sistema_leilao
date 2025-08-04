// main_cliente.c
#include "headers/client.h"

int main()
{
    int sock = conectar_ao_servidor("127.0.0.1", 8080);
    loop_cliente(sock);
    return 0;
}
