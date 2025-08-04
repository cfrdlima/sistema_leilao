CC = gcc
CFLAGS = -Iheaders -Wall
SRC = src/server.c main.c
BIN = server

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)

clean:
	rm -f $(BIN)
