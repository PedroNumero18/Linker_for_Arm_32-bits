#include "elf_header.h"
#include <stdlib.h> 
#include <stdio.h> 




void lire_header(FILE* file, ELF* elf){
    if (file == NULL || elf == NULL){
        fprintf(stderr,"Erreur fichier ou d'initialisation d'elf\n");
        exit(1);
    }
    char bina
}