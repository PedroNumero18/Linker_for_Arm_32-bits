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

void affichage_entete(Elf32_Ehdr* header){
    //Faudra changer pour que le texte corresponde mais pour l'instant j'ai ça
    if (!header) error("Erreur d'initialisation d'header\n");

    printf("ELF Header :\n");
    /* Magic */
    printf("  Magique:   ");
    for (int i = 0; i < EI_NIDENT; i++) printf("%02x ", header->e_ident[i]);
    printf("\n");

    /* Classe */
    if (header->e_ident[EI_CLASS] == ELFCLASS32) printf("  Classe:                            ELF32\n");

    /* Endianness */
    if (header->e_ident[EI_DATA] == ELFDATA2MSB) printf("  Données:                           Big endian\n");
    else                                         printf("  Données:                           Little endian\n");

    /* Version ELF */
    printf("  Version:                           %u (current)\n",header->e_ident[EI_VERSION]);

    /* OS / ABI */
    switch (header->e_ident[EI_OSABI]) {
        case EM_NONE:   printf("  OS/ABI:                            UNIX - System V\n"); break;
        case EM_ARM:    printf("  OS/ABI:                            ARM EABI\n"); break;
        default:        printf("  OS/ABI:                            Autre\n"); break;
    }

    /* ABI version */
    printf("  Version ABI:                       %u\n",header->e_ident[EI_ABIVERSION]);

    /* Type */
    switch (header->e_type) {
        case ET_REL:  printf("  Type:                              REL (Relocatable)\n"); break;
        case ET_EXEC: printf("  Type:                              EXEC (Executable)\n"); break;
        default:      printf("  Type:                              Autre\n"); break;
    }

    /* Machine */
    if (header->e_machine == EM_ARM)    printf("  Machine:                           ARM\n");

    /* Version */
    printf("  Version:                           0x%x\n", header->e_version);
    printf("  Adresse du point d'entrée:         0x%x\n", header->e_entry);
    printf("  Début des en-têtes de programme:   %u (bytes dans le fichier)\n", header->e_phoff);
    printf("  Début des en-têtes de section:     %u (bytes dans le fichier)\n", header->e_shoff);
    printf("  Fanions:                           0x%x\n", header->e_flags);
    printf("  Taille de cet en-tête:             %u (bytes)\n", header->e_ehsize);
    printf("  Taille entrée programme:           %u (bytes)\n", header->e_phentsize);
    printf("  Nombre entrées programme:          %u\n", header->e_phnum);
    printf("  Taille entrée section:             %u (bytes)\n", header->e_shentsize);
    printf("  Nombre entrées section:            %u\n", header->e_shnum);
    printf("  Table d'index des chaînes d'en-tête de section: %u\n", header->e_shstrndx);
    printf("\n");
}


