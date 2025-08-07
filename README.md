# Sistema de Leilão em C (Cliente-Servidor TCP)

Este projeto implementa um sistema de leilão online usando sockets TCP na linguagem C. Clientes podem se autenticar, entrar em uma sala de leilão, fazer lances e visualizar os resultados. O leilão é gerenciado por um servidor central que permite múltiplos clientes simultâneos.

---

## 🧱 Estrutura do Projeto

```
sistema_leilao/
├── headers/            # Arquivos .h (interface dos módulos)
│   ├── server.h
│   ├── auction.h
│   ├── users.h
│   └── ...
├── src/                # Implementação dos módulos
│   ├── server.c
│   ├── auction.c
│   ├── users.c
│   └── ...
├── main_server.c       # Inicialização do servidor
├── main_client.c       # Inicialização do cliente
├── makefile            # Compilação automática
└── README.md           # Instruções
```

---

## ⚙️ Compilação

No diretório raiz do projeto:

```bash
make
```

Isso gerará dois executáveis:

- `./server` → servidor de leilão
- `./client` → cliente participante

CASO FOR EXECUTAR O SERVIDOR EM OUTRO COMPUTADOR, ATENTE-SE AO IP PARA CONECTAR. IP ESTA DEFINIDO COMO LOCALHOST POIS OS TESTES FORAM EXECUTADOS NA MESMA MAQUINA.

---

## 🚀 Execução

### 1. Iniciar o servidor

```bash
./server
```

O servidor escutará na porta 8080 e aceitará múltiplos clientes.

### 2. Iniciar um cliente (em outro terminal ou máquina)

```bash
./client
```

Você pode abrir quantos clientes quiser. Cada um representará um usuário distinto no leilão.

---

## 💬 Comandos disponíveis (cliente → servidor)

| Comando                   | Descrição                            |
| ------------------------- | ------------------------------------ |
| `LOGIN <usuario> <senha>` | Faz login no sistema                 |
| `INFO`                    | Mostra o nome do usuário autenticado |
| `LOGOUT`                  | Encerra a sessão atual               |
| `ENTRAR_LEILAO`           | Entra no leilão                      |
| `LANCE <valor>`           | Dá um lance (se estiver no leilão)   |
| `PING`                    | Testa a conexão com o servidor       |

---

## 📡 Respostas do servidor

| Resposta                                | Significado                      |
| --------------------------------------- | -------------------------------- |
| `LOGIN_OK` / `LOGIN_FAIL`               | Sucesso ou falha no login        |
| `LEILAO_INICIO <item> <minimo> <tempo>` | Início do leilão                 |
| `NOVO_LANCE <usuario> <valor>`          | Alguém deu um novo lance         |
| `LEILAO_FIM <vencedor> <valor>`         | Resultado do leilão              |
| `LANCE_REJEITADO`                       | Lance abaixo do valor atual      |
| `Você já está logado.`                  | Proteção contra múltiplos logins |
| `Você não está logado.`                 | Comando feito sem login          |
| `PONG`                                  | Resposta ao `PING`               |

---

## 📋 Exemplo de uso (cliente)

```bash
LOGIN joao 123
INFO
ENTRAR_LEILAO
LANCE 1500
LOGOUT
```

---

## 👤 Usuários de teste

Você pode autenticar com:

- `joao 123`
- `maria abc`
- `ana 456`

---

## 🔁 Rodadas de leilão

- Um leilão inicia automaticamente quando 2 clientes entram com `ENTRAR_LEILAO`
- Após 30 segundos, o servidor encerra a rodada e envia `LEILAO_FIM`
- Uma nova rodada pode começar com novos participantes

---

## 🛠 Requisitos técnicos

- Sistema operacional: Linux
- Compilador: `gcc`
- Bibliotecas usadas: apenas padrão (`stdio.h`, `string.h`, `sys/socket.h`, `unistd.h`, etc.)

---

## 📦 Limpeza

```bash
make clean
```

Remove os binários gerados.

---

## 👨‍💻 Autor

Desenvolvido como trabalho final da disciplina de Redes de Computadores — 2025/1.
Alunos: Claudinei de Lima, Kalani Sosa, Yuri Nunes.
