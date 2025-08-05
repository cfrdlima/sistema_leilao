# Compilador e flags
CC = gcc
CFLAGS = -Wall -Iheaders

# Fontes
SRC_DIR = src
HEADERS_DIR = headers

SERVER_SRC = main_server.c src/server.c src/users.c
CLIENT_SRC = main_client.c $(SRC_DIR)/client.c

# Binários
SERVER_BIN = server
CLIENT_BIN = cliente

# Alvo padrão
all: $(SERVER_BIN) $(CLIENT_BIN)

# Compilar servidor
$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER_BIN) $(SERVER_SRC)

# Compilar cliente
$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT_BIN) $(CLIENT_SRC)

# Limpar binários
clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)
