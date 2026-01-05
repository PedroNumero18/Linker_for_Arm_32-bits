SRC_DIR=src
BIN_DIR=bin
INCLUDE_DIR=include

CC=clang
CFLAGS=-Wall -Werror -Wextra -I$(INCLUDE_DIR) 

TARGET=elfReader linker
TEST=test_elfReader.py
DISTRIBUTION=Rendus_PROG5_2025-2026_groupe4.tar.gz

SRC=$(wildcard $(SRC_DIR)/*.c)
OBJS=$(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRC))

.PHONY: all clean dist distclean test release debug

all: 
	@echo "Tapez 'make debug' pour pouvoir utiliser gdb ou valgrind, 'make release' ou 'make test' pour lancer le jeu de test"

release: $(TARGET)

debug: CFLAGS+=-g $(TARGET)

test: release
	./$(TEST)

dist: distclean
	tar czvf $(DISTRIBUTION) *

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

elfReader.o: $(SRC_DIR)/elfReader.c $(INCLUDE_DIR)/utils.h $(INCLUDE_DIR)/elf.h
linker.o: $(SRC_DIR)/elfReader.c $(INCLUDE_DIR)/utils.h $(INCLUDE_DIR)/elf.h

distclean:
	rm -rf $(BIN_DIR) $(TARGET) $(DISTRIBUTION) test_elfReader_results.log

clean: 
	rm -rf $(TARGET) test_elfReader_results.log
