#include <stdlib.h> 
#include <stdio.h>

#include "utils.h"
#include "elf.h"

const char* get_rel_type(uint32_t r_info){
    unsigned char type = ELF32_R_TYPE(r_info);
    switch(type){
        case 0: return "NONE";
        case 2 ... 11: return "STATIC";
        default: return "UNKNOWN";
    }
}

uint32_t get_rel_sym(uint32_t r_info){
    return ELF32_R_SYM(r_info);
}


void lire_Reimple(FILE* file, elf32_t* elf){
    if (file == NULL || elf == NULL) error("Erreur : fichier ou structure ELF NULL\n");

    //recherche section rel et rela
    int index_Rela = -1, index_rel = -1;

    for (int i = 0; i < elf->header.e_shnum; i++){
        uint32_t type = elf->sections[i].h_section.sh_type;
        if (type == SHT_REL)   index_rel = i;
        if (type == SHT_RELA)  index_Rela = i;
    }

    //init
    elf->rel_table = NULL;
    elf->RELA_table = NULL;
    elf->nb_rel = 0;
    elf->nb_RELA = 0;

    //rela
    if (index_Rela != -1){
        Elf32_Shdr s = elf->sections[index_Rela].h_section;
        int nb = s.sh_size / s.sh_entsize;
        if (nb > 0) {
            elf->RELA_table = malloc(sizeof(Elf32_Rela) * nb);
            elf->nb_RELA = 0;
            if (!elf->RELA_table) error("Impossible d'allouer RELA\n");
            fseek(file, s.sh_offset, SEEK_SET);
            for (int i = 0; i < nb; i++){
                get_32B(&elf->RELA_table[i].r_offset, file);
                get_32B(&elf->RELA_table[i].r_info,   file);
                get_32B((uint32_t*)&elf->RELA_table[i].r_addend, file);
            }
            elf->nb_RELA = nb;
        }
    }

    //rel
    if (index_rel != -1){
        Elf32_Shdr s = elf->sections[index_rel].h_section;
        int nb = s.sh_size / s.sh_entsize;
        if (nb > 0) {
            elf->rel_table = malloc(sizeof(Elf32_Rel) * nb);
            if (!elf->rel_table) error("Impossible d'allouer REL\n");
            fseek(file, s.sh_offset, SEEK_SET);
            for (int i = 0; i < nb; i++){
                get_32B(&elf->rel_table[i].r_offset, file);
                get_32B(&elf->rel_table[i].r_info, file);
            }
            elf->nb_rel = nb;
        }
    }
}


void afficher_Reimple(elf32_t* elf){
    if (!elf) return;

    //rel
    if (elf->rel_table && elf->nb_rel > 0){
        printf("\nTable REL:\n");
        for (uint32_t i = 0; i < elf->nb_rel; i++){
            Elf32_Rel r = elf->rel_table[i];
            printf("Offset: %08x Info: %08x Type: %s  Sym: %u\n",
                r.r_offset,
                r.r_info,
                get_rel_type(r.r_info),
                get_rel_sym(r.r_info));
        }
    } else {
        printf("Aucune table REL présente.\n");
    }

    //rela
    if (elf->RELA_table && elf->nb_RELA > 0){
        printf("\nTable RELA:\n");
        for (uint32_t i = 0; i < elf->nb_RELA; i++){
            Elf32_Rela r = elf->RELA_table[i];
            printf("Offset: %08x  Info: %08x  Type: %s  Sym: %u  Addend: %d\n",
                r.r_offset,
                r.r_info,
                get_rel_type(r.r_info),
                get_rel_sym(r.r_info),
                r.r_addend);
        }
    } else {
        printf("Aucune table RELA présente.\n");
    }
}
