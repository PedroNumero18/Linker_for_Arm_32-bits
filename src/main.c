#include <stdlib.h> 
#include <stdio.h>

#include "utils.h"
#include "elf.h"

char *filename;

int main(int argc,char *argv[]){
    if(argc <2) error("le nombre de fichier est incorrrect");
    
    elf32_t* elf = malloc(sizeof(elf32_t));
    if(!elf)error("Error allocating memory to pointer\n");
    
    FILE* inputFile = fopen(argv[1],"rb");
    if (!inputFile)error("Error opening file\n");
    filename = argv[1];

    lire_header(inputFile, elf);
    affichage_entete(&elf->header);
    lire_sections(inputFile, elf);
    afficher_sections(elf);
    lire_symbole(inputFile, elf);
    afficher_symboles(elf);
    
    free(elf);
    return 0;
}
