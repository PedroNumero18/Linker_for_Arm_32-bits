#include <stdlib.h> 
#include <stdio.h>

#include "elf.h"
#include "utils.h"


/* Lecture du header */ 
void lire_header(FILE* file, elf32_t* elf){
    /*Semantique à écrire*/
    if (!file || !elf) error("Erreur fichier ou d'initialisation d'elf\n");
    //lecture ei indent + verif
    fseek(file,0,SEEK_SET);
    fread(&(elf->header).e_ident,sizeof((elf->header).e_ident),1,file);

    if((elf->header.e_ident[EI_MAG0] != ELFMAG0)
        || (elf->header.e_ident[EI_MAG1] != ELFMAG1) 
        || (elf->header.e_ident[EI_MAG2] != ELFMAG2) 
        || (elf->header.e_ident[EI_MAG3] != ELFMAG3)
    ){error("Erreur fichier, ce n'est pas un fichier ELF \n");}
    
    if (!((elf->header).e_ident[EI_CLASS] == ELFCLASS32)) error("Erreur fichier, ce n'est pas une class 32\n");

    if((elf->header).e_ident[EI_DATA] <= 0 || (elf->header).e_ident[EI_DATA] > 2 ) error("Erreur e_ident[EI_DATA]\n");

    get_16B(&elf->header.e_type, file);
    get_16B(&elf->header.e_machine, file);
    get_32B(&elf->header.e_version, file);
    get_32B(&elf->header.e_entry, file);
    get_32B(&elf->header.e_phoff, file);
    get_32B(&elf->header.e_shoff, file);
    get_32B(&elf->header.e_flags, file);
    get_16B(&elf->header.e_ehsize, file);
    get_16B(&elf->header.e_phentsize, file);
    get_16B(&elf->header.e_phnum, file);
    get_16B(&elf->header.e_shentsize, file);
    get_16B(&elf->header.e_shnum, file);
    get_16B(&elf->header.e_shstrndx, file);


}

void affichage_flags(uint32_t flags) {
    if (flags == 0) {
        return;
    }
    
    printf(", ");
    
    // Version de l'EABI
    uint32_t eabi_version = (flags & 0xFF000000) >> 24;
    if (eabi_version >= 1 && eabi_version <= 5) {
        printf("Version%d EABI", eabi_version);
    }
    
    // Flags spécifiques
    int first_flag = (eabi_version == 0);
    
    if (flags & 0x200) {  // EF_ARM_SOFT_FLOAT
        if (!first_flag) printf(", ");
        printf("soft-float ABI");
        first_flag = 0;
    }
    
    if (flags & 0x400) {  // EF_ARM_VFP_FLOAT
        if (!first_flag) printf(", ");
        printf("hard-float ABI");
        first_flag = 0;
    }
}

void affichage_entete(Elf32_Ehdr* header){
    //Faudra changer pour que le texte corresponde mais pour l'instant j'ai ça
    if (!header) error("Erreur d'initialisation d'header\n");

    printf("ELF Header:\n");
    
    /* Magic */
    printf("  Magic:   ");
    for (int i = 0; i < EI_NIDENT; i++) printf("%02x ", header->e_ident[i]);
    printf("\n");

    /* Classe */
    if (header->e_ident[EI_CLASS] == ELFCLASS32) printf("  Class:                             ELF32\n");

    /* Endianness */
    printf("  Data:                              ");
    if (header->e_ident[EI_DATA] == ELFDATA2MSB) printf("2's complement, big endian\n");
    else                                         printf("2's complement, little endian\n");

    /* Version ELF */
    printf("  Version:                           %u (current)\n",header->e_ident[EI_VERSION]);

    /* OS / ABI */
    switch (header->e_ident[EI_OSABI]) {
        case EM_NONE:   printf("  OS/ABI:                            UNIX - System V\n"); break;
        case EM_ARM:    printf("  OS/ABI:                            ARM EABI\n"); break;
        default:        printf("  OS/ABI:                            Autre\n"); break;
    }

    /* ABI version */
    printf("  ABI Version:                       %d\n", header->e_ident[EI_ABIVERSION]);

    printf("  Type:                              ");
    switch (header->e_type) {
        case ET_NONE: printf("NONE (Unknown)\n"); break;
        case ET_REL:  printf("REL (Relocatable file)\n"); break;
        case ET_EXEC: printf("EXEC (Executable file)\n"); break;
        case ET_DYN:  printf("DYN (Shared object file)\n"); break;
        default:      printf("<unknown: %d>\n", header->e_type); break;
    }

    /* Machine */
    printf("  Machine:                           ");
    if (header->e_machine == EM_ARM) printf("ARM\n");
    else printf("<unknown: %d>\n", header->e_machine);

    /*Version*/
    printf("  Version:                           0x%x\n", header->e_version);
    printf("  Entry point address:               0x%x\n", header->e_entry);
    printf("  Start of program headers:          %u (bytes into file)\n", header->e_phoff);
    printf("  Start of section headers:          %u (bytes into file)\n", header->e_shoff);
    printf("  Flags:                             0x%x", header->e_flags);
    if(header->e_machine == EM_ARM) affichage_flags(header->e_flags);
    printf("\n");
    printf("  Size of this header:               %u (bytes)\n", header->e_ehsize);
    printf("  Size of program headers:           %u (bytes)\n", header->e_phentsize);
    printf("  Number of program headers:         %u\n", header->e_phnum);
    printf("  Size of section headers:           %u (bytes)\n", header->e_shentsize);
    printf("  Number of section headers:         %u\n", header->e_shnum);
    printf("  Section header string table index: %u\n", header->e_shstrndx);
}


