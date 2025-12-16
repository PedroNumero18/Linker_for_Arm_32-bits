#include <stdlib.h> 
#include <stdio.h>

#include "elf.h"

void lire_sections(FILE* file, elf32_t* elf) {

   if (file == NULL || elf == NULL){
        fprintf(stderr,"Erreur fichier ou d'initialisation d'elf\n");
        exit(1);
    }
    fseek(file,elf->header.e_shoff,SEEK_SET);
    elf->sections = malloc(sizeof(Elf32_Shdr)*(elf->header.e.shnum));
    if (elf->sections == NULL){
        fprintf(stderr,"Erreur malloc sections\n");
        exit(1);
    }
    for(int i= 0; i <elf->header.e.shnum; i++){
        
    }
    free(elf->sections);

}

/*void affichage_section(elf32_t* elf) {

}*