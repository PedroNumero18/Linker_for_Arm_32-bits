#include <stdlib.h> 
#include <stdio.h>

#include "elf.h"

void error(const char* msg){
    fprintf(stderr, "%s", msg);
    exit(1);
}

int main(int argc,char *argv[]){
    if(argc <2){
        error("le nombre de fichier est incorrrect");
    }
    
    elf32_t* elf = malloc(sizeof(elf32_t));
    if(!elf){
        error("Error allocating memory to pointer\n");
    }
    
    FILE* inputFile = fopen(argv[1],"rb");
    if (!inputFile){
        error("Error opening file\n");
    }

    lire_header(inputFile, elf);
    affichage_entete(&elf->header);
    
    free(elf);
    return 0;
}