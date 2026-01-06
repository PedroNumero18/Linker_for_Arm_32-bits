# Outils de compilation
CC      = arm-none-eabi-gcc
CXX     = arm-none-eabi-g++
AS      = arm-none-eabi-as
RUSTC   = rustc

HAS_RUSTC := $(shell command -v $(RUSTC) >/dev/null 2>&1 && echo yes)
HAS_ARMGCC:= $(shell (command -v $(CC) >/dev/null 2>&1 && echo yes) && (command -v $(CXX) >/dev/null 2>&1 && echo yes))

CFLAGS   = -D__TEST1__ -nostdlib -static
CXXFLAGS = -D__TEST2__ -nostdlib -static
RUST_TARGET = armebv7r-none-eabi

ENDIANFLAGS = -mbig-endian
CFLAGS   += $(ENDIANFLAGS)
CXXFLAGS += $(ENDIANFLAGS)
ASFLAGS  = -EB


BINS =
ifneq ($(HAS_ARMGCC),)
BINS += Test1.elf Test2.elf
endif
ifneq ($(HAS_RUSTC),)
BINS += Test4.elf
endif

all: $(BINS)

.PHONY: all clean

Test1.elf: test1.c test3.o
	$(CC) $(CFLAGS) -o $@ $^

Test2.elf: test2.cpp test3.o
	$(CXX) $(CXXFLAGS) -o $@ $^

test3.o: test3.s
	$(AS) $(ASFLAGS) -o $@ $<

Test4.elf: test4.rs test3.o
	$(RUSTC) --target $(RUST_TARGET) $(RUSTFLAGS_NO_STD) -o $@ $<

clean:
	rm -f $(BINS) *.o *.out *.elf


