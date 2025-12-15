CC=clang
RELEASE=-O2
CFLAGS=-Wall -Werror -Wextra -Iinclude

SRC_DIR=src/
BIN_DIR=bin/

TARGET=linker 
SRC=$(SRC_DIR)$(wildcard *.c)
OBJS=$(BIN_DIR)$(wildcard *.o)

.PHONE 

All: $(TARGET)

$(TARGET):
