#include <stdlib.h> 
#include <stdio.h>

#include "elf_header.h"

void lire_header(FILE* file, ELF* elf){
    if (file == NULL || elf == NULL){
        fprintf(stderr,"Erreur fichier ou d'initialisation d'elf\n");
        exit(1);
    }
}