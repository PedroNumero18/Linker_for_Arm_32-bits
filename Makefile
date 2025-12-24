SRC_DIR=src
BIN_DIR=bin
INCLUDE_DIR=include

CC=clang
CFLAGS=-Wall -Werror -Wextra -I$(INCLUDE_DIR) -g 

TARGET=elfReader 
DISTRIBUTION=Rendus_PROG5_2025-2026_groupe4.tar.gz

SRC=$(wildcard $(SRC_DIR)/*.c)
OBJS=$(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRC))

.PHONY: All clean dist

All: $(TARGET)

dist: clean
	tar czvf $(DISTRIBUTION) *

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

elfReader.o: $(SRC_DIR)/elfReader.c $(INCLUDE_DIR)/utils.h $(INCLUDE_DIR)/elf.h

clean: 
	rm -rf $(BIN_DIR) $(TARGET) $(DISTRIBUTION)
