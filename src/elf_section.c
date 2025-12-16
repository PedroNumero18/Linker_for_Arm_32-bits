#include <stdlib.h> 
#include <stdio.h>

#include "elf.h"

void lire_section(FILE* file, elf32_t* elf) {
    /*
    
    */
   if (file == NULL || elf == NULL){
        fprintf(stderr,"Erreur fichier ou d'initialisation d'elf\n");
        exit(1);
    }
}

/*void affichage_section(elf32_t* elf) {

}*/