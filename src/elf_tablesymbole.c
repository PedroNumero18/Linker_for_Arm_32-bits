#include <stdlib.h> 
#include <stdio.h>
#include "utils.h"
#include "elf.h"

void lire_symbole(FILE* file, elf32_t* elf){
    uint8_t info=0;/*pour mieux gérer le cast plus tard*/
    if (file == NULL || elf == NULL){
        fprintf(stderr,"Erreur sur le fichier contenant les symbole\n");
        exit(1);
    }
    int taille_Tsym = (elf->header.e_shnum+elf->header.e_phnum)*2;
    elf->table_symbole = malloc(sizeof(Elf32_Sym)*(taille_Tsym));
    if (elf->table_symbole == NULL){
        fprintf(stderr,"Erreur malloc table des symboles \n");
        exit(1);
    }
    for(int i= 0; i <taille_Tsym; i++){
        get_32B(&(elf->table_symbole)[i].st_name, file);
        get_32B(&(elf->table_symbole)[i].st_value, file);
        get_32B(&(elf->table_symbole)[i].st_size, file);
        get_8B(&info,file);
        (elf->table_symbole)[i].st_info = (unsigned char) info;
        get_8B(&info,file);/*on stocke les 8 bits de Others ici*/
        (elf->table_symbole)[i].st_other = (unsigned char) info;
        get_16B(&(elf->table_symbole)[i].st_shndx, file);
    } 
}
