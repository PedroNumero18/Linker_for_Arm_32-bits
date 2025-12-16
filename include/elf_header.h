#ifndef __ELF_HEADER__
#define __ELF_HEADER__

#include <stdint.h>
#include <stdio.h> 

//Constantes ELF
#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3
#define EI_CLASS       4
#define EI_DATA        5
#define EI_VERSION     6
#define EI_OSABI       7
#define EI_ABIVERSION  8
#define EI_NIDENT  16


#define ELFMAG0 '0x7f'
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'


#define ELFCLASS32 1
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2


//types
#define ET_NONE 0
#define ET_REL  1
#define ET_EXEC 2
#define ET_DYN  3
#define ET_CORE 4

//machines
#define EM_NONE 0
#define EM_M386 3
#define EM_ARM 40
#define EM_X86_64 62
#define EM_AARCH64 183



typedef uint32_t	Elf32_Addr;
typedef uint16_t	Elf32_Half;
typedef uint32_t	Elf32_Off;
typedef uint32_t	Elf32_Word;

typedef struct {
	unsigned char e_ident[EI_NIDENT];	/* File identification. */
	Elf32_Half	e_type;		/* File type. */
	Elf32_Half	e_machine;	/* Machine architecture. */
	Elf32_Word	e_version;	/* ELF format version. */
	Elf32_Addr	e_entry;	/* Entry point. */
	Elf32_Off	e_phoff;	/* Program header file offset. */
	Elf32_Off	e_shoff;	/* Section header file offset. */
	Elf32_Word	e_flags;	/* Architecture-specific flags. */
	Elf32_Half	e_ehsize;	/* Size of ELF header in bytes. */
	Elf32_Half	e_phentsize;	/* Size of program header entry. */
	Elf32_Half	e_phnum;	/* Number of program header entries. */
	Elf32_Half	e_shentsize;	/* Size of section header entry. */
	Elf32_Half	e_shnum;	/* Number of section header entries. */
	Elf32_Half	e_shstrndx;	/* Section name strings section. */
} Elf32_Ehdr;

typedef struct {
	Elf32_Word	sh_name;	/* Section name (index into the
					   section header string table). */
	Elf32_Word	sh_type;	/* Section type. */
	Elf32_Word	sh_flags;	/* Section flags. */
	Elf32_Addr	sh_addr;	/* Address in memory image. */
	Elf32_Off	sh_offset;	/* Offset in file. */
	Elf32_Word	sh_size;	/* Size in bytes. */
	Elf32_Word	sh_link;	/* Index of a related section. */
	Elf32_Word	sh_info;	/* Depends on section type. */
	Elf32_Word	sh_addralign;	/* Alignment in bytes. */
	Elf32_Word	sh_entsize;	/* Size of each entry in section. */
} Elf32_Shdr;

typedef struct { 
    Elf32_Ehdr header ;
    Elf32_Shdr section; 
} ELF ;
/*
pour l'instant on s'en fout on vera quand on sera sur les sections
typedef struct {
    Elf32_Ehdr  header;
    Elf32_Shdr* sections;
    char*       section_str_table;
} ELF;
*/

void lire_header(FILE* file, ELF* elf);

void affichage_entete(Elf32_Ehdr* header);

#endif