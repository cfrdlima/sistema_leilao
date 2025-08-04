# 🛠️ Sistema de Leilão — Cliente/Servidor em C

Este projeto implementa a base de um **sistema de leilão** usando **Sockets TCP em C**, com arquitetura **cliente-servidor**.

---

## 📚 Descrição

A comunicação entre cliente e servidor é feita via socket TCP. Por enquanto, o sistema responde ao comando `PING`, retornando `PONG`, para validar a conexão entre cliente e servidor.

Em breve, o sistema será expandido para incluir:

- Autenticação
- Controle de lances
- Leilões com tempo
- Broadcast de mensagens

---

## 🗂️ Estrutura de diretórios

```
.
├── headers/
│   ├── client.h
│   └── server.h
├── src/
│   ├── client.c
│   └── server.c
├── main_client.c
├── main_server.c
├── makefile
└── README.md
```

---

## ⚙️ Compilação

### Compilar tudo (cliente e servidor):

```bash
make
```

### Compilar apenas o servidor:

```bash
make server
```

### Compilar apenas o cliente:

```bash
make cliente
```

### Limpar os binários:

```bash
make clean
```

---

## 🚀 Execução

### 1. Iniciar o servidor

Em um terminal:

```bash
./server
```

Você verá:

```
Servidor ouvindo na porta 8080...
```

### 2. Iniciar o cliente

Em outro terminal:

```bash
./cliente
```

Você verá:

```
Conectado ao servidor 127.0.0.1:8080
>
```

---

## 📡 Teste com `PING`

No terminal do cliente, digite:

```text
PING
```

E o servidor irá responder:

```text
Servidor: PONG
```

✅ Isso confirma que a comunicação TCP entre cliente e servidor está funcionando.

---

## ❌ Encerrando

- Para sair do cliente: `Ctrl + D` ou `Ctrl + C`
- Para parar o servidor: `Ctrl + C`

---

## 🧪 Próximos passos (em desenvolvimento)

- `LOGIN` / `LOGOUT`
- `JOIN_AUCTION`, `BID valor`
- Encerramento automático de leilão por tempo
- Mensagens em JSON ou protocolo customizado
- Multiplexação de clientes simultâneos

---

## 👨‍💻 Desenvolvido por

Grupo de 3 alunos – Trabalho final da disciplina **Redes de Computadores**  
Universidade Federal de Pelotas – 2025/2

---
