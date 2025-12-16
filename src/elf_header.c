#include <stdlib.h> 
#include <stdio.h>

#include "elf.h"

void lire_header(FILE* file, ELF* elf){
    /*
    Semantique à écrire
    */
    if (file == NULL || elf == NULL){
        fprintf(stderr,"Erreur fichier ou d'initialisation d'elf\n");
        exit(1);
    }
    //lecture ei indent + verif
    fseek(file,0,SEEK_SET);
    fread(&(elf->header),sizeof(Elf32_Ehdr),1,file);

    if (!((elf->header).e_ident[EI_MAG0] == ELFMAG0 && (elf->header).e_ident[EI_MAG1] == ELFMAG1 && (elf->header).e_ident[EI_MAG2] == ELFMAG2 && (elf->header).e_ident[EI_MAG3] == ELFMAG3)) {
        fprintf(stderr,"Erreur fichier, ce n'est pas un fichier ELF \n");
        exit(1);
    }

    if (!((elf->header).e_ident[EI_CLASS] == ELFCLASS32)) {
        fprintf(stderr,"Erreur fichier, ce n'est pas un e class 32\n");
        exit(1);
    }

    //si faut corriger l'endianess faudra faire plein de conversion mais pour le moment blk
    if (!((elf->header).e_ident[EI_DATA]).is_big_endian()) {
        //si c'est pas en big endian
        //on converti... TOUT
    }


}


void affichage_entete(Elf32_Ehdr* header){
    //Faudra changer pour que le texte corresponde mais pour l'instant j'ai ça
    if (header == NULL){
        fprintf(stderr,"Erreur d'initialisation d'header\n");
        exit(1);
    }

    /* Magic */
    printf("  Magique:   ");
    for (int i = 0; i < EI_NIDENT; i++)
        printf("%02x ", header->e_ident[i]);
    printf("\n");

    /* Classe */
    if (header->e_ident[EI_CLASS] == ELFCLASS32)
        printf("  Classe:                            ELF32\n");

    /* Endianness */
    if (header->e_ident[EI_DATA] == ELFDATA2MSB)
        printf("  Données:                           Big endian\n");
    else
        printf("  Données:                           Little endian\n");

    /* Version ELF */
    printf("  Version:                           %u (current)\n",
           header->e_ident[EI_VERSION]);

    /* OS / ABI */
    switch (header->e_ident[EI_OSABI]) {
        case EM_NONE:  printf("  OS/ABI:                            UNIX - System V\n"); break;
        case EM_M386:  printf("  OS/ABI:                            Linux\n"); break;
        case EM_ARM: printf("  OS/ABI:                            ARM EABI\n"); break;
        default: printf("  OS/ABI:                            Autre\n"); break;
    }

    /* ABI version */
    printf("  Version ABI:                       %u\n",
           header->e_ident[EI_ABIVERSION]);

    /* Type */
    switch (header->e_type) {
        case ET_REL:  printf("  Type:                              REL (Relocatable)\n"); break;
        case ET_EXEC: printf("  Type:                              EXEC (Executable)\n"); break;
        default:      printf("  Type:                              Autre\n"); break;
    }

    /* Machine */
    if (header->e_machine == EM_ARM)
        printf("  Machine:                           ARM\n");

    /* Version */
    printf("  Version:                           0x%x\n", header->e_version);
    printf("  Adresse du point d'entrée:         0x%x\n", header->e_entry);
    printf("  Début des en-têtes de programme:   %u\n", header->e_phoff);
    printf("  Début des en-têtes de section:     %u\n", header->e_shoff);
    printf("  Fanions:                           0x%x\n", header->e_flags);
    printf("  Taille de cet en-tête:             %u\n", header->e_ehsize);
    printf("  Taille entrée programme:           %u\n", header->e_phentsize);
    printf("  Nombre entrées programme:          %u\n", header->e_phnum);
    printf("  Taille entrée section:             %u\n", header->e_shentsize);
    printf("  Nombre entrées section:            %u\n", header->e_shnum);
    printf("  Index .shstrtab:                   %u\n", header->e_shstrndx);
}


