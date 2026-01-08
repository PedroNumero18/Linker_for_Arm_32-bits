#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf.h"

/* ====== Gestion des Structures ====== */

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

elf32_fusion_sections* fusion_sect_init(){
    elf32_fusion_sections* fusion = malloc(sizeof(elf32_fusion_sections));
    
    fusion->sections = NULL;
    fusion->nb_sections = 0;
    fusion->map_elf2 = NULL;
    fusion->nb_map = 0;
    fusion->shstrtab_orig = NULL;
    fusion->section_str_table = NULL;
    
    return fusion;
}

elf32_fusion_reimpl* fusion_reimpl_init(){
    elf32_fusion_reimpl* fusion = malloc(sizeof(elf32_fusion_reimpl));
    if (!fusion) error("Erreur allocation mémoire pour fusion réimplantation");

    fusion->nb_rel = 0;
    fusion->nb_rel_sections = 0;
    fusion->rel_table = NULL;
    fusion->rel_section_info = NULL;
    return fusion;
}

void free_fusion_reimpl(elf32_fusion_reimpl* fusion) {
    if (fusion) {
        free(fusion->rel_table);
        free(fusion->rel_section_info);
        free(fusion);
    }
}