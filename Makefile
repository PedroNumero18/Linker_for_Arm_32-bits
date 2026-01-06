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
OBJS_COMMON=$(filter-out $(BIN_DIR)/elfReader.o $(BIN_DIR)/linker.o, $(OBJS))

.PHONY: all clean dist distclean test release debug

all: 
	@echo "Tapez 'make debug' pour pouvoir utiliser gdb ou valgrind, 'make release' ou 'make test' pour lancer le jeu de test"

release: $(TARGET)

debug: CFLAGS+=-g 
debug: $(TARGET)

test: release
	./$(TEST)

dist: distclean
	tar czvf $(DISTRIBUTION) *

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

elfReader: $(BIN_DIR)/elfReader.o $(OBJS_COMMON)
	$(CC) $(CFLAGS) -o $@ $^

linker: $(BIN_DIR)/linker.o $(OBJS_COMMON)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

distclean:
	rm -rf $(BIN_DIR) $(TARGET) $(DISTRIBUTION) test_elfReader_results.log

clean: 
	rm -rf $(TARGET) test_elfReader_results.log
