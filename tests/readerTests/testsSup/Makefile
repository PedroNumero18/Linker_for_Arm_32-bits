# Outils de compilation
CC      = arm-none-eabi-gcc
CXX     = arm-none-eabi-g++
AS      = arm-none-eabi-as

HAS_ARMGCC:= $(shell (command -v $(CC) >/dev/null 2>&1 && echo yes) && (command -v $(CXX) >/dev/null 2>&1 && echo yes))

CFLAGS   = -D__TEST1__ -nostdlib -static
CXXFLAGS = -D__TEST2__ -nostdlib -static

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

clean:
	rm -f $(BINS) *.o *.out *.elf


