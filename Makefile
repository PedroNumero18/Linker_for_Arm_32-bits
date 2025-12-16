CC=clang
RELEASE=-O2
DEBUG=-g
CFLAGS=-Wall -Werror -Wextra -Iinclude

SRC_DIR=src/
BIN_DIR=bin/

TARGET=linker 
SRC=$(wildcard $(SRC_DIR)*.c)
OBJS=$(patsubst $(SRC_DIR)%.c, $(BIN_DIR)%.o, $(SRC))

.PHONY: All clean 

All: 
	$(shell echo "use make debug or release")

release: $(TARGET)

debug: $(TARGET)

$(BIN_DIR):
	mkdir -p bin

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

$(BIN_DIR)%.o: $(SRC_DIR)%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm -rf $(BIN_DIR) $(TARGET)