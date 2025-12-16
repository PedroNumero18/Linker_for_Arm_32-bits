# Outils de compilation
CC      = arm-none-eabi-gcc
CXX     = arm-none-eabi-g++
AS      = arm-none-eabi-as
RUSTC   = rustc

# Détection de rustc (vide si absent)
HAS_RUSTC := $(shell command -v $(RUSTC) >/dev/null 2>&1 && echo yes)
HAS_ARMGCC:= $(shell (command -v $(CC) >/dev/null 2>&1 && echo yes) && (command -v $(CXX) >/dev/null 2>&1 && echo yes))

# Drapeaux
CFLAGS   = -D__TEST1__ -nostdlib -static
CXXFLAGS = -D__TEST2__ -nostdlib -static

ENDIANFLAGS = -mbig-endian

CFLAGS   += $(ENDIANFLAGS)
CXXFLAGS += $(ENDIANFLAGS)

# Pour l’assembleur GNU as : -EB (big endian)
ASFLAGS  = -EB


# Cible Rust (à adapter à ton CPU)
RUST_TARGET = thumbv7m-none-eabi

# Liste des binaires à produire
BINS = Test1 Test2
ifneq ($(HAS_RUSTC),)
BINS += test4
endif

all: $(BINS)

Test1: test1.c test3.o
	$(CC) $(CFLAGS) -o $@ $^

Test2: test2.cpp test3.o
	$(CXX) $(CXXFLAGS) -o $@ $^

test3.o: test3.s
	$(AS) $(ASFLAGS) -o $@ $<

# Ne sera appelée que si rustc est détecté (car test4 n'est ajouté à BINS que dans ce cas)
test4: test4.rs
	$(RUSTC) --target $(RUST_TARGET) $(RUSTFLAGS_NO_STD) -o $@ $<

clean:
	rm -f Test1 Test2 test4 test3.o

.PHONY: all clean
