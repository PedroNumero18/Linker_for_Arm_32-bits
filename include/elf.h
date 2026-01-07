#ifndef __ELF_HEADER__
#define __ELF_HEADER__

#include <stdint.h>
#include <stdio.h> 

extern char* filename;   //Nom de fichier défini dans le main


//Constantes ELF
#define EI_MAG0        0
#define EI_MAG1        1
#define EI_MAG2        2
#define EI_MAG3        3
#define EI_CLASS       4
#define EI_DATA        5
#define EI_VERSION     6
#define EI_OSABI       7
#define EI_ABIVERSION  8
#define EI_NIDENT      16


#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'


#define ELFCLASS32  1
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
#define EM_ARM  40


//sh_type possibles
#define SHT_NULL           0
#define SHT_PROGBITS       1
#define SHT_SYMTAB         2
#define SHT_STRTAB         3
#define SHT_RELA           4
#define SHT_HASH           5
#define SHT_DYNAMIC        6
#define SHT_NOTE           7
#define SHT_NOBITS         8
#define SHT_REL            9
#define SHT_SHLIB          10
#define SHT_DYNSYM         11
#define SHT_LOPROC         0x70000000
#define SHT_HIPROC         0x7fffffff
#define SHT_LOUSER         0x80000000
#define SHT_HIUSER         0xffffffff
#define SHT_ARM_ATTRIBUTES 0x70000003U


//sh_flags 
#define SHF_WRITE              (1 << 0)        /* Writable */
#define SHF_ALLOC              (1 << 1)        /* Occupies memory during execution */
#define SHF_EXECINSTR          (1 << 2)        /* Executable */
#define SHF_MERGE              (1 << 4)        /* Might be merged */
#define SHF_STRINGS            (1 << 5)        /* Contains nul-terminated strings */
#define SHF_INFO_LINK          (1 << 6)        /* `sh_info' contains SHT index */
#define SHF_LINK_ORDER         (1 << 7)        /* Preserve order after combining */
#define SHF_OS_NONCONFORMING   (1 << 8)        /* Non-standard OS specific handling required */
#define SHF_GROUP              (1 << 9)        /* Section is member of a group.  */
#define SHF_TLS                (1 << 10)       /* Section hold thread-local data.  */
#define SHF_MASKOS             0x0ff00000      /* OS-specific.  */
#define SHF_MASKPROC           0xf0000000      /* Processor-specific */
#define SHF_ORDERED            (1 << 30)       /* Special ordering requirement(Solaris).  */
#define SHF_EXCLUDE            (1 << 31)       /* Section is excluded unless
                                                  referenced or allocated (Solaris).*/

#define STN_UNDEF 0 /*index des symboles non définis et de la premiere entréé*/

//Types st_info
#define STT_NOTYPE   0 
#define STT_OBJECT   1
#define STT_FUNC     2  
#define STT_SECTION  3 
#define STT_FILE     4  
#define STT_COMMON   5 
#define STT_TLS      6  

//Bind st_info
#define STB_LOCAL    0 
#define STB_GLOBAL   1  
#define STB_WEAK     2  


//Visibilité st_other
#define STV_DEFAULT   0
#define STV_INTERNAL  1
#define STV_HIDDEN    2
#define STV_PROTECTED 3


/* Quelques constantes pour mieux gérer les infos */
#define ELF32_ST_BIND(i) ((i)>>4)
#define ELF32_ST_TYPE(i) ((i)&0xf)
#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xf))

/*  gérer les infos lors des reimplemenntationns */
#define ELF32_R_SYM(i) ((i)>>8)
#define ELF32_R_TYPE(i) ((unsigned char)(i))
#define ELF32_R_INFO(s,t) (((s)<<8)+(unsigned char)(t))


typedef uint32_t	Elf32_Addr;
typedef uint16_t	Elf32_Half;
typedef uint32_t	Elf32_Off;
typedef uint32_t	Elf32_Word;
typedef int32_t	    Elf32_Sword;/*categorie supplementaire pour les reimplementations*/



/* Header global du fichier */
typedef struct {
	unsigned char e_ident[EI_NIDENT]; /* File identification. */
	Elf32_Half    e_type;             /* File type. */
	Elf32_Half    e_machine;          /* Machine architecture. */
	Elf32_Word    e_version;          /* ELF format version. */
	Elf32_Addr    e_entry;            /* Entry point. */
	Elf32_Off     e_phoff;            /* Program header file offset. */
	Elf32_Off     e_shoff;            /* Section header file offset. */
	Elf32_Word    e_flags;            /* Architecture-specific flags. */
	Elf32_Half    e_ehsize;           /* Size of ELF header in bytes. */
	Elf32_Half    e_phentsize;        /* Size of program header entry. */
	Elf32_Half    e_phnum;            /* Number of program header entries. */
	Elf32_Half    e_shentsize;        /* Size of section header entry. */
	Elf32_Half    e_shnum;            /* Number of section header entries. */
	Elf32_Half    e_shstrndx;         /* Section name strings section. */
} Elf32_Ehdr;


/* Header de section */
typedef struct {
	Elf32_Word  sh_name;      /* Section name (index into the
	                             section header string table). */
	Elf32_Word  sh_type;      /* Section type. */
	Elf32_Word  sh_flags;     /* Section flags. */
	Elf32_Addr  sh_addr;      /* Address in memory image. */
	Elf32_Off   sh_offset;    /* Offset in file. */
	Elf32_Word  sh_size;      /* Size in bytes. */
	Elf32_Word  sh_link;      /* Index of a related section. */
	Elf32_Word  sh_info;      /* Depends on section type. */
	Elf32_Word  sh_addralign; /* Alignment in bytes. */
	Elf32_Word  sh_entsize;   /* Size of each entry in section. */
} Elf32_Shdr;


/* Type pour regrouper un header et son contenu */
typedef struct{
	Elf32_Shdr h_section;
	uint8_t*   contenu;
} elf32_sections ; 


/* Structure des symboles */
typedef struct {
	Elf32_Word    st_name;
	Elf32_Addr    st_value;
	Elf32_Word    st_size;
	unsigned char st_info;
	unsigned char st_other;
	Elf32_Half    st_shndx;
} Elf32_Sym;

//Structure pour gerer les symboles globaux plus facilement
typedef struct {
    char* nom;           // pointeur vers fusion->strtab
    Elf32_Sym* sym1;     // NULL si pas dans elf1
    Elf32_Sym* sym2;     // NULL si pas dans elf2
    int index_fusion;   
} Symboles_globaux;


/*Structure pour les reimplementations*/
typedef struct {
 Elf32_Addr r_offset;
 Elf32_Word r_info;
} Elf32_Rel;


typedef struct {
 Elf32_Addr r_offset;
 Elf32_Word r_info;
 Elf32_Sword r_addend;
} Elf32_Rela;


/*etape6 fusion des sections*/
typedef struct {
   elf32_sections* sections;   
   int  nb_sections;
   int* section_map_elf2;
   int* section_offset_elf2;
 } elf32_fusion_sections;

/* Table de symboles fusionnée (symtab de sortie) */
 typedef struct {
    Elf32_Sym *table_symbole; 
    int        nb_sym;      
    char      *strtab;     
    int        strtab_size;
    int       *sym_map_elf1; 
    int       *sym_map_elf2;
} elf32_fusion_symboles;


/*Structure represantant le contenu du fichier ELF*/
typedef struct { 
  Elf32_Ehdr      header ;
  elf32_sections* sections;
  char*           section_str_table;	
  Elf32_Sym*      table_symbole; /* ajout de la table des symboles*/
  uint32_t        nb_symboles;
  Elf32_Rel* 	  rel_table;
  uint32_t        nb_rel;
  Elf32_Rela* 	  RELA_table;
  uint32_t        nb_RELA;
} elf32_t ;

//gestion de structure
elf32_t* elf_init();
void elf_free(elf32_t* elf);

//Header 
void lire_header(FILE* file, elf32_t* elf);
void affichage_entete(Elf32_Ehdr* header);


//Section
void lire_sections(FILE* file,elf32_t* elf);
void afficher_sections(const elf32_t* elf);

//Cotenu section
void lire_contenu_sect( FILE* f, elf32_t *elf, int index);
int get_section_ind_par_nom(const elf32_t *elf, const char *name);
const char *get_type(Elf32_Word t);
void afficher_contenu_section( elf32_t *elf, char *param);

//FUSION SECTION
elf32_fusion_sections* fusion_sections(elf32_t* elf1, elf32_t* elf2);
void free_fusion_sections(elf32_fusion_sections* fusion);

//Table des symboles
void lire_symbole(FILE* file, elf32_t* elf);
const char* get_type_string(unsigned char info);
const char* get_bind_string(unsigned char info);
const char* get_ndx_string(Elf32_Half shndx);
void afficher_symboles(elf32_t* elf);

//FUSION SYMBOLE
elf32_fusion_symboles* fusion_symboles(elf32_t* elf1, elf32_t* elf2);

//REIMPLEMENTATION
void lire_Reimple(FILE* file, elf32_t* elf);
void afficher_Reimple(elf32_t* elf);

#endif
