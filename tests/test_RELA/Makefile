# Makefile for ARM assembly examples

# Assembler and linker
AS = arm-none-eabi-as
LD = arm-none-eabi-ld

# Source files and executables
TARGETS = somme multiplication find_max factoriel ascii_uppercase filtre_even empty
SRC_FILES = $(wildcard *.s)

all: $(TARGETS)

$(TARGETS):
	@for src in $(SRC_FILES); do \
	    exe=$$(basename $$src .s); \
		$(AS) -EB -o $$exe.o $$src; \
	    $(LD) -EB -o $$exe $$exe.o; \
	done

clean:
	rm -f *.o $(TARGETS) 