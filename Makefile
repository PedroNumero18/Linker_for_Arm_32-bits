CC=clang
RELEASE=-O2
CFLAGS=-Wall -Werror -Wextra -Iinclude

SRC_DIR=src/
BIN_DIR=bin/

TARGET=linker 
SRC=$(wildcard $(SRC_DIR)*.c)
OBJS=$(patsubst $(SRC_DIR)%.c, $(BIN_DIR)%.o, $(SRC))

.PHONY: All clean 

All: $(TARGET)

$(BIN_DIR):
	mkdir bin

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

$(BIN_DIR)%.o: $(SRC_DIR)%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm -rf $(BIN_DIR) $(TARGET)