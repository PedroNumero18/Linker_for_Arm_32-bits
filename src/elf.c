#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf.h"
#include "utils.h"

elf32_t* elf_init() {
    elf32_t* elf = malloc(sizeof(elf32_t));
    if (!elf) error("Error allocating memory to pointer\n");
    
    memset(&elf->header, 0, sizeof(Elf32_Ehdr));
    elf->sections = NULL;
    elf->section_str_table = NULL;
    //elf->symbol_str_table = NULL;
    elf->table_symbole = NULL;
    elf->nb_symboles = 0;
    elf->rel_table = NULL;
    elf->nb_rel = 0;
    elf->RELA_table = NULL;
    elf->nb_RELA = 0;
    
    return elf;
}

void elf_free(elf32_t* elf){
    if (!elf) return;
    if (elf->sections) {
        for (int i = 0; i < elf->header.e_shnum; i++) {
            if (elf->sections[i].contenu) free(elf->sections[i].contenu);
        }
        free(elf->sections);
    }
    if (elf->table_symbole) free(elf->table_symbole);
    if (elf->rel_table)     free(elf->rel_table);
    if (elf->RELA_table)    free(elf->RELA_table);
    
    free(elf); 
}
