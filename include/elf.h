#ifndef __ELF_HEADER__
#define __ELF_HEADER__

#include <stdint.h>
#include <stdio.h> 

extern char* filename;   // Nom de fichier défini dans le main

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


#define ELFMAG0 0x7f
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
#define EM_ARM 40


//sh_type possibles
#define SHT_NULL          0                
#define SHT_PROGBITS          1                
#define SHT_SYMTAB          2               
#define SHT_STRTAB          3                
#define SHT_RELA          4               
#define SHT_HASH          5               
#define SHT_DYNAMIC          6              
#define SHT_NOTE          7               
#define SHT_NOBITS          8                
#define SHT_REL                  9                
#define SHT_SHLIB          10              
#define SHT_DYNSYM          11               
#define SHT_LOPROC 0x70000000
#define SHT_HIPROC 0x7fffffff
#define SHT_LOUSER 0x80000000
#define SHT_HIUSER 0xffffffff
#define SHT_ARM_ATTRIBUTES 0x70000003U 	

//sh_flags 

#define SHF_WRITE             (1 << 0)        /* Writable */
#define SHF_ALLOC             (1 << 1)        /* Occupies memory during execution */
#define SHF_EXECINSTR             (1 << 2)        /* Executable */
#define SHF_MERGE             (1 << 4)        /* Might be merged */
#define SHF_STRINGS             (1 << 5)        /* Contains nul-terminated strings */
#define SHF_INFO_LINK             (1 << 6)        /* `sh_info' contains SHT index */
#define SHF_LINK_ORDER             (1 << 7)        /* Preserve order after combining */
#define SHF_OS_NONCONFORMING (1 << 8)        /* Non-standard OS specific handling required */
#define SHF_GROUP             (1 << 9)        /* Section is member of a group.  */
#define SHF_TLS                     (1 << 10)        /* Section hold thread-local data.  */
#define SHF_MASKOS             0x0ff00000        /* OS-specific.  */
#define SHF_MASKPROC             0xf0000000        /* Processor-specific */
#define SHF_ORDERED             (1 << 30)        /* Special ordering requirement(Solaris).  */
#define SHF_EXCLUDE             (1 << 31)        /* Section is excluded unless
                                           referenced or allocated (Solaris).*/

#define STN_UNDEF 0 /*index des symboles non définis et de la premiere entréé*/

/*Quelques constantes pour mieux gérer les infos*/
#define ELF32_ST_BIND(i) ((i)>>4)
#define ELF32_ST_TYPE(i) ((i)&0xf)
#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xf))



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


typedef struct{
	Elf32_Shdr h_section;
	uint8_t* contenu;
} elf32_sections ; 

/*Strucutre des symboles*/
typedef struct {
	Elf32_Word st_name;
	Elf32_Addr st_value;
	Elf32_Word st_size;
	unsigned char st_info;
	unsigned char st_other;
	Elf32_Half st_shndx;
} Elf32_Sym;


typedef struct { 
    Elf32_Ehdr header ;
    elf32_sections* sections;
	char*       section_str_table;	
	Elf32_Sym* table_symbole; /* ajout de la table des symboles*/
}elf32_t ;



/*
pour l'instant on s'en fout on vera quand on sera sur les sections
typedef struct {
    Elf32_Ehdr  header;
    Elf32_Shdr* sections;
    char*       section_str_table;
} ELF;
*/

//Q1
void lire_header(FILE* file, elf32_t* elf);

void affichage_entete(Elf32_Ehdr* header);



//Q2
void lire_sections(FILE* file,elf32_t* elf);

void afficher_sections(const elf32_t* elf);

//Q4
void lire_symbole(FILE* file, elf32_t* elf);

#endif
