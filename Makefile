CC=clang
CFLAGS=-Wall  -Wextra -Iinclude -g 

SRC_DIR=src/
BIN_DIR=bin/

TARGET=linker 
DISTRIBUTION=Rendus_PROG5_2025-2026_groupe4.tar.gz

SRC=$(wildcard $(SRC_DIR)*.c)
OBJS=$(patsubst $(SRC_DIR)%.c, $(BIN_DIR)%.o, $(SRC))

.PHONY: All clean 

All: $(TARGET)

dist:
	$(shell make clean)
	$(shell tar czvf $(DISTRIBUTION) *)

$(BIN_DIR):
	mkdir -p bin

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

$(BIN_DIR)%.o: $(SRC_DIR)%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm -rf $(BIN_DIR) $(TARGET) $(DISTRIBUTION)